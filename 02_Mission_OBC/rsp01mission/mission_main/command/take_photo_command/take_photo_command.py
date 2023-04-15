# -*- coding: utf-8 -*-
import json
from collections import OrderedDict

from mission_camera.camera.camera import Camera
from mission_main.command.command_base import CommandBase
from .take_photo_command_validator import TakePhotoCommandValidator


class TakePhotoCommand(CommandBase):
    command_name = "takePhoto"
    default_interval_msec = 0
    default_duration_sec = 0
    default_start = 0

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")
        result = self.RET_SUCCESS

        validator = TakePhotoCommandValidator(args)
        if validator.valid():
            return_code = self.camera.capture(**validator.to_dict())
            if return_code != Camera.SUCCESS:
                result = self.RET_ERROR
        else:
            result = 1

        # 応答メッセージ作成
        response = OrderedDict()
        response['cmd'] = self.command_name
        ret_arg = OrderedDict()
        ret_arg['result'] = result
        response['arg'] = ret_arg
        encoded_response = json.dumps(response).replace(u" ", "")
        self.logger.debug("response: " + encoded_response)

        return encoded_response
