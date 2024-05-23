import csv
from structures import *
import os
from datetime import datetime
import serial

DATA_PATH = "/tmp/data/"
SERIAL_PORT = "/dev/tty.usbmodem1461403"
BAUD_RATE = 115200

def init_tm_csv(header_csv, filename='output.csv'):
  f = open(filename, mode='w', newline='')
  writer = csv.writer(f)
  # Write header
  header = ['TIME', 'APID']
  # Add parameters of the packet
  for headerParam in header_csv:
    header.append(headerParam)
  writer.writerow(header)
  f.close()

def tm_to_csv(prim_header, secheader, userdata, filename):
  f = open(filename, mode='a', newline='')
  writer = csv.writer(f)

  # Write userDataField
  row = [ secheader.TIME, prim_header.APID ]
  values = list(userdata.values())
  for dataItem in values[1:]:   # Skip first element, is not valid
    row.append(dataItem)
  writer.writerow(row)
  f.close()

def gen_checksum(data):
  sum = 0
  size = len(data)
  even_size = size - size % 2
  for i in range(0, even_size, 2):
    sum += data[i] + 256 * data[i+1]
  if even_size < size:
    sum += data[size-1]
  return sum

# Create data structure
# Get current date and time
now = datetime.now()
#timestamp = now.strftime("%Y-%m-%d-%H-%M")
timestamp = now.strftime("2024-05-23")


# Create folder with timestamp as name
folder_name = DATA_PATH + timestamp
# Check if the folder already exists
if not os.path.exists(folder_name):
  os.makedirs(folder_name)
  print(f"Folder '{folder_name}' created.")
else:
  print(f"Folder '{folder_name}' already exists.")

print("Saving the data in {}".format(folder_name))

# Create containers for the headers of the CSV

# Create files with the data
f_tm_adc = folder_name + '/adc.csv'
f_tm_rtd = folder_name + '/rtd.csv'
f_tm_ls = folder_name + '/ls.csv'

f_list = [f_tm_ls, "", f_tm_rtd, f_tm_adc]

init_tm_csv(['TIME', 'APID', 'ADC0', 'ADC1', 'ADC2', 'ADC3', 'ADC4', 'ADC5', 'ADC6', 'ADC7', 'ADC8', 'ADC9'], f_tm_adc)
init_tm_csv(['TIME', 'APID', 'RTD-S0CH0', 'RTD-S0CH1', 'RTD-S0CH2', 'RTD-S1CH0', 'RTD-S1CH1', 'RTD-S1CH2'], f_tm_rtd)
init_tm_csv(['TIME', 'APID', 'LS0', 'LS1', 'LS2', 'LS3', 'LS4', 'LS5', 'LS6', 'LS7', 'LS8', 'LS9', 'LS10', 'LS11', 'LS12'], f_tm_ls)
# Missing the mux boards



# Start the reception
# Create a serial object
ser = serial.Serial(SERIAL_PORT, BAUD_RATE)

try:
  while True:
    # Read a line from the serial port
    ser_in_header = ser.read(STBP_HEADER_S.sizeof())
    prim_header = STBP_HEADER_S.parse(ser_in_header)

    if prim_header.SECH == 0b1:
      ser_in_secheader = ser.read(STBP_SECH.sizeof())
      secheader = STBP_SECH.parse(ser_in_secheader)

    if prim_header.APID == 2:
      ser_in_userdata = ser.read(STBP_TM_RTD.sizeof())
      userdata = STBP_TM_RTD.parse(ser_in_userdata)
    elif prim_header.APID == 3:
      ser_in_userdata = ser.read(STBP_TM_ADC.sizeof())
      userdata = STBP_TM_ADC.parse(ser_in_userdata)
    else:
      ser_in_userdata = ser.read(STBP_TM_LSDATA.sizeof())
      userdata = STBP_TM_LSDATA.parse(ser_in_userdata)
  
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
      continue
    
    tm_to_csv(prim_header, secheader, userdata, f_list[prim_header.APID])

    # Print the received line
    print("Received packet {}".format(prim_header.APID))

except KeyboardInterrupt:
  # Close the serial port when Ctrl+C is pressed
  ser.close()
  print("Serial port closed.")