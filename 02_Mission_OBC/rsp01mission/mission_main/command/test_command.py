# -*- coding: utf-8 -*-

import json
from collections import OrderedDict

from mission_main.command.command_base import CommandBase


class TestCommand(CommandBase):
    """
    コマンド実装のサンプルを兼ねるテストコマンド
    """
    command_name = "Test"

    def __init__(self):
        super().__init__(self.command_name)

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")
        self.logger.debug("args: " + str(args))

        # 受信したJSONオブジェクトの処理
        if args:
            # 通常のTestCommandはargを持たないためここには入ってこない。
            test_arg = args["AAA"]
            if test_arg == "BBB":
                self.logger.debug(args)

        # 文字列で encoded_response = "{\"name\":\"Test\"}"としてもいいが、例としてdictionaryで作成する
        response = OrderedDict()
        response['cmd'] = "Test"
        encoded_response = json.dumps(response).replace(u" ", "")  # JSONエンコードし、不要な空白を削除しておく
        self.logger.debug("response: " + encoded_response)

        # CRC8は送信時に付加するためここでは不要
        return encoded_response
