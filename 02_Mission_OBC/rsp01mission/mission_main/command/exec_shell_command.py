# -*- coding: utf-8 -*-
import json
import subprocess
from collections import OrderedDict
from mission_main.command.command_base import CommandBase


class ExecShellCommand(CommandBase):
    """
    任意シェル実行コマンド
    コマンドパラメータで入力された文字列をシェル実行し、実行結果を返す。
    """
    command_name = "execShell"

    def __init__(self):
        super().__init__(self.command_name)

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")
        # 実行を行うarg/cmdから文字列を取得
        cmd = args.get("cmd")
        if self.__valid_cmd(cmd):
            ret_msg = self.__execute_cmd(cmd)
        else:
            ret_msg = '{0}: is invalid cmd'.format(cmd)

        # 応答メッセージ作成
        ret_arg = OrderedDict()
        ret_arg["msg"] = ret_msg

        response = OrderedDict()
        response["cmd"] = self.command_name
        response["arg"] = ret_arg
        encoded_response = json.dumps(response)
        self.logger.debug("response: " + encoded_response)

        return encoded_response

    def __execute_cmd(self, cmd):
        try:
            sh_ret_bytes = subprocess.check_output(
                cmd,
                stderr=subprocess.STDOUT,
                shell=True,
            )
            return sh_ret_bytes.decode("UTF-8")
        except subprocess.CalledProcessError as ex:
            # エラー発生時はそのエラーメッセージを応答する
            sh_ret = ex.output.decode('utf-8')
            self.logger.error(sh_ret)
            return sh_ret

    def __valid_cmd(self, cmd):
        if cmd is None:
            return False

        if type(cmd) is not str:
            return False

        return True
