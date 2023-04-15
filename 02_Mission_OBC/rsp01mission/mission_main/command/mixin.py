import json
import subprocess
from collections import OrderedDict
from datetime import datetime, timedelta

import tailer


class ResponseMixin(object):
    @classmethod
    def _make_response(cls, command_name: str, arg: dict):
        response = OrderedDict()
        response['cmd'] = command_name
        ret_arg = OrderedDict()
        for key, value in arg.items():
            ret_arg[key] = value
        response['arg'] = ret_arg
        return json.dumps(response).replace(u' ', '')


class ShellMixin(object):
    @classmethod
    def _execute_cmd(cls, cmd: str):
        subprocess.Popen(cmd.split(' '))

    @classmethod
    def _execute_cmd_and_wait(cls, cmd: str):
        p = subprocess.Popen(cmd.split(' '), stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        stdout, stderr = p.communicate()
        return p.returncode, stdout, stderr


class LogMixin(object):
    @classmethod
    def _get_code_line(cls, log_file_path: str, line_num_from_bottom: int = 1):
        """
        FileNotFoundError と IndexError が発生する可能性がある
        """
        target_line = tailer.tail(open(log_file_path), line_num_from_bottom)
        return target_line[-line_num_from_bottom]

    @classmethod
    def _is_one_hour_ago_log(cls, log_line: str):
        """
        log_line には「[2019/04/02 21:16:05]...」で始まっているという前提
        format に合ってない場合、ValueError が出る
        """
        log_datetime = datetime.strptime(log_line[1: 20], '%Y/%m/%d %H:%M:%S')
        return log_datetime < datetime.now() - timedelta(hours=1)
