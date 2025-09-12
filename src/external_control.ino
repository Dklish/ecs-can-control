/*  External Control System for CAN Bus Communication Protocol
-This external control system receives and processes CAN messages from the primary controller and provides 
manual throttle command interface via serial when the primary 
-System allows control handoff. Demonstrates bi-directional CAN bus communication 
patterns commonly used in industrial control applications.
Author:Diego Klish 
*/
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;//setup CAN bus

//Message IDs
#define PRIMARY_STATUS_ID    0x4B4
#define ECS_PROP_CMD_ID      0x4B3

// Control variables
bool primary_allows_control = false;
uint8_t stbd_throttle = 0x00;    // 0-0xFA (0-100%)
uint8_t port_throttle = 0x00;

void setup() {
  Serial.begin(115200);

  can1.begin();
  can1.setBaudRate(250000);

}

void loop() {
  // Read CAN messags
  CAN_message_t msg;
  while (can1.read(msg)) {
    if (msg.id == PRIMARY_STATUS_ID) {
      Serial.print("Status message buf[0]: 0x");  // Add this line
      Serial.println(msg.buf[0], HEX); 

      bool was_allowed = primary_allows_control;
      primary_allows_control = (msg.buf[0] == 0x00);
      
      if (was_allowed != primary_allows_control) {
        Serial.print("Control: ");
        Serial.println(primary_allows_control ? "ACTIVE" : "INACTIVE");
      }
    }
    else if (msg.id == 0x4B2) {  // PRIMARY_PROP_ID
      uint8_t primary_throttle = msg.buf[0];
      int throttle_percent = map(primary_throttle, 0x00, 0xFA, 0, 100);
      Serial.print("Primary sending throttle: ");
      Serial.print(throttle_percent);
      Serial.println("%");
    }
  }

  // Handle commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();
  
    if (cmd.startsWith("st")) {
      int val = constrain(cmd.substring(2).toInt(), 0, 100);
      stbd_throttle = map(val, 0, 100, 0x00, 0xFA);
      Serial.printf("STBD throttle: %d%%\n", val);
    }
    else if (cmd.startsWith("pt")) {
      int val = constrain(cmd.substring(2).toInt(), 0, 100);
      port_throttle = map(val, 0, 100, 0x00, 0xFA);
      Serial.printf("PORT throttle: %d%%\n", val);
    }
  }
  
  // Send throttle commands when allowed
  static unsigned long lastSend = 0;
  if (primary_allows_control && millis() - lastSend >= 100) {
    CAN_message_t prop_msg;
    prop_msg.id = ECS_PROP_CMD_ID;
    prop_msg.flags.extended = 0;
    prop_msg.len = 8;
    
    prop_msg.buf[0] = 0x7D;           // PORT Direction: Neutral
    prop_msg.buf[1] = 0x7D;           // STBD Direction: Neutral
    prop_msg.buf[2] = 0xFF;
    prop_msg.buf[3] = 0xFF;
    prop_msg.buf[4] = port_throttle;  // PORT Throttle
    prop_msg.buf[5] = stbd_throttle;  // STBD Throttle
    prop_msg.buf[6] = 0xFF;
    prop_msg.buf[7] = 0xFF;
    
    can1.write(prop_msg);
    lastSend = millis();
  }
}