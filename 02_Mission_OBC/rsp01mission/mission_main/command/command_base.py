# -*- coding: utf-8 -*-
import json
from collections import OrderedDict
from mission_main.command import get_module_logger


class CommandBase:
    """
    コマンドのベースクラス。
    C&DHからのコマンドに対応する処理はこのクラスを継承して作成してください。
    """
    RET_SUCCESS = 0
    RET_ERROR = 1

    def __init__(self, command_name):
        """
        :param command_name: C&DHからのコマンド名と同じ文字列を指定して初期化してください。
        """
        self.logger = get_module_logger(__name__)
        self.command_name = command_name
        self.logger.debug("Initialize command: " + self.command_name)

    def onExecute(self, args):
        """
        コマンドの処理を記述する。
        :param args: JSON形式の引数
        :return C&DHへ返すJSON形式のメッセージ
        """
        self.logger.info("CommandBase original onExecute is called")

    def _build_response(self, command_name, args={}):
        """
        コマンドの処理のレスポンスを作成する
        :param  command_name: コマンド名, args: レスポンスとして返すargs。dict型とする。
        :return C&DHへ返すJSON形式のメッセージ
        """
        response = OrderedDict()
        response['cmd'] = command_name
        response['arg'] = OrderedDict(args)
        return json.dumps(response).replace(u" ", "")
