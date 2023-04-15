# -*- coding: utf-8 -*-
import json
from collections import OrderedDict

from mission_camera.camera.camera import Camera
from mission_main.command.command_base import CommandBase


class AbortTakePicCommand(CommandBase):
    command_name = "abortTakePic"

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        result = self.camera.cancel_capture_timer()

        # 応答メッセージ作成
        response = OrderedDict()
        response['cmd'] = self.command_name
        ret_arg = OrderedDict()
        ret_arg['result'] = result
        response['arg'] = ret_arg
        encoded_response = json.dumps(response).replace(u" ", "")
        self.logger.debug("response: " + encoded_response)

        return encoded_response
