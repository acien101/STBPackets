import serial
from construct import Struct, BitStruct, BitsInteger, Int32ul

# Create data structures
STBP_HEADER_S = BitStruct(
  "TYPE" / BitsInteger(1),
  "APID" / BitsInteger(6),
  "SECH" / BitsInteger(1),
  "LENGTH" / BitsInteger(8)
)

STBP_SECH = Struct(
  "TIME" / Int32ul
)

STBP_RTD = BitStruct(
  "RTDSensor0Channel0" / BitsInteger(24),
  "RTDSensor0Channel1" / BitsInteger(24),
  "RTDSensor0Channel2" / BitsInteger(24),
  "RTDSensor1Channel0" / BitsInteger(24),
  "RTDSensor1Channel1" / BitsInteger(24),
  "RTDSensor1Channel2" / BitsInteger(24),
)

STBP_DATA = Struct(
  "SECH" / STBP_SECH,
  "STBP_RTD" / STBP_RTD
)

# Define the serial port and baud rate
serial_port = '/dev/tty.usbmodem1461303'  # Change this to the appropriate port on your system
baud_rate = 9600  # Change this to match the baud rate of your device

# Create a serial object
ser = serial.Serial(serial_port, baud_rate)



try:
  while True:
    # Read a line from the serial port
    ser_in_header = ser.read(STBP_HEADER_S.sizeof())
    prim_header = STBP_HEADER_S.parse(ser_in_header)
    ser_in_data = ser.read(prim_header.LENGTH + 1)
    data_packet = STBP_DATA.parse(ser_in_data)

    # Print the received line
    print(prim_header)
    print(data_packet)

except KeyboardInterrupt:
  # Close the serial port when Ctrl+C is pressed
  ser.close()
  print("Serial port closed.")