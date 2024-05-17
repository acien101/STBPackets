#include "STBPacket.h"

STBPacket::STBPacket(){
  this->primHeader = {0};
  this->secHeader = {0};

  // Set user data to 0
  memcpy(userDataBuff, 0, sizeof(userDataBuff));
}

/**
 * Constructor used to create a STBPacket. The easier way is to send the headers structures
*/
STBPacket::STBPacket(STBP_HEADER_S primHeader, STBP_SECH_S secHeader){
  this->primHeader = primHeader;
  this->secHeader = secHeader;

  // Set user data to 0
  memcpy(userDataBuff, 0, sizeof(userDataBuff));
}

/**
 * Setter for Primary header
*/
uint8_t STBPacket::setPrimHeader(STBP_HEADER_S primHeader){
  this->primHeader = primHeader;

  return 0;
}

uint8_t STBPacket::setSecHeader(STBP_SECH_S secHeader){
  this->secHeader = secHeader;

  return 0;
}

uint8_t STBPacket::setUserData(uint8_t* data, size_t size){
  if(size > sizeof(userDataBuff)){
    return -1;      // The size is too high
  }
  memcpy(this->userDataBuff, data, size);

  return 0;
}

uint8_t STBPacket::getUserDataLength(){
  uint8_t res = 0;

  res = primHeader.length - STBP_CRC_LENGTH_B + 1;

  if(primHeader.sech == STBP_SECH_PRESENT) res -= STBP_SECHEADER_LENGTH_B;

  return res;
}

/**
 * It builds the packet on the dst pointer. It returns 0 if successful.
*/
uint8_t STBPacket::buildPacket(uint8_t* dst){
  uint8_t* dst_p = dst;   // Moving pointer

  // Build primary header
  uint16_t header_c = (primHeader.type << STBP_TYPE_OFFSET) |
                    (primHeader.apid << STBP_APID_OFFSET) |
                    (primHeader.sech << STBP_SECH_OFFSET) |
                     primHeader.length;

  memcpy(dst_p, &header_c, STBP_HEADER_LENGTH_B);
  dst_p += STBP_HEADER_LENGTH_B;

  if(primHeader.sech){
    memcpy(dst_p, &secHeader, STBP_SECHEADER_LENGTH_B);
    dst_p += STBP_SECHEADER_LENGTH_B;
  }

  uint8_t userDataLength = getUserDataLength();
  memcpy(dst_p, &userDataBuff, userDataLength);
  dst_p += userDataLength;

  // Add the checksum
  uint16_t crc = gen_checksum(dst, dst_p - dst);
  memcpy(dst_p, &crc, STBP_CRC_LENGTH_B);

  return 0;
}

void STBPacket::setRTDData(uint32_t rtd0ch0, uint32_t rtd0ch1, uint32_t rtd0ch2, 
                    uint32_t rtd1ch0, uint32_t rtd1ch1, uint32_t rtd1ch2){
  // Structure of RTD Data User Data Field
  //   RTDSensor0Channel0 - 24 Bits
  //   RTDSensor0Channel1 - 24 Bits
  //   RTDSensor0Channel2 - 24 Bits
  //   RTDSensor1Channel0 - 24 Bits
  //   RTDSensor1Channel1 - 24 Bits
  //   RTDSensor1Channel2 - 24 Bits

  uint24_t* mv_p = (uint24_t*) userDataBuff;
  memset(mv_p, 0, STBP_TM_RTDDATA_LENGTH_B);

  memcpy(mv_p++, &rtd0ch0, sizeof(uint24_t));
  memcpy(mv_p++, &rtd0ch1, sizeof(uint24_t));
  memcpy(mv_p++, &rtd0ch2, sizeof(uint24_t));
  memcpy(mv_p++, &rtd1ch0, sizeof(uint24_t));
  memcpy(mv_p++, &rtd1ch1, sizeof(uint24_t));
  memcpy(mv_p++, &rtd1ch2, sizeof(uint24_t));
}

/**
 * Set the data of the internal ADC Data. param adcData is an array of 10 variables.
*/
void STBPacket::setInternalADCData(uint16_t* adcData){
  uint16_t* mv_p = (uint16_t*) userDataBuff;
  memset(mv_p, 0, STBP_TM_IADCDATA_LENGTH_B);

  for(uint8_t i = 0; i < STBP_TM_IADCDATA_NUM_ELEMENTS; i++){
    memcpy(mv_p++, adcData++, STBP_TM_IADCDATA_BYTES_ELEMENT);
  }
}

void STBPacket::setLSData(uint16_t data){
  uint16_t* mv_p = (uint16_t*) userDataBuff;
  memcpy(mv_p, &data, STBP_TM_LSDATA_LENGTH_B);
}


uint16_t STBPacket::gen_checksum(uint8_t const *data, int size) {     
  uint16_t sum = 0;
  size_t even_size = size - size%2;
  for (int i = 0; i < even_size; i+=2) {
    sum += data[i] + 256 * data[i+1];
  }
  if (even_size < size) {
    sum += data[size-1];
  }
  return sum;
}

/**
 * Helper function to decode header from a pointer
*/
void STBPacket::parseHeader(uint16_t* data){
  primHeader.type = ( *data >> STBP_TYPE_OFFSET ) & STBP_TYPE_MASK;
  primHeader.apid = ( *data >> STBP_APID_OFFSET ) & STBP_APID_MASK;
  primHeader.sech = ( *data >> STBP_SECH_OFFSET ) & STBP_SECH_MASK;
  primHeader.length = ( *data >> STBP_LENGTH_OFFSET ) & STBP_LENGTH_MASK;
}

void STBPacket::parseSecHeader(uint8_t* data){
  memcpy(&secHeader, data, STBP_SECHEADER_LENGTH_B);
}

void STBPacket::parseTCUserData(uint8_t* data){
  // Calculate length to read
  uint8_t length = primHeader.length - STBP_CRC_LENGTH_B + 1;
  // If secondary header was counter, we need to substract it
  if(primHeader.sech) length -= STBP_SECH_SIZE;

  memcpy(userDataBuff, data, length);

  /*
  // ANOTHER WAY TO DO IT LESS EFFICIENT
  switch (primHeader.apid){
  case STBP_APID_TC_LSSTATUS:
    memcpy(userDataBuff, data, STBP_TC_LSSTATUS_LENGTH_B);
    break;
  case STBP_APID_TC_BUILDSEQ:
    // TELECOMMAND BUILD SEQUENCE is a sequence of device status
    // Read from the length parameter and copy to user field
    break;
  case STBP_APID_TC_STARTSEQ:
    memcpy(userDataBuff, data, STBP_TC_STARTSEQ_LENGTH_B);
    break;
  case STBP_APID_TC_STOPSEQ:
    memcpy(userDataBuff, data, STBP_TC_STOPSEQ_LENGTH_B);
    break;
  default:
    break;
  }
  */

  
}

STBP_HEADER_S STBPacket::getPrimHeader(){
  return primHeader;
}

STBP_SECH_S STBPacket::getSecHeader(){
  return secHeader;
}

/**
 * Check if the CRC from the packet match the received CRC.
 * Return 1 if correct, return 0 if incorrect 
*/
uint8_t STBPacket::checkCRC(uint16_t* data){
  // Create a packet with the data and check if it match
  uint8_t packet_buff[STBP_HEADER_LENGTH_B + primHeader.length + 1] = {0}; // Without CRC
  buildPacket(packet_buff);

  uint16_t calc_crc = gen_checksum(packet_buff, sizeof(packet_buff) - STBP_CRC_LENGTH_B);

  uint16_t received_crc = *(data);

  // TODO: THE CRC IS NOT CORRECT BECAUSE THE USER DATA IS NOT CORRECTLY DECODED

  return calc_crc == received_crc;
}