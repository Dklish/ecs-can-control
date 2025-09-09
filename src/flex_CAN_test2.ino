/* Dive deeper into the protocol for CAN 

Author:Diego Klish 
*/
#include <FlexCAN_T4.h> 

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1; //first CAN controller 256 message receive buffer, 16 message transmit buffer 
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2; //second CAN controller  
CAN_message_t msg;

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
