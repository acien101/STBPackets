#include <Arduino.h>

#include "STBPacket.h"

#define DEBUG true

void sendRTDData();
void sendInternalADCData();
void sendLSData();

uint8_t buff[256] = {0};

void setup() {
  Serial.begin(9600);
  
  //sendRTDData();
  //sendInternalADCData();
  //sendLSData();
}

/**
 * Use to clean the incomming serial
*/
void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

/**
 * Function to receive serial data, it waits for the data to arrive
 * Return 0 if perfect, 1 if there was an error
*/
void serialGetBuffer(uint8_t* buff, size_t size){
  uint8_t* buff_mp = buff;    // Moving pointer
  size_t size_cnt = 0;    // Counter for the size
  do{
    // Wait to receive the first byte
    while(!Serial.available()){
      delay(10);
    }
    *(buff_mp++) = Serial.read();    // Increment the counter
    size_cnt++;
  }while(size_cnt < size);
}

// When reading commands it should be in the next way:
// 1. Wait until there are necessary packets for reading the header
// 2. Read packets of the header. If there are not necessary packets, flush and repeat.
// 3. From the header take the length of the rest of the packet. Wait to receive the information.
// 4. Receive the packets, if there are not enough, then flush and repeat.
// 5. Decode the packet
// 6. Check CRC, if is not valid, then flush all the data, and repeat.

void refresh(){
  uint8_t* m_p;   // Moving pointer

  if(Serial.available() >= STBP_HEADER_LENGTH_B){
    // Create a STBP packet
    STBPacket packet = STBPacket();

    if(DEBUG){
      Serial.print("Receiving a STBPacket with ");
      Serial.print(String(Serial.available()));
      Serial.println(" bytes");
    }
    

    // If there is data read the header
    if(Serial.readBytes(buff, STBP_HEADER_LENGTH_B) != STBP_HEADER_LENGTH_B){
      if (DEBUG){
        Serial.print("Could not read the packet. Flushing and repeating.");
      }
      
      delay(10);     // Wait to receive all the packets and flush them
      serialFlush();
      return;
    }

    // Decode STBPacker Header
    packet.parseHeader((uint16_t*) buff);

    // Wait to receive the rest of the packet
    // Read the user data field
    // Calculate length to read
    uint8_t dataLength = packet.getPrimHeader().length + 1;

    serialGetBuffer(buff, dataLength);  // Read data while waiting for incoming data

    // Point to the beggining of the buffer
    // The pointer is used to read different parts of the packet
    m_p = buff;

    // If there is secondary header read it
    if(packet.getPrimHeader().sech){
      packet.parseSecHeader((uint8_t*) m_p);
      // Move pointer of the buffer
      m_p = m_p + STBP_SECHEADER_LENGTH_B;
    }

    // Parse User Data fied
    packet.parseTCUserData(m_p);
    m_p = m_p + packet.getUserDataLength();    // Move pointer of the buffer

    // Read the checksum (CRC) and confirm that is correct
    
    if(packet.checkCRC((uint16_t*) m_p) == 0){
      if (DEBUG){
        Serial.println("Incorrect checksum, flush data");
      }
      
      delay(10);     // Wait to receive all the packets and flush them
      serialFlush();
      return;
    }

    if (DEBUG){
      Serial.println("Packet received completly");
      packet.printPacket();
    }
  }
}

void loop() {
  refresh();
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