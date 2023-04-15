# -*- coding: utf-8 -*-
import sys
from threading import Lock
from time import sleep

import serial

from mission_main.communication import get_module_logger
from mission_main.communication.crc8_util import Crc8Util


class SerialComm:
    """
    シリアル通信クラス。シングルトンとして存在し、各モジュールから呼び出して使用する。
    """
    _unique_instance = None
    _lock = Lock()  # クラスロック

    def __new__(cls):
        raise NotImplementedError('Cannot initialize via Constructor')

    @classmethod
    def __internal_new__(cls):
        return super().__new__(cls)

    @classmethod
    def get_instance(cls):
        if not cls._unique_instance:
            with cls._lock:
                if not cls._unique_instance:
                    cls._unique_instance = cls.__internal_new__()
                    cls.initialize(cls)
        return cls._unique_instance

    def initialize(self):
        self.crc8_util = Crc8Util()
        self.logger = get_module_logger(__name__)
        self.serial_comm = serial.Serial(
            port="/dev/serial0",
            #port="/tmp/ttyV0",  # 動作確認用
            baudrate=115200,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=2.0
        )
        self.serial_comm.reset_input_buffer()
        self.serial_comm.reset_output_buffer()
        self.max_retry_count = 3
        self.min_msg_len = 6

    def send(self, data):
        """
        送信データにCRC8を付加して送信する。
        :param data: 送信データ
        :return:
        """
        self.logger.debug("send() is called")

        if not data or len(data) < self.min_msg_len:
            self.logger.warn("The message is too short or none")
            return

        crc8 = self.crc8_util.calc_crc8(data)
        send_msg = data + "," + format(crc8, '02x') + "\n"
        self.logger.debug("Send message: " + send_msg)

        sleep(0.1)  # wait aruduino listening
        self.serial_comm.write(send_msg.encode())

        # 相手からの受信応答の確認
        for i in range(self.max_retry_count):
            recv_msg = self.serial_comm.readline().decode().strip()
            if recv_msg == "OK":
                self.logger.debug("Receive OK response")
                break
            elif recv_msg == "NG":
                self.serial_comm.write(send_msg.encode())
            elif recv_msg == "":
                self.logger.warn("Response timeout, times " + str(i))
                self.serial_comm.write(send_msg.encode())
            else:
                self.logger.warn("Unknown response: " + recv_msg)

    def send_ok(self):
        """
        受信成功の応答を送信する。CRC8は付加しない。
        :return:
        """
        send_msg = "OK\n"
        self.logger.debug("Send message: " + send_msg)
        self.serial_comm.write(send_msg.encode())

    def send_ng(self):
        """
        受信失敗の応答を送信する。CRC8は付加しない。
        :return:
        """
        send_msg = "NG\n"
        self.logger.debug("Send message: " + send_msg)
        self.serial_comm.write(send_msg.encode())

    def receive(self):
        """
        メッセージを受信する。受診時にCRC8チェックも行う。
        :return: 受信に成功した場合、受信メッセージを返す。受信に失敗した場合、空文字を返す。
        """

        try:
            # LF means end of message, and delete LF
            recv_msg = self.serial_comm.readline().decode().strip()
        except UnicodeDecodeError as e:
            recv_msg = ""
            self.logger.error("CRC is invalid hexadecimal format")
            self.logger.error(sys.exc_info())
            self.logger.error(e)

        recv_data = ""

        if not recv_msg:
            return recv_data
        else:
            self.logger.debug("receive from C&DH: " + recv_msg)

        if len(recv_msg) < self.min_msg_len:
            self.logger.warn("Received message is too short: " + recv_msg)
            return recv_data

        try:
            recv_crc8 = recv_msg[-2:]  # crc
            recv_data = recv_msg[0:-3]  # json
        except IndexError as e:
            self.send_ng()
            self.logger.error(e)
            recv_data = ""
            return recv_data

        # check CRC8
        crc8 = self.crc8_util.calc_crc8(recv_data)

        try:
            if int(recv_crc8, 16) != crc8:
                self.logger.warn("CRC error! expected is " + recv_crc8 + ", but actual: " + format(crc8, '02x'))
                recv_data = ""
                self.send_ng()
            else:
                self.logger.debug("CRC OK")
                self.send_ok()
        except ValueError as e:
            self.logger.error("CRC is invalid hexadecimal format")
            self.logger.error(sys.exc_info())
            self.logger.error(e)
            recv_data = ""
            self.send_ng()

        return recv_data

    def is_open(self):
        return self.serial_comm.is_open
