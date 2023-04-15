# -*- coding: utf-8 -*-
import json
from collections import OrderedDict

from mission_main.command.command_base import CommandBase
from mission_chat.src.ChatRequestHandler import ChatRequestHandler

class SaveChatReqMsg(CommandBase):
    command_name = "saveChatReqMsg"

    def __init__(self):
        super().__init__(self.command_name)
        self.handler = ChatRequestHandler()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        #コマンド実行
        #targetに埋め込まれているJSON型文字列を取り出す
        result = self.handler.saveRequestMessage(args)

        #応答メッセージ作成
        response = OrderedDict()
        response['cmd'] = self.command_name
        response['arg'] = json.loads(result)
        encoded_response = json.dumps(response).replace(u" ", "")
        self.logger.debug("response: " + encoded_response)
        return encoded_response