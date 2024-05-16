#include "STBPacket.h"

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

  res = primHeader.length + 1;

  if(primHeader.sech == STBP_SECH_PRESENT) res -= STBP_SECHEADER_LENGTH_B;

  return res;
}

/**
 * It builds the packet on the dst pointer. It returns 0 if successful.
*/
uint8_t STBPacket::buildPacket(uint8_t* dst){
  uint8_t* dst_p = dst;   // Moving pointer
  memcpy(dst_p, (uint8_t *) &primHeader, STBP_HEADER_LENGTH_B);
  dst_p += STBP_HEADER_LENGTH_B;

  // TODO if there is a secondary header

  memcpy(dst_p, (uint8_t *) &secHeader, STBP_SECHEADER_LENGTH_B);
  dst_p += STBP_SECHEADER_LENGTH_B;
  memcpy(dst_p, (uint8_t *) &userDataBuff, getUserDataLength());
  dst_p += STBP_SECHEADER_LENGTH_B;

  return 0;
}

void STBPacket::setRTDData(uint32_t rtd0ch0, uint32_t rtd0ch1, uint32_t rtd0ch2, 
                    uint32_t rtd1ch0, uint32_t rtd1ch1, uint32_t rtd1ch2){

  //this->userDataBuff

  // TODO COPY THE DATA
}