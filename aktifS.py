import smbus
import struct
import sys
import time
import array

bus = smbus.SMBus(1)


def ConvertStringsToBytes(src):
  converted = []
  for b in src:
    converted.append(ord(b))
  return converted

i=0

while(1):
    data = bytearray()
    BytesToSend = ConvertStringsToBytes("D")
    bus.write_i2c_block_data(0x62, 0x00, BytesToSend)
    break

