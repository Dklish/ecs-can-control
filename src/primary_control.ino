/* Dive deeper into the protocol for CAN 
-Modeling it now after actual use case with primary and external control system signals 
being sent to and from the CAN bus, Modeling the Primary control system using a potentiemteer 
as a throttle and LEDS to signify what gear and how fast in that geaer based on blinking then we will use 
other external control system to "take control" 
Author:Diego Klish 
*/

#include <FlexCAN_T4.h>
#define POTENTIOMETER_PIN A0
#define LED_PIN 10

void setup(){
  Serial.begin(115200); delay(400);
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop(){
   int potValue = analogRead(POTENTIOMETER_PIN);
   //Serial.print(potValue);
  
  int brightness = map(potValue, 0, 1023, 0, 255);

  analogWrite(LED_PIN, brightness);

  delay(10);  // Small delay for stability
}