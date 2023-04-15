# -*- coding: utf-8 -*-
import json
from mission_main.command.command_base import CommandBase
from mission_chat.src.StatusCollector import StatusCollector


class GetChatStatCommand(CommandBase):
    command_name = "getChatStat"

    def __init__(self):
        super().__init__(self.command_name)
        self.handler = StatusCollector()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        # コマンド実行
        # targetに埋め込まれているJSON型文字列を取り出す
        result = self.handler.getStatus(args)

        # 応答メッセージ作成
        encoded_response = self._build_response(self.command_name, json.loads(result))
        self.logger.debug("response: " + encoded_response)
        return encoded_response
