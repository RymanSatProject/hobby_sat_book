# -*- coding: utf-8 -*-
import base64

from mission_camera.camera.camera import Camera
from mission_main.command.command_base import CommandBase
from mission_main.communication.packet_with_ecc import PacketWithECC

from .get_file_command_validator import GetFileCommandValidator


class GetFileCommand(CommandBase):
    command_name = "getFile"

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")
        result = 0

        validator = GetFileCommandValidator(args)
        if validator.valid():
            try:
                read_data = self.read_file(**validator.to_dict())
            except FileNotFoundError as ex:
                self.logger.error(ex)
                result = 1
        else:
            result = 1

        if result == 0:
            response = self._build_response(self.command_name, {
                'size': len(read_data),
                'data': read_data,
                'result': result,
            })
        else:
            response = self._build_response(self.command_name, {
                'size': 0,
                'data': '',
                'result': result,
            })

        self.logger.debug("response: " + response)
        return response

    def read_file(self, filepath, offset, read_size):
        read_data = ""
        with open(filepath, "rb") as file:
            for packet in PacketWithECC.read_data(file, offset, read_size):
                packet_with_ecc = packet.get_packet_with_ecc()
                encoded_packet_with_ecc = base64.b64encode(packet_with_ecc)
                byte_str = encoded_packet_with_ecc.decode('utf-8')
                read_data += str(byte_str)

        return read_data
