import serial
from construct import *
from structures import *

def gen_checksum(data):
  sum_value = 0
  for byte in data:
    sum_value += byte
  return sum_value % 65536

# Define the serial port and baud rate
serial_port = '/dev/tty.usbmodem1461403'  # Change this to the appropriate port on your system
baud_rate = 115200  # Change this to match the baud rate of your device

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

    if prim_header.APID == 0:
      ser_in_userdata = ser.read(STBP_TM_LSDATA.sizeof())
      userdata = STBP_TM_LSDATA.parse(ser_in_userdata)
    elif prim_header.APID == 2:
      ser_in_userdata = ser.read(STBP_TM_RTD.sizeof())
      userdata = STBP_TM_RTD.parse(ser_in_userdata)
    elif prim_header.APID == 3:
      ser_in_userdata = ser.read(STBP_TM_ADC.sizeof())
      userdata = STBP_TM_ADC.parse(ser_in_userdata)
    elif prim_header.APID == 4 or prim_header.APID == 5:
      ser_in_userdata = ser.read(STBP_TM_MUX_TEMPIC.sizeof())
      userdata = STBP_TM_MUX_TEMPIC.parse(ser_in_userdata)
    elif prim_header.APID == 6 or prim_header.APID == 7 or prim_header.APID == 8 or prim_header.APID == 9:
      ser_in_userdata = ser.read(STBP_TM_MUX_ADC.sizeof())
      userdata = STBP_TM_MUX_ADC.parse(ser_in_userdata)
  
    ser_in_crc = ser.read(STBP_CRC.sizeof())
    crc = STBP_CRC.parse(ser_in_crc)

    # Check the checksum
    packet_withoutCRC = 0
    if(prim_header.SECH == 0b1):
      packet_withoutCRC = ser_in_header + ser_in_secheader + ser_in_userdata
    else:
      packet_withoutCRC = ser_in_header + ser_in_userdata
    calcCRC = gen_checksum(packet_withoutCRC)   # Calculated CRC

    if crc.CRC != calcCRC:
      print("CRC DOESNT MATCH, FLUSH SERIAL")
      ser.flush()

    # Print the received line
    print(prim_header)
    print(secheader)
    print(userdata)
    print(crc)

except KeyboardInterrupt:
  # Close the serial port when Ctrl+C is pressed
  ser.close()
  print("Serial port closed.")
