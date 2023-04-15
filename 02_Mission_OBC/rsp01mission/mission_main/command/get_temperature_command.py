# -*- coding: utf-8 -*-
from collections import OrderedDict
from mission_main.command.command_base import CommandBase


class GetTemperatureCommand(CommandBase):
    command_name = "getTemp"

    def __init__(self):
        super().__init__(self.command_name)

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        temperature = -273
        with open("/sys/class/thermal/thermal_zone0/temp", "r") as file:
            temperature = float(int(file.read()) / 1000)

        # 応答メッセージ作成
        ret_arg = OrderedDict()
        ret_arg['temp'] = temperature
        encoded_response = self._build_response(self.command_name, ret_arg)
        self.logger.debug("response: " + encoded_response)

        return encoded_response
