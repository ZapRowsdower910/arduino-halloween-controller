/*
HC-SR04 Ping distance sensor]
VCC to arduino 5v GND to arduino GND
Echo to Arduino pin 13 Trig to Arduino pin 12
Red POS to Arduino pin 11
Green POS to Arduino pin 10
560 ohm resistor to both LED NEG and GRD power rail
More info at: http://goo.gl/kJ8Gl
Original code improvements to the Ping sketch sourced from Trollmaker.com
Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
*/

/*
 * NEC Codes
 * LEFT - 0xFD50AF
 * 1 - 0xFD08F7
 * 2 - 0xFD8877
 * 3 - 0xFD48B7
 * 4 - 0xFD28D7
 * 
 * Sonar Sensor
 * 500cm is max range on sensor
 * 500cm ~ 16.5ft
 * 200cm ~ 6.5ft
 * 100cm ~ 3.2ft
 * 50 ~ 1.5ft
 * 25 ~ 9in
 */

#include <NewPing.h>
#include <IRremote.h>

#define TRIG_PIN 12
#define ECHO_PIN 11
#define MAX_DISTANCE 2000 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define PIR_PIN 8
#define LED_PIN 13

IRsend irsend;
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


boolean needsReset = false;
boolean sendLatch = false;
boolean previousOutOfRange = false;

unsigned long latchedStart = millis();
uint32_t latchLength = 500;  // Millis

int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
 

// Set stuff up
void setup() {
  Serial.begin (9600);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Started");
}


void loop() {
  unsigned long duration, distance;

  val = digitalRead(PIR_PIN);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(LED_PIN, HIGH);  // turn LED ON
    distance = sonar.ping_median() / 148;
    Serial.println(distance);
      if(distance < 70 && distance != 0) {
        Serial.println("State 2 - HIGH!");
        triggerBlast(0xFD48B7);
        
      } else {
        

        Serial.println("State 1 - low");
        triggerBlast(0xFD8877);
      }
      
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
      
    } // implied else - pirState is high
    
  } else {
    digitalWrite(LED_PIN, LOW); // turn LED OFF
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;

      irsend.sendNEC(0xFD08F7, 32);
    }
  }
  
  // yo brah, can you hear meh?
//  if(!sendLatch){

    // HELLO HELlo Hello hell he..
//    duration = checkDistance();
//      duration = sonar.ping_median(20);
//      distance = (duration/2) / 29.1;
//      distance = duration / 148;
//      distance = (duration/10);
//      distance = sonar.ping_median();
//      distance = sonar.ping_in();
//      Serial.println(distance);
  
//    Serial.print(distance); Serial.println(" cm");

//    if(distance <= 500 && distance > 200) {
//    if(distance <= 11000 && distance > 1000) {
//      Serial.println("State 1");
//      triggerBlast(0xFD8877);
//      
//    } else if(distance <= 1000 && distance != 0){
//      Serial.println("State 2");
//      triggerBlast(0xFD48B7);
//
////    } else if(distance > 200 && distance < 1000){
////      Serial.println("State 3");
////      irsend.sendNEC(0xFD08F7, 32);
//      
//    } else {
//      Serial.print("Out of range - raw value: "); Serial.println(distance);
//      
//      if(previousOutOfRange && needsReset){
//        Serial.println("sending reset");
//        irsend.sendNEC(0xFD08F7, 32);
//        needsReset = false;
//      }
//      previousOutOfRange = true;
////      if(!sendLatch){
////        irsend.sendNEC(0xFD08F7, 32);
////        sendLatch = true;
////      }
//    }
    
//  } else if(millis() - latchedStart > latchLength){
//    Serial.print("unlatching: "); Serial.println(millis() - latchedStart);
//    // Latch time has past, release latch to allow new re-triggering
////    if(!previousOutOfRange){
////      irsend.sendNEC(0xFD08F7, 32);  
////    }
//    sendLatch = false;
//  }

//  previousOutOfRange = sendLatch;
}

// Sonar stuff
//long checkDistance(){
//  // Ping
//  digitalWrite(trigPin, LOW);
//  delayMicroseconds(2);
//  // Pong
//  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(10);
//  // Ping
//  digitalWrite(trigPin, LOW);
//
//  //  Oh no you sunk my battle shit!
//  return pulseIn(echoPin, HIGH);
//}

// Pew Pew Pew
void triggerBlast(uint32_t hexCode){
  Serial.print("Latching and sending IRCode: "); Serial.println(hexCode, HEX);
  // Send IR code
  irsend.sendNEC(hexCode, 32);
  // Set flag to latched
  sendLatch = true;
  // set flag to indicate we need to reset lamps back to smoldering
  needsReset = true;
  // set flag indicating previous run found motion
  previousOutOfRange = false;
  // Figure out when we should release the latch if the fuuuuture
  latchedStart = millis();
}

