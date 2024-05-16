#include <Arduino.h>

#include "STBPacket.h"

uint8_t RTDsData[18] = {0};

void setup() {
  Serial.begin(9600);
  //Serial.println("Skimmer TestBench Packets example\r\n  Author: Fran Acien, North Propulsion");
  
  RTDsData[0] = 0xFF;
  RTDsData[17] = 0xFF;

  // Create Header of the Packet
  // Calculate length
  size_t lengthField = STBP_SECHEADER_LENGTH_B + STBP_TM_RTDDATA_LENGTH_B - 1;

  // Create Header
  STBP_HEADER_S primHeader = {STBP_TYPE_TELEMETRY, STBP_APID_TM_RTDDATA, STBP_SECH_PRESENT, lengthField};

  STBP_SECH_S secHeader = {micros()};

  uint8_t packet_buff[STBP_HEADER_LENGTH_B + STBP_SECHEADER_LENGTH_B + STBP_TM_RTDDATA_LENGTH_B + STBP_CRC_LENGTH_B] = {0};

  STBPacket myPacket = STBPacket(primHeader, secHeader);
  myPacket.setRTDData(1, 2, 3, 4, 5, 6);

  myPacket.buildPacket(packet_buff);
  Serial.write(packet_buff, sizeof(packet_buff));
}

void loop() {
  // put your main code here, to run repeatedly:
}
