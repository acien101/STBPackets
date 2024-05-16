#ifndef STBPacket_H
#define STBPacket_H

#include <Arduino.h>

#define STBP_MAX_USERDATA_LENGTH 255 // Bytes

// Define LENGTH
#define STBP_HEADER_LENGTH_B 2    // Bytes
#define STBP_SECHEADER_LENGTH_B 4 // Bytes
#define STBP_CRC_LENGTH_B 2 // Bytes

// Define sizes
#define STBP_TYPE_SIZE 1        // Bits
#define STBP_APID_SIZE 6        // Bits
#define STBP_SECH_SIZE 1        // Bits
#define STBP_LENGTH_SIZE 8      // Bits

// Define OFFSETS
#define STBP_TYPE_OFFSET 15
#define STBP_APID_OFFSET 9
#define STBP_SECH_OFFSET 8
#define STBP_LENGTH_OFFSET 0

// Define specific Data Field User Packets

#define STBP_TM_RTDDATA_LENGTH_B 18   // Bytes
#define STBP_TM_RTDDATA_OFFSET 24     // Offset in bits of each element

#define STBP_TM_IADCDATA_LENGTH_B 20 // Bytes
#define STBP_TM_IADCDATA_NUM_ELEMENTS 10 // 16 bits * 10 elements
#define STBP_TM_IADCDATA_BYTES_ELEMENT 2

#define STBP_TM_LSDATA_LENGTH_B 2    // Bytes

// Define ENUMS

typedef enum {
  STBP_TYPE_TELEMETRY = 0,
  STBP_TYPE_TELECOMMAND = 1
} STBP_TYPE_E;    // TYPE ENUM

typedef enum {
  STBP_SECH_NOTPRESENT = 0,
  STBP_SECH_PRESENT = 1
} STBP_SECH_E;  // Secondary header ENUM

typedef enum {
  STBP_APID_TM_LSDATA = 0,
  STBP_APID_TM_MUXDATA = 1,
  STBP_APID_TM_RTDDATA = 2,
  STBP_APID_TM_IADCDATA = 3,        // Internal ADC Data
} STBP_APID_TM_E;  // APID ENUM

// Structures types

typedef struct STBP_HEADER_S{
  uint8_t type : 1;     // 0:1
  uint8_t apid : 6;     // 1:7
  uint8_t sech : 1;     // 7:8
  uint8_t length;       // 8:15
} STBP_HEADER_S;  // Header STRUCTURE

typedef struct STBP_SECH_S{
  unsigned long time;
} STBP_SECH_S;  // Secondary Header Structure

typedef unsigned char uint24_t[3];

class STBPacket {
    STBP_HEADER_S primHeader;
    STBP_SECH_S secHeader;
    uint8_t userDataBuff[STBP_MAX_USERDATA_LENGTH];
  public:
    STBPacket();
    STBPacket(STBP_HEADER_S primHeader, STBP_SECH_S secHeader);
    uint8_t setPrimHeader(STBP_HEADER_S primHeader);
    uint8_t setSecHeader(STBP_SECH_S secHeader);
    void setRTDData(uint32_t rtd0ch0, uint32_t rtd0ch1, uint32_t rtd0ch2, 
                    uint32_t rtd1ch0, uint32_t rtd1ch1, uint32_t rtd1ch2);
    void setInternalADCData(uint16_t* adcData);
    void setLSData(uint16_t data);
    uint8_t setUserData(uint8_t* data, size_t size);
    uint8_t buildPacket(uint8_t* dst);
  private:
    uint8_t getUserDataLength();
    uint16_t gen_checksum(uint8_t const *data, int size);
};

#endif /* STBPacket_H */