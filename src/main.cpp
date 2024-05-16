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
  //myPacket.setUserData(RTDsData, sizeof(RTDsData));
  myPacket.setRTDData(1, 2, 3, 4, 5, 6);

  myPacket.buildPacket(packet_buff);
  Serial.write(packet_buff, sizeof(packet_buff));
  //Serial.println("End");
/*
  STBP_HEADER_S other = {STBP_TYPE_TELEMETRY, STBP_APID_TM_RTDDATA, STBP_SECH_PRESENT, 0b11};
  uint16_t header = (other.type << STBP_TYPE_OFFSET) |
                    (other.apid << STBP_APID_OFFSET) |
                    (other.sech << STBP_SECH_OFFSET) |
                    other.length;

  uint8_t buf[2] = {0};
  memcpy(buf, &header, 2);
  Serial.write((uint8_t*) &header, 2);
  */
}

void loop() {
  // put your main code here, to run repeatedly:
}
