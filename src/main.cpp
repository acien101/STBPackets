#include <Arduino.h>

#include "STBPacket.h"

void sendRTDData();
void sendInternalADCData();
void sendLSData();

void setup() {
  Serial.begin(9600);
  
  //sendRTDData();
  //sendInternalADCData();
  sendLSData();
}

void loop() {
  // put your main code here, to run repeatedly:
}

void sendRTDData(){
  // Calculate length
  size_t lengthField = STBP_SECHEADER_LENGTH_B + STBP_TM_RTDDATA_LENGTH_B + STBP_CRC_LENGTH_B - 1;

  // Create Header
  STBP_HEADER_S primHeader = {STBP_TYPE_TELEMETRY, STBP_APID_TM_RTDDATA, STBP_SECH_PRESENT, lengthField};

  STBP_SECH_S secHeader = {micros()};

  uint8_t packet_buff[STBP_HEADER_LENGTH_B + lengthField + 1] = {0};

  STBPacket myPacket = STBPacket(primHeader, secHeader);
  myPacket.setRTDData(1, 2, 3, 4, 5, 6);

  myPacket.buildPacket(packet_buff);
  Serial.write(packet_buff, sizeof(packet_buff));
}

void sendInternalADCData(){
    // Calculate length
  size_t lengthField = STBP_SECHEADER_LENGTH_B + STBP_TM_IADCDATA_LENGTH_B + STBP_CRC_LENGTH_B - 1;

  // Create Header
  STBP_HEADER_S primHeader = {STBP_TYPE_TELEMETRY, STBP_APID_TM_IADCDATA, STBP_SECH_PRESENT, lengthField};

  STBP_SECH_S secHeader = {micros()};

  uint8_t packet_buff[STBP_HEADER_LENGTH_B + lengthField + 1] = {0};

  STBPacket myPacket = STBPacket(primHeader, secHeader);
  
  uint16_t simADCData[STBP_TM_IADCDATA_NUM_ELEMENTS];
  for(int i = 0; i < STBP_TM_IADCDATA_NUM_ELEMENTS; i++){
    simADCData[i] = i;
  }

  myPacket.setInternalADCData(simADCData);

  myPacket.buildPacket(packet_buff);
  Serial.write(packet_buff, sizeof(packet_buff));
}

void sendLSData(){
  // Calculate length
  size_t lengthField = STBP_SECHEADER_LENGTH_B + STBP_TM_LSDATA_LENGTH_B + STBP_CRC_LENGTH_B - 1;

  // Create Header
  STBP_HEADER_S primHeader = {STBP_TYPE_TELEMETRY, STBP_APID_TM_LSDATA, STBP_SECH_PRESENT, lengthField};

  STBP_SECH_S secHeader = {micros()};

  uint8_t packet_buff[STBP_HEADER_LENGTH_B + lengthField + 1] = {0};

  STBPacket myPacket = STBPacket(primHeader, secHeader);

  uint16_t exampleData = 0x8080;      // Take into account the is being sent the full byte, but is decoding
                                      // the most significant bits.
// 0x8080 is LS0 and LS8

  myPacket.setLSData(exampleData);

  myPacket.buildPacket(packet_buff);
  Serial.write(packet_buff, sizeof(packet_buff));
}