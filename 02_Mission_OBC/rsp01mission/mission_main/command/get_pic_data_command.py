# -*- coding: utf-8 -*-
import os
import base64
import json
from collections import OrderedDict

from mission_camera.camera.camera import Camera
from mission_main.command.command_base import CommandBase
from mission_main.communication.packet_with_ecc import PacketWithECC


class GetPicDataCommand(CommandBase):
    command_name = "getPicData"
    THUMBNAIL = 0
    TYPE_ORIGINAL = 1

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        if not self.__valid_args(args):
            self.logger.error("Invalid Request: {}".format(json.dumps(args)))
            return self.__build_encoded_response(
                self.command_name,
                {
                    'result': 1,
                    'size': 0,
                    'data': '',
                }
            )

        image_type = args.get("type")
        pic_id = args.get("id")
        pic_sub_id = args.get("sub_id", -1)
        if pic_sub_id < 0 or pic_sub_id == 65535:
            pic_sub_id = ""
        offset = args.get("pos")
        read_size = args.get("size")

        filepath = self.__get_image_path(image_type, pic_id, pic_sub_id)
        if not os.path.exists(filepath):
            self.logger.error("Request image is not found. Request:{}".format(json.dumps(args)))
            return self.__build_encoded_response(
                self.command_name,
                {
                    'result': 1,
                    'size': 0,
                    'data': '',
                }
            )

        self.logger.debug("get pic data of " + str(filepath))
        send_data_base64 = self.__read_file(filepath, offset, read_size)

        return self.__build_encoded_response(
            self.command_name,
            {
                'result': 0,
                'size': len(send_data_base64),
                'data': send_data_base64,
            }
        )


    def __read_file(self, filepath, offset, read_size):
        send_data_base64 = ""
        if not read_size > 0:
            read_size = 1024  # os.path.getsize(filepath)

        with open(filepath, "rb") as file:
            for packet in PacketWithECC.read_data(file, offset, read_size):
                packet_with_ecc = packet.get_packet_with_ecc()
                encoded_packet_with_ecc = base64.b64encode(packet_with_ecc)
                byte_str = encoded_packet_with_ecc.decode('utf-8')
                send_data_base64 += str(byte_str)

        return send_data_base64


    def __build_encoded_response(self, response_cmd, response_args={}):
        response = OrderedDict()
        response['cmd'] = response_cmd
        args = OrderedDict(response_args)
        response['arg'] = args
        return json.dumps(response).replace(u" ", "")
    

    def __valid_args(self, args):
        pic_id = args.get("id", -1)
        if not type(pic_id) is int or pic_id < 0:
            return False
        
        # sub_idの指定ありの場合のみ、検証
        pic_sub_id = args.get("sub_id", None)
        if pic_sub_id and (not type(pic_sub_id) == int or pic_sub_id < 0):
            return False

        image_type = args.get("type", -1)
        if not image_type in [self.THUMBNAIL, self.TYPE_ORIGINAL]:
            return False
        
        offset = args.get("pos",  -1)
        if not type(offset) is int or offset < 0:
            return False
        
        read_size = args.get("size",  -1)
        if not type(read_size) is int or read_size < 0:
            return False

        return True
    

    def __get_image_path(self, image_type, pic_id, pic_sub_id):
        if image_type == self.TYPE_ORIGINAL:
            return self.camera.IMAGE_DIR + self.camera.seq_to_filename(pic_id, pic_sub_id)
        else:
            return self.camera.IMAGE_DIR + self.camera.seq_to_thumb_filename(pic_id, pic_sub_id)
