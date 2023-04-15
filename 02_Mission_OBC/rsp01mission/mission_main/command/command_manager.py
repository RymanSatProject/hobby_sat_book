# -*- coding: utf-8 -*-
import threading

from mission_main.command import get_module_logger
from collections import OrderedDict

from mission_main.command.command_base import CommandBase


class CommandManager:
    """
    コマンド管理クラス。
    CommandManagerを経由して各コマンドをコマンド名で呼び出す。
    """

    def __init__(self):
        self.logger = get_module_logger(__name__)
        self.command_dict = OrderedDict()

    def subscribe(self, command: CommandBase):
        """
        コマンドを追加する。
        ここで追加されたコマンドは、C&DHからの通信で同名のコマンドを受信した際に呼び出される。
        :param command: C&DHからのコマンドに従って実行されるコマンド
        :return:
        """
        if command.command_name not in self.command_dict:
            self.command_dict[command.command_name] = command
            self.logger.info("The command [ " + command.command_name + " ] is subscribed successfully")
        else:
            self.logger.warn("The command [ " + command.command_name + " ] is already subscribed")

    def unsubscribe(self, command_name):
        if command_name in self.command_dict:
            del self.command_dict[command_name]
            self.logger.info("The command [ " + command_name + " ] is unsubscribed successfully")
        else:
            self.logger.info("The command [ " + command_name + " ] is not subscribed")

    def exec(self, command_name, args):
        response = ""
        if command_name in self.command_dict:
            response = self.command_dict[command_name].onExecute(args)
        else:
            self.logger.warn("Unknown command: " + command_name)

        return response
