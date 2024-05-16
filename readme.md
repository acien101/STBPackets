# Skimmer TestBench Packets example

This project create a encoder/decoder for data packets used in the Skimmer TestBench.

The features are:
* Easy to understand and implement
* Short overhead
* They can be sent consecutively
* Telemetry and telecommand structure. Telecommands (TC) are sent from PC to the STB, and Telemetry (TM) is sent from STB to PC.
* There are different packets sent at different data rates. For this reason there is different packets. Each packet type is meant to be for each FreeRTOS task on the STB.

## Structure of the packets

This packets are a simpler version of CCSDS packets, taking some useful structures.

Header structure:
* TYPE - 1 Bit - Defines if is 0 Telemetry, or 1 Telecommand.
* APID - 6 Bits - Defines identifier of telemetry. Each type of telemetry has different APID.
* SECH - 1 Bit - Secondary Header, Defines if is 1 there is a secondary header, or 0 there is not a secondary header.
* LENGTH - 8 Bits - Defines the length of the packet. This length is calculated as the lenfth of the PACKET DATA field minus 1. The secondary header is considered to be part of the DATA field. The Lenght is counted in BYTES.


Secondary header:
* TIME - 32 Bits - Defines the time when this packet was created in microseconds. It is reference with the time it started the microcontroller.

The PACKET DATA field is defined as:
* Packet Secondary header
* USER DATA FIELD

The user data field can have different shapes, depends on the APID:
* 0 - Load switches data - Data collected from the Load Switches.
* 1 - Mux Board data - Data collected from the Mux Board
* 2 - RTDs data - Data collected from all the channels of the RTDs PCBs.

Structure of user data field "0" - Load switches data:
* LS0 - 1 Bit - Load Switch 0 status
* LS1 - 1 Bit
* LS2 - 1 Bit
* LS3 - 1 Bit
* LS4 - 1 Bit
* LS5 - 1 Bit
* LS6 - 1 Bit
* LS7 - 1 Bit

Structure of user data field "1" - Mux Board data:
* TBD

Structure of user data field "2" - RTDs data:
* RTDSensor0Channel0 - 24 Bits
* RTDSensor0Channel1 - 24 Bits
* RTDSensor0Channel2 - 24 Bits
* RTDSensor1Channel0 - 24 Bits
* RTDSensor1Channel1 - 24 Bits
* RTDSensor1Channel2 - 24 Bits