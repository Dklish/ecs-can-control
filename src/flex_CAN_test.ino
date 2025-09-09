/* Intial code to test two diffirent can bus on the teensy commmunicating with one another over high and low CAN lines 
-familiarzing myself with the flex can library as well as the CAN protocol 
-this program starts two can controllers and has one sending and one recieving from the other 
Author:Diego Klish 
*/

#include <FlexCAN_T4.h> //includes our library for can network for teensy three can busses 

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1; //first CAN controller 256 message receive buffer, 16 message transmit buffer 
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2; //second CAN controller  
CAN_message_t msg; //structure to hold CAN message data 

void setup(void) {
  Serial.begin(115200); 
  delay(2000);
  
  can1.begin(); //initialize can bus 1 
  can1.setBaudRate(250000); //set baud rate 
  Serial.println("CAN1 initialized");
  
  can2.begin(); //initialize can bus 2 
  can2.setBaudRate(250000); //set baud rate 
  Serial.println("CAN2 initialized"); 
  
}

void loop() {
  
   static unsigned long lastSend = 0;
  if (millis() - lastSend > 3000) {
    CAN_message_t testMsg;
    testMsg.id = 0x123; //message id length 
    testMsg.len = 8; //message byte length 
    testMsg.buf[0] = 0xAA;
    testMsg.buf[1] = 0xBB;
    testMsg.buf[2] = 0xCC;
    testMsg.buf[3] = 0xDD;
    testMsg.buf[4] = 0xEE;
    testMsg.buf[5] = 0xFF;
    testMsg.buf[6] = 0x11;
    testMsg.buf[7] = 0x22;
    
    can1.write(testMsg);  // Send on CAN1
    Serial.println("Test message sent on CAN1");
    lastSend = millis();
  }

  
  if (can1.read(msg)) { //if CAN1 receives the message print the id and length of the message 
    Serial.print("CAN1 - ID: 0x"); 
    Serial.print(msg.id, HEX);
    Serial.print(" LEN: ");
    Serial.print(msg.len);
    Serial.print(" DATA: ");
    for (uint8_t i = 0; i < msg.len; i++) {//loops through length of mesage and prints the bytes in hex 
      Serial.print("0x");
      Serial.print(msg.buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  if (can2.read(msg)) { //if CAN2 gets message from CAN1 prints id and length of message 
    Serial.print("CAN2 - ID: 0x"); 
    Serial.print(msg.id, HEX);
    Serial.print(" LEN: ");
    Serial.print(msg.len);
    Serial.print(" DATA: ");
    for (uint8_t i = 0; i < msg.len; i++) {//loops through msg hex values and prints them 
      Serial.print("0x");
      Serial.print(msg.buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}