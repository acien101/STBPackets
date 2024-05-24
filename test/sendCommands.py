import serial
from construct import *
from structures import *

def gen_checksum(data):
  sum_value = 0
  for byte in data:
    sum_value += byte
  return sum_value % 65536


def gen_LS_CMD():
  header = STBP_HEADER_S.build(dict(
    TYPE = 1,   # CMD
    APID = 0,   # Change status Load Switch
    SECH = 0,
    LENGTH = STBP_TC_LSSTATUS.sizeof() + 2 - 1    # Count the CRC
  ))
  userDataField = STBP_TC_LSSTATUS.build(dict(
    DEVICE_ID = 3,
    VALUE = 19
  ))
  # Concatenate header and user data field
  packet_no_crc = header + userDataField
  crc = gen_checksum(packet_no_crc)
  packet = packet_no_crc + Int16ul.build(crc)
  return packet

def gen_BUILDSEQ_CMD():
  # Define CMDs that are going to be sent
  cmds = []
  cmds.append(Container(DEVICE_ID=1, VALUE=2)) # Cmd0
  cmds.append(Container(DEVICE_ID=2, VALUE=3)) # Cmd1
  cmds.append(Container(DEVICE_ID=3, VALUE=4)) # Cmd2
  cmds.append(Container(DEVICE_ID=4, VALUE=5)) # Cmd3
  cmds.append(Container(DEVICE_ID=5, VALUE=6)) # Cmd4
  header = STBP_HEADER_S.build(dict(
    TYPE = 1,   # CMD
    APID = 1,   # Build sequence
    SECH = 0,
    LENGTH = (STBP_TC_SEQ_ELEMENT.sizeof()*len(cmds)) + 2 - 1
  ))
  STBP_TC_BUILDSEQ = Array(len(cmds), STBP_TC_SEQ_ELEMENT)
  userDataField = STBP_TC_BUILDSEQ.build(cmds)
  # Concatenate header and user data field
  packet_no_crc = header + userDataField
  crc = gen_checksum(packet_no_crc)
  packet = packet_no_crc + Int16ul.build(crc)
  return packet

def gen_STARTSEQ_CMD():
  header = STBP_HEADER_S.build(dict(
    TYPE = 1,   # CMD
    APID = 2,   # Start sequence
    SECH = 0,
    LENGTH = 1 + 2 - 1    # 1 Byte of Padding + CRC
  ))
  userDataField = Int8ul.build(5)     # Its Padding, doesnt matter the value
  # Concatenate header and user data field
  packet_no_crc = header + userDataField
  crc = gen_checksum(packet_no_crc)
  packet = packet_no_crc + Int16ul.build(crc)
  return packet

def gen_STOPSEQ_CMD():
  header = STBP_HEADER_S.build(dict(
    TYPE = 1,   # CMD
    APID = 3,   # STOP sequence
    SECH = 0,
    LENGTH = 1 + 2 - 1    # 1 Byte of Padding + CRC
  ))
  userDataField = Int8ul.build(5)     # Its Padding, doesnt matter the value
  # Concatenate header and user data field
  packet_no_crc = header + userDataField
  crc = gen_checksum(packet_no_crc)
  packet = packet_no_crc + Int16ul.build(crc)
  return packet

# Define the serial port and baud rate
serial_port = '/dev/tty.usbmodem1461303'  # Change this to the appropriate port on your system
baud_rate = 115200  # Change this to match the baud rate of your device

# Create a serial object
ser = serial.Serial(serial_port, baud_rate)

packet = gen_BUILDSEQ_CMD()

ser.write(packet)

print(packet)

ser.close()
