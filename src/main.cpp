#include <Arduino.h>

#include "STBPacket.h"

void sendRTDData();
void sendInternalADCData();
void sendLSData();

void setup() {
  Serial.begin(9600);
  
  //sendRTDData();
  //sendInternalADCData();
  //sendLSData();
}

uint8_t buff[256] = {0};

void loop() {
  if(Serial.available() >= STBP_HEADER_LENGTH_B){
    // Create a STBP packet
    STBPacket packet = STBPacket();
    Serial.println("Received a byte");
    uint32_t available = Serial.available();
    Serial.println(String(available));

    // If there is data read the header
    uint8_t val = 0;

    if(val = Serial.readBytes(buff, STBP_HEADER_LENGTH_B) != STBP_HEADER_LENGTH_B){
      Serial.print("Could not read the packet. Flushing and repeating. Could read ");
      Serial.print(String(val));
      Serial.flush();
      return;
    }

    //available = Serial.available();
    //Serial.println(String(available));
    //Serial.println(String(val));

    // TODO: Check if the readed values make sense


    packet.parseHeader((uint16_t*) buff);

    // If there is secondary header read it
    if(packet.getPrimHeader().sech){
      Serial.readBytes(buff, STBP_SECHEADER_LENGTH_B);
      packet.parseSecHeader((uint8_t*) buff);
    }

    // Read the user data field
    // Calculate length to read
    uint8_t userDataLength = packet.getUserDataLength();

    // Wait til I got all the data
    while(Serial.available() < userDataLength){

    }
    
    if(val = Serial.readBytes(buff, userDataLength) != userDataLength){
      Serial.print("Could not read the packet. Flushing and repeating. Could read ");
      Serial.print(String(val));
      Serial.flush();
      return;
    }

    //Serial.println(String(val));
    packet.parseTCUserData(buff);

    // Wait until I have received the rest
    while(Serial.available() < STBP_CRC_LENGTH_B){

    }

    // Read the checksum (CRC) and confirm that is correct
    Serial.readBytes(buff, STBP_CRC_LENGTH_B);
    if(packet.checkCRC((uint16_t*) buff) == 0){
      Serial.println("Incorrect checksum, flush data");
      Serial.flush();
    }

    int i = 0 ;

    Serial.println("Received ");
  }
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