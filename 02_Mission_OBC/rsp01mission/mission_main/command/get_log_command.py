# -*- coding: utf-8 -*-
import json
from collections import OrderedDict
from mission_main.command.command_base import CommandBase
from mission_chat.src.LogCollector import LogCollector


class GetLog(CommandBase):
    command_name = "getLog"

    def __init__(self):
        super().__init__(self.command_name)
        self.handler = LogCollector()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")
        # targetに埋め込まれているJSON型文字列を取り出す
        result = self.handler.getlogs(args)

        response = OrderedDict()
        response['cmd'] = self.command_name
        response['arg'] = json.loads(result)
        encoded_response = json.dumps(response)

        self.logger.debug("response: " + encoded_response)
        return encoded_response
