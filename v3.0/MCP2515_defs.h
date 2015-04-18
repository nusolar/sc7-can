/*
  MCP2515_defs.h - Library for Microchip MCP2515 CAN Controller
  
  Author: David Harding
  
  Created: 11/08/2010
  
  For further information see:
  
  http://ww1.microchip.com/downloads/en/DeviceDoc/21801e.pdf
  http://en.wikipedia.org/wiki/CAN_bus
*/

#ifndef MCP2515_defs_h
#define MCP2515_defs_h

#include <stdint.h>
#include "Arduino.h"

typedef struct
{
      unsigned long id;      // EID if ide set, SID otherwise
      byte srr = 0;                  // Standard Frame Remote Transmit Request
      byte rtr = 0;                  // Remote Transmission Request
      byte ide = 0;                  // Extended ID flag
      byte dlc = 0;                  // Number of data bytes
      union {
        // 8 bytes
        uint64_t value;
        // 8 bytes (signed)
        int64_t  value_s;
        // 4 bytes (float)
        struct {
          float low_f;
          float high_f;
        };
        // 4 bytes (int)
        struct {
          uint32_t low;
          uint32_t high;
        };
        // 4 bytes (signed int)
        struct {
          signed long  low_s;
          signed long  high_s;
        };
        // 2 bytes
        struct {
          uint16_t s0;
          uint16_t s1;
          uint16_t s2;
          uint16_t s3;
        };
        // 2 bytes (signed)
        struct {
          signed int i0;
          signed int i1;
          signed int i2;
          signed int i3;
        };
        // 1 byte
        uint8_t data[8];
      };
} Frame;
      

// MCP2515 SPI Commands
#define CAN_RESET       0xC0
#define CAN_READ        0x03
#define CAN_WRITE       0x02
#define CAN_RTS         0x80
#define CAN_STATUS      0xA0
#define CAN_BIT_MODIFY  0x05  
#define CAN_RX_STATUS   0xB0
#define CAN_READ_BUFFER 0x90
#define CAN_LOAD_BUFFER 0X40  

// Register Bit Masks
// CANSTAT
#define MODE_CONFIG            0x80
#define MODE_LISTEN            0x60
#define MODE_LOOPBACK          0x40
#define MODE_SLEEP             0x20
#define MODE_NORMAL            0x00
// CANINTF
#define RX0IF                  0x01
#define RX1IF                  0x02
#define TX0IF                  0x04
#define TX1IF                  0x08
#define TX2IF                  0x10
#define ERRIF                  0x20
#define WAKIF                  0x40
#define MERRF                  0x80
#define INTALL                 0xFF

// CANINTE
#define RX0IE                  0x01
#define RX1IE                  0x02
#define TX0IE                  0x04
#define TX1IE                  0x08
#define TX2IE                  0x10
#define ERRIE                  0x20
#define WAKIE                  0x40
#define MERRE                  0x80

// Configuration Registers
#define CANSTAT         0x0E
#define CANCTRL         0x0F
#define BFPCTRL         0x0C
#define TEC             0x1C
#define REC             0x1D
#define CNF3            0x28
#define CNF2            0x29
#define CNF1            0x2A
#define CANINTE         0x2B
#define CANINTF         0x2C
#define EFLG            0x2D
#define TXRTSCTRL       0x0D

// TX Buffer 0
#define TXB0CTRL        0x30
#define TXB0SIDH        0x31
#define TXB0SIDL        0x32
#define TXB0EID8        0x33
#define TXB0EID0        0x34
#define TXB0DLC         0x35
#define TXB0D0          0x36
#define TXB0D1          0x37
#define TXB0D2          0x38
#define TXB0D3          0x39
#define TXB0D4          0x3A
#define TXB0D5          0x3B
#define TXB0D6          0x3C
#define TXB0D7          0x3D
                         
// TX Buffer 1
#define TXB1CTRL        0x40
#define TXB1SIDH        0x41
#define TXB1SIDL        0x42
#define TXB1EID8        0x43
#define TXB1EID0        0x44
#define TXB1DLC         0x45
#define TXB1D0          0x46
#define TXB1D1          0x47
#define TXB1D2          0x48
#define TXB1D3          0x49
#define TXB1D4          0x4A
#define TXB1D5          0x4B
#define TXB1D6          0x4C
#define TXB1D7          0x4D

// TX Buffer 2
#define TXB2CTRL        0x50
#define TXB2SIDH        0x51
#define TXB2SIDL        0x52
#define TXB2EID8        0x53
#define TXB2EID0        0x54
#define TXB2DLC         0x55
#define TXB2D0          0x56
#define TXB2D1          0x57
#define TXB2D2          0x58
#define TXB2D3          0x59
#define TXB2D4          0x5A
#define TXB2D5          0x5B
#define TXB2D6          0x5C
#define TXB2D7          0x5D
                         
// RX Buffer 0
#define RXB0CTRL        0x60
#define RXB0SIDH        0x61
#define RXB0SIDL        0x62
#define RXB0EID8        0x63
#define RXB0EID0        0x64
#define RXB0DLC         0x65
#define RXB0D0          0x66
#define RXB0D1          0x67
#define RXB0D2          0x68
#define RXB0D3          0x69
#define RXB0D4          0x6A
#define RXB0D5          0x6B
#define RXB0D6          0x6C
#define RXB0D7          0x6D
                         
// RX Buffer 1
#define RXB1CTRL        0x70
#define RXB1SIDH        0x71
#define RXB1SIDL        0x72
#define RXB1EID8        0x73
#define RXB1EID0        0x74
#define RXB1DLC         0x75
#define RXB1D0          0x76
#define RXB1D1          0x77
#define RXB1D2          0x78
#define RXB1D3          0x79
#define RXB1D4          0x7A
#define RXB1D5          0x7B
#define RXB1D6          0x7C
#define RXB1D7          0x7D

// Buffer Bit Masks
#define RXB0            0x00
#define RXB1            0x02
#define TXB0            0x01
#define TXB1            0x02
#define TXB2            0x04
#define TXB_ALL                  TXB0 | TXB1 | TXB2

// Buffer Full Flags
#define RXB0Full        0x01;
#define RXB1Full        0x02;
#define RXB_All                  RXB1Full | RXB2Full

// filters for RX buffers (0-1 -> RXB0, 2-5 -> RXB1)
#define RXF0SIDH   0x00
#define RXF0SIDL   0x01
#define RXF1SIDH   0x04
#define RXF1SIDL   0x05
#define RXF2SIDH   0x08
#define RXF2SIDL   0x09
#define RXF3SIDH   0x10
#define RXF3SIDL   0x11
#define RXF4SIDH   0x14
#define RXF4SIDL   0x15
#define RXF5SIDH   0x18
#define RXF5SIDL   0x19

// masks for RX buffers
#define RXM0SIDH 0x20
#define RXM0SIDL 0x21
#define RXM1SIDH 0x24
#define RXM1SIDL 0x25

#endif
