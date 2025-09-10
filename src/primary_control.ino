/* Dive deeper into the protocol for CAN 
-Modeling it now after actual use case with primary and external control system signals 
being sent to and from the CAN bus, Modeling the Primary control system using a potentiemteer 
as a throttle and LEDS to signify what gear and how fast in that geaer based on blinking then we will use 
other external control system to "take control" 
Author:Diego Klish 
*/
/*
Modeling the Primary control system using a potentiemteer as a throttle and LEDS to signify what gear and how fast in that geaer based on blinking 
Author:Diego Klish 
*/
#include <FlexCAN_T4.h>
#define POTENTIOMETER_PIN A0
#define FORWARD_LED 10
#define NEUTRAL_LED 9
#define REVERSE_LED 8

void setup(){
  Serial.begin(115200); delay(400);
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(FORWARD_LED, OUTPUT);
  pinMode(NEUTRAL_LED, OUTPUT);
  pinMode(REVERSE_LED, OUTPUT);
}

void loop(){
   int potValue = analogRead(POTENTIOMETER_PIN);
   //Serial.print(potValue);
  
  if(potValue >= 612){
    int distance_from_min = potValue - 612;
    int forward_range = 1023 - 612;
    int throttle_percent = map(distance_from_min, 0, forward_range, 0, 100);
    int blinkInterval = map(throttle_percent, 0, 100, 500, 50);
    
    if(millis() - lastBlink >= blinkInterval){//function to make blink faster closer to interval it gets 
      ledState = !ledState;
      lastBlink = millis();
    }
  }
  else if(potValue < 612 && potValue > 412){
    analogWrite(NEUTRAL_LED, 255);
  }
  else{
    int distance_from_max = 412 - potValue;        // How far from start of reverse zone
    int reverse_range = 412 - 0;                   // Total reverse range (412)
    int throttle_percent = map(distance_from_max, 0, reverse_range, 0, 100);
    
    // Blink interval: 500ms at 0% down to 50ms at 100%
    int blinkInterval = map(throttle_percent, 0, 100, 500, 50);
    
    if(millis() - lastBlink >= blinkInterval){//function to make blink faster closer to interval it gets 
      ledState = !ledState;
      lastBlink = millis();
    }
    analogWrite(REVERSE_LED, ledState ? 255 : 0);
  }
  delay(10);  // Small delay for stability
  
}