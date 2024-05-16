import serial
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

STBP_RTD = Struct(
  "RTDSensor0Channel0" / Int24ul,
  "RTDSensor0Channel1" / Int24ul,
  "RTDSensor0Channel2" / Int24ul,
  "RTDSensor1Channel0" / Int24ul,
  "RTDSensor1Channel1" / Int24ul,
  "RTDSensor1Channel2" / Int24ul,
)

STBP_CRC = Struct(
  "CRC" / Int16ul
)

STBP_DATA = Struct(
  "SECH" / STBP_SECH,
  "STBP_RTD" / STBP_RTD
)

# Define the serial port and baud rate
serial_port = '/dev/tty.usbmodem1461403'  # Change this to the appropriate port on your system
baud_rate = 9600  # Change this to match the baud rate of your device

# Create a serial object
ser = serial.Serial(serial_port, baud_rate)



try:
  while True:
    # Read a line from the serial port
    ser_in_header = ser.read(STBP_HEADER_S.sizeof())
    prim_header = STBP_HEADER_S.parse(ser_in_header)

    if prim_header.SECH == 0b1:
      print("Reading secondary header")
      ser_in_secheader = ser.read(STBP_SECH.sizeof())
      secheader = STBP_SECH.parse(ser_in_secheader)

    ser_in_userdata = ser.read(STBP_RTD.sizeof())
    userdata = STBP_RTD.parse(ser_in_userdata)

    ser_in_crc = ser.read(STBP_CRC.sizeof())
    crc = STBP_CRC.parse(ser_in_crc)

    # Print the received line
    print(prim_header)
    print(secheader)
    print(userdata)
    print(crc)

except KeyboardInterrupt:
  # Close the serial port when Ctrl+C is pressed
  ser.close()
  print("Serial port closed.")