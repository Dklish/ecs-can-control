# Dual-Teensy CAN Bus Control System

A dual-microcontroller CAN bus implementation demonstrating industrial control protocols for embedded systems development and learning.

## System Overview

This project demonstrates a complete control system architecture using two Teensy 4.0 microcontrollers communicating over CAN bus at 250kbps, implementing industry-standard communication patterns commonly used in automotive, marine, and industrial applications.

## Components

### Primary Controller (Teensy #1)
- Potentiometer-based throttle input with three operational zones
- Visual feedback system using three LEDs with variable blink rates
- Real-time CAN message transmission of control status and throttle data
- Serial debugging output for system monitoring

### External Control System - ECS (Teensy #2)
- CAN message reception and processing
- Manual throttle override capability via serial interface
- Automatic control handoff when permitted by primary controller
- Bi-directional communication with primary system

## Hardware Requirements

### For Each Teensy System:
- Teensy 4.0 microcontroller
- MCP2551 or TJA1050 CAN transceiver
- 120Ω termination resistors for CAN bus
- Common ground connection between systems

### Primary Controller Additional Hardware:
- 10kΩ potentiometer (center to A0, outer pins to 3.3V/GND)
- 3 LEDs (Red, Orange, Green) with 330Ω current limiting resistors
- Connections: Red→Pin 8, Orange→Pin 10, Green→Pin 9

### CAN Bus Wiring:
- Teensy Pin 22 (CAN1_TX) → Transceiver CTX
- Teensy Pin 23 (CAN1_RX) → Transceiver CRX
- Both CANH lines connected together
- Both CANL lines connected together
- 120Ω termination at both ends of bus

## CAN Bus Protocol

| Parameter | Value |
|-----------|-------|
| **Standard CAN** | Uses 11-bit identifiers (not extended 29-bit) |
| **Message Rate** | 100ms intervals (10Hz) |
| **CAN Bus Baud Rate** | 250,000 bps |
| **Serial Debug Baud Rate** | 115,200 bps |
| **Data Format** | 8-byte CAN frames with custom data structure |

### Custom Message IDs:
- `0x4B4`: Primary status messages
- `0x4B2`: Primary throttle data
- `0x4B3`: ECS throttle commands

## Control Logic

The system implements a safe control handoff protocol:

1. Primary controller continuously sends status and throttle data
2. ECS monitors for control permission (data byte 0 = 0x00)
3. When permitted, ECS can send override throttle commands
4. Primary can reclaim control at any time

## Installation & Setup

### Prerequisites
- Arduino IDE with Teensy support
- FlexCAN_T4 library installed

### Upload Instructions
1. Connect Primary Controller Teensy via USB
2. Upload `primary_control.ino`
3. Connect ECS Teensy via USB
4. Upload `external_controller.ino`
5. Connect both systems to CAN bus with proper termination

## Operation

### Primary Controller
**Potentiometer Zones:**
- **Forward:** 612-1023 (Orange LED, blinks faster approaching max)
- **Neutral:** 412-612 (Green LED, solid)
- **Reverse:** 0-412 (Red LED, blinks faster approaching min)

### ECS Commands
Type in serial monitor connected to ECS Teensy:

```
st<0-100>    Set starboard throttle percentage
pt<0-100>    Set port throttle percentage
```

**Example:** `st75` sets starboard to 75%

## System Status

Both systems provide real-time serial debug output showing:
- CAN message reception/transmission
- Current throttle values and control status
- Operational mode and system health

## Safety Features

- ✅ **Dead zone implementation:** 200-point neutral zone prevents accidental gear changes
- ✅ **Control permission protocol:** ECS only operates when explicitly allowed
- ✅ **Primary override capability:** Manual controls always maintain ultimate authority
- ✅ **Continuous status monitoring:** Real-time feedback prevents communication failures

## Troubleshooting

### No CAN Communication:
- Verify 120Ω termination resistors installed
- Check CANH/CANL wiring between transceivers
- Confirm common ground between both systems

### Serial Commands Not Working:
- Verify correct COM port selected for ECS Teensy
- Check baud rate set to 115200
- Ensure no duplicate `Serial.available()` calls in code

### LEDs Not Responding:
- Check resistor values (330Ω recommended)
- Verify LED polarity (anode to Teensy pin)
- Test potentiometer with multimeter (should read 0-3.3V on center pin)

## Technical Notes

This implementation demonstrates core concepts used in industrial control systems including:

- Industry-standard CAN bus communication protocols
- Safe control transfer mechanisms
- Real-time system monitoring and feedback
- Redundant control architectures for critical applications

The code serves as an educational foundation for understanding distributed control systems and can be extended for additional features like sensor integration, data logging, or integration with higher-level control systems.

## Circuit Diagram

```
Primary Controller (Teensy #1)          ECS Controller (Teensy #2)
┌─────────────────────────┐            ┌─────────────────────────┐
│  Pin 22 (CAN1_TX) ──────┼────────────┼────── Pin 22 (CAN1_TX) │
│  Pin 23 (CAN1_RX) ──────┼────────────┼────── Pin 23 (CAN1_RX) │
│                         │            │                         │
│  Pin A0 ── Potentiometer│            │                         │
│  Pin 8  ── Red LED      │            │                         │
│  Pin 9  ── Green LED    │            │                         │
│  Pin 10 ── Orange LED   │            │                         │
└─────────────────────────┘            └─────────────────────────┘
           │                                      │
           │              CAN Bus                │
           └──────────────────────────────────────┘
                    (120Ω termination at each end)
```

---

**Author:** Diego Klish   
