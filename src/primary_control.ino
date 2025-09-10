/* Dive deeper into the protocol for CAN 
-Modeling it now after actual use case with primary and external control system signals 
being sent to and from the CAN bus, Modeling the Primary control system using a potentiemteer 
as a throttle and LEDS to signify what gear and how fast in that geaer based on blinking then we will use 
other external control system to "take control" 
Author:Diego Klish 
*/

#include <FlexCAN_T4.h>
#define POTENTIOMETER_PIN A0
#define FORWARD_LED 10
#define NEUTRAL_LED 9
#define REVERSE_LED 8

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;//define CAN bus 

#define PRIMARY_STATUS_ID    0x4B4  // System Status from Primary Control
#define PRIMARY_PROP_ID      0x4B2  // Propulsion Status from Primary

unsigned long lastBlink = 0;  // Tracks when we last blinked
bool ledState = false;
unsigned long lastCANSend = 0;

void setup(){
  Serial.begin(115200); delay(400);
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(FORWARD_LED, OUTPUT);
  pinMode(NEUTRAL_LED, OUTPUT);
  pinMode(REVERSE_LED, OUTPUT);

  can1.begin();
  can1.setBaudRate(250000);

}

void loop(){
  int potValue = analogRead(POTENTIOMETER_PIN);//takes in our potentiometer value 
  
  handleLEDs();//runs our LED function 

  if(millis() - lastCANSend >= 100){ //send can messages with our Potentiometer value 
    sendCANMessages(potValue);
    lastCANSend = millis();
  }
  delay(10);
}

void handleLEDs(){
   int potValue = analogRead(POTENTIOMETER_PIN);
   //Serial.print(potValue);
    analogWrite(FORWARD_LED, 0);
    analogWrite(NEUTRAL_LED, 0);  // <-- This turns off neutral
    analogWrite(REVERSE_LED, 0);

  if(potValue >= 612){
    int distance_from_min = potValue - 612;
    int forward_range = 1023 - 612;
    int throttle_percent = map(distance_from_min, 0, forward_range, 0, 100);
    int blinkInterval = map(throttle_percent, 0, 100, 500, 50);
    
    if(millis() - lastBlink >= blinkInterval){//function to make blink faster closer to interval it gets 
      ledState = !ledState;
      lastBlink = millis();
    }
     analogWrite(FORWARD_LED, ledState ? 255 : 0);
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

void sendCANMessages(int potValue){
  // Just send throttle percentage - no direction needed
  uint8_t throttle = 0x00;   // Default 0%
  
  if(potValue >= 612){
    // Forward zone
    int distance_from_min = potValue - 612;
    int forward_range = 1023 - 612;
    int throttle_percent = map(distance_from_min, 0, forward_range, 0, 100);
    throttle = map(throttle_percent, 0, 100, 0x00, 0xFA);
  }
  else if(potValue < 612 && potValue > 412){
    // Neutral zone
    throttle = 0x00;   // 0%
  }
  else{
    // Reverse zone  
    int distance_from_max = 412 - potValue;
    int reverse_range = 412 - 0;
    int throttle_percent = map(distance_from_max, 0, reverse_range, 0, 100);
    throttle = map(throttle_percent, 0, 100, 0x00, 0xFA);
  }
  
  // Send Primary Status Message (ID 0x4B4) - just allow control
  CAN_message_t status_msg;
  status_msg.id = PRIMARY_STATUS_ID;
  status_msg.flags.extended = 0;
  status_msg.len = 8;
  status_msg.buf[0] = 0x00;  // Allow ECS control
  for(int i = 1; i < 8; i++) status_msg.buf[i] = 0xFF;  // Fill rest with 0xFF
  can1.write(status_msg);
  
  // Send just throttle position (ID 0x4B2)  
  CAN_message_t prop_msg;
  prop_msg.id = PRIMARY_PROP_ID;
  prop_msg.flags.extended = 0;
  prop_msg.len = 8;
  prop_msg.buf[0] = throttle;    // Just the throttle value
  for(int i = 1; i < 8; i++) prop_msg.buf[i] = 0xFF;  // Fill rest with 0xFF
  can1.write(prop_msg);
}