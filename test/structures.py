from construct import *

# Create data structures
STBP_HEADER_S = ByteSwapped(BitStruct(
  "TYPE" / BitsInteger(1),
  "APID" / BitsInteger(6),
  "SECH" / BitsInteger(1),
  "LENGTH" / BitsInteger(8)
))

STBP_SECH = Struct(
  "TIME" / Int32ul
)

STBP_CRC = Struct(
  "CRC" / Int16ul
)

STBP_TM_RTD = Struct(
  "RTDSensor0Channel0" / Int24ul,
  "RTDSensor0Channel1" / Int24ul,
  "RTDSensor0Channel2" / Int24ul,
  "RTDSensor1Channel0" / Int24ul,
  "RTDSensor1Channel1" / Int24ul,
  "RTDSensor1Channel2" / Int24ul,
)

STBP_TM_ADC = Struct(
  "ADC0" / Int16ul,
  "ADC1" / Int16ul,
  "ADC2" / Int16ul,
  "ADC3" / Int16ul,
  "ADC4" / Int16ul,
  "ADC5" / Int16ul,
  "ADC6" / Int16ul,
  "ADC7" / Int16ul,
  "ADC8" / Int16ul,
  "ADC9" / Int16ul
)

# Create data structures
STBP_TM_LSDATA = ByteSwapped(BitStruct(
  "LS0" / BitsInteger(1),
  "LS1" / BitsInteger(1),
  "LS2" / BitsInteger(1),
  "LS3" / BitsInteger(1),
  "LS4" / BitsInteger(1),
  "LS5" / BitsInteger(1),
  "LS6" / BitsInteger(1),
  "LS7" / BitsInteger(1),
  "LS8" / BitsInteger(1),
  "LS9" / BitsInteger(1),
  "LS10" / BitsInteger(1),
  "LS11" / BitsInteger(1),
  "LS12" / BitsInteger(1),
  "PAD0" / Padding(1),
  "PAD1" / Padding(1),
  "PAD2" / Padding(1),
))

STBP_CRC = Struct(
  "CRC" / Int16ul
)

STBP_TC_LSSTATUS = Struct(
  "DEVICE_ID" / Int8ul,
  "VALUE" / Int24ul
)

STBP_TC_SEQ_ELEMENT = Struct(
  "DEVICE_ID" / Int8ul,
  "VALUE" / Int24ul
)