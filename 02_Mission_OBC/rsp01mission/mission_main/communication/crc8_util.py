# coding: utf-8
# !/usr/bin/env python


class Crc8Util:
    def __init__(self):
        poly = 0x07
        mask = 0xFF
        self.CRC8TABLE = [0 for i in range(256)]

        for i in range(256):
            crc8 = i
            for j in range(8):
                if crc8 & 0x80:
                    crc8 = (((crc8 << 1) & mask) ^ poly)
                else:
                    crc8 <<= 1
                j += 1
            self.CRC8TABLE[i] = crc8 & 0xFF
            i += 1

    def calc_crc8(self, data):
        crc8 = 0x00
        for i in range(len(data)):
            crc8 = self.CRC8TABLE[crc8 ^ int.from_bytes(data[i].encode(), 'big')]
        return crc8
