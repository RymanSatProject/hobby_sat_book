# -*- coding: utf-8 -*-
import subprocess
from collections import OrderedDict

from mission_main.command.command_base import CommandBase


class KillMOBCCommand(CommandBase):
    command_name = "killMOBC"

    def __init__(self):
        super().__init__(self.command_name)

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        return_code = subprocess.call("sudo -S /sbin/shutdown -h +1", shell=True)
        result = self.RET_SUCCESS if return_code == 0 else self.RET_ERROR

        # 応答メッセージ作成
        ret_arg = OrderedDict()
        ret_arg['result'] = result
        encoded_response = self._build_response(self.command_name, ret_arg)
        self.logger.debug("response: " + encoded_response)

        return encoded_response
