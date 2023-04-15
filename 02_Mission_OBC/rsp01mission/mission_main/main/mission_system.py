# -*- coding: utf-8 -*-
import json
import sys
from collections import OrderedDict

from mission_main.command.command_manager import CommandManager
from mission_main.command.delete_pic_command import DeletePicCommand
from mission_main.command.exec_shell_command import ExecShellCommand
# チャット系コマンド
from mission_main.command.get_chat_res_msg_command import GetChatResMsgCommand
from mission_main.command.get_chat_stat_command import GetChatStatCommand
from mission_main.command.get_file_command import GetFileCommand
from mission_main.command.get_log_command import GetLog
from mission_main.command.get_pic_data_command import GetPicDataCommand
from mission_main.command.get_pic_info_command import GetPicInfoCommand
from mission_main.command.get_pic_list_command import GetPicListCommand
from mission_main.command.get_temperature_command import GetTemperatureCommand
from mission_main.command.kill_mobc_command import KillMOBCCommand
# 画像認識系コマンド
from mission_main.command.pic_categ_commands import (AbortPicCategCommand, CheckPicCategCommand, GetGoodPicIdsCommand,
                                                     GetPicCategCommand, StartPicCategCommand)
from mission_main.command.run_chat_msg_gen import RunChatMsgCommand
from mission_main.command.save_chat_req_msg_command import SaveChatReqMsg
from mission_main.command.send_file_command import SendFileCommand
from mission_main.command.set_camera_config_command import SetCameraConfigCommand
from mission_main.command.set_comp_pic_command import SetCompPicCommand
from mission_main.command.set_pic_size_command import SetPicSizeCommand
from mission_main.command.take_photo_command import TakePhotoCommand
from mission_main.command.test_command import TestCommand
from mission_main.communication.serial_comm import SerialComm
from mission_main.main import get_module_logger


class MissionSystem:

    def __init__(self):
        self.logger = get_module_logger()
        self.serial = SerialComm.get_instance()
        self.command_manager = CommandManager()

        self.command_list = [
            TestCommand(),
            TakePhotoCommand(),
            GetPicInfoCommand(),
            DeletePicCommand(),
            GetPicDataCommand(),
            KillMOBCCommand(),
            GetTemperatureCommand(),
            GetPicListCommand(),
            SetPicSizeCommand(),
            SetCompPicCommand(),
            SetCameraConfigCommand(),
            GetChatResMsgCommand(),
            GetChatStatCommand(),
            SaveChatReqMsg(),
            GetLog(),
            RunChatMsgCommand(),
            SetCameraConfigCommand(),
            AbortPicCategCommand(),
            CheckPicCategCommand(),
            GetPicCategCommand(),
            StartPicCategCommand(),
            GetGoodPicIdsCommand(),
            ExecShellCommand(),
            SendFileCommand(),
            GetFileCommand()
        ]

        self.load_commands()

    def run(self):
        self.logger.debug("Mission system is running")

        while self.serial.is_open:
            # receive data from C&DH
            receive_command = self.serial.receive()
            if receive_command:
                try:
                    response = self.exec_command(receive_command)
                    self.serial.send(response)
                except Exception as e:
                    # すべての例外を補足して処理の実行を優先する。
                    self.logger.error(
                        "Unhandled exception is detected! The exception must be caught in calling method!")
                    self.logger.exception(e)

        self.logger.warn("Serial is closed")

    def exec_command(self, receive_command):
        """
        C&DHから受信したコマンドに応じて処理を実行させる。
        :param receive_command: 受信したコマンドのJSON
        :return: C＆DHに返信するJSON形式のメッセージ
        """
        self.logger.debug("receive: " + receive_command)

        # parse JSON command
        try:
            receive_command = json.loads(receive_command)
            command_name = receive_command.get("cmd", "")
            args = OrderedDict()
            if "arg" in receive_command:
                args = receive_command["arg"]

            # exec command
            self.logger.debug("command: " + command_name)
            response = self.command_manager.exec(command_name, args)

        except json.JSONDecodeError as e:
            self.logger.error(sys.exc_info())
            self.logger.error(e)
            response = "{\"name\":\"Fail\"}"  # TODO エラー時に返す値を確認する

        return response

    def load_commands(self):
        self.logger.debug("Load commands")
        for command in self.command_list:
            self.command_manager.subscribe(command)
