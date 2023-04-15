# -*- coding: utf-8 -*-
import json
from collections import OrderedDict

from mission_main.command.command_base import CommandBase
from mission_chat.src.ChatPredictProcBooter import BootPredictProcessor

class RunChatMsgCommand(CommandBase):
    command_name = "runChatMsgGen"

    def __init__(self):
        super().__init__(self.command_name)
        self.handler = BootPredictProcessor()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        #コマンド実行
        #targetに埋め込まれているJSON型文字列を取り出す
        result = self.handler.bootPredictProc()

        #応答メッセージ作成
        response = OrderedDict()
        response['cmd'] = self.command_name
        response['arg'] = json.loads(result)
        encoded_response = json.dumps(response).replace(u" ", "")
        self.logger.debug("response: " + encoded_response)
        return encoded_response