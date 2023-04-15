# -*- coding: utf-8 -*-
import base64
import hashlib
import os

from mission_camera.camera.camera import Camera
from mission_main.command.command_base import CommandBase
from mission_main.communication.packet_with_ecc import PacketWithECC
from .send_file_command_validator import SendFileCommandValidator


class SendFileCommand(CommandBase):
    command_name = "sendFile"
    _SAVE_DIR = "/home/pi/receive/"

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

        if not os.path.exists(self._SAVE_DIR):
            self.logger.info("create directory: " + self._SAVE_DIR)
            os.mkdir(self._SAVE_DIR)

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")
        validator = SendFileCommandValidator(args)
        if validator.valid():
            res_args = self.write_file(**validator.to_dict())
        else:
            res_args = {'result': 1, 'size': 0, 'hash': ''}

        return self._build_response(self.command_name, res_args)

    def write_file(self, filename, pos, data):
        filepath = self._SAVE_DIR + filename
        file_hash = hashlib.sha256()
        mode = self.file_open_mode(filepath)

        try:
            with open(filepath, mode) as file:
                file.seek(pos)
                b64_decoded_data = base64.b64decode(data)

                # バッファ単位からパケット単位にデータを分割
                for packet_with_ecc in PacketWithECC.split_packet(b64_decoded_data):
                    decoded_packet = PacketWithECC()
                    write_data = decoded_packet.decode_packet(packet_with_ecc)

                    decoded_packet.write_data(file, write_data)
                    file_hash.update(write_data)
                    self.logger.debug("write: " + str(len(write_data)))

            return {'result': 0, 'size': len(data), 'hash': file_hash.hexdigest()}

        except FileNotFoundError as ex:
            self.logger.error(ex)
            return {'result': 1, 'size': 0, 'hash': ''}

    def file_open_mode(self, filepath):
        if os.path.exists(filepath):
            # すでにファイルが存在する場合は上書き（編集）モードで開く
            self.logger.info('Already the file exists, update: ' + filepath)
            return 'r+b'
        else:
            # ファイルクリアまたはファイル作成してから書き込みモード
            return 'w+b'
