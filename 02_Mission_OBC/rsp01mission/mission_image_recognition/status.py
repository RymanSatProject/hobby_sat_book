# coding: utf-8

import shlex
import subprocess
from enum import Enum

import tailer

from settings import ABORTING_FILE_PATH, IMAGE_RECOGNITION_LOG_PATH, RESULTS


class Status(Enum):
    WORKING = RESULTS['checkPicCateg']['working']
    STOP = RESULTS['checkPicCateg']['stop']
    ABORTING = RESULTS['checkPicCateg']['aborting']
    ERROR = RESULTS['checkPicCateg']['error']

    @classmethod
    def create(cls):
        if cls._find_running_process():
            return cls.ABORTING if cls._find_aborting_file() else cls.WORKING
        return cls.STOP if cls._find_finish_keyword_in_log() else cls.ERROR

    @classmethod
    def _find_running_process(cls):
        cmd = 'ps aux'
        processes = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE).stdout.readlines()
        for process in processes:
            if 'startPicCateg' in process:  # todo getPicCategWithTF も入れる？
                return True
        return False

    @classmethod
    def _find_finish_keyword_in_log(cls):
        if not IMAGE_RECOGNITION_LOG_PATH.exists():
            return True
        log = tailer.tail(open(str(IMAGE_RECOGNITION_LOG_PATH)), 1)
        if len(log) == 0:
            return True
        return 'finish startPicCateg {}'.format(RESULTS['startPicCateg']['success']) in log[0]

    @classmethod
    def _find_aborting_file(cls):
        return ABORTING_FILE_PATH.exists()

    @classmethod
    def reset(cls):
        """
        abort ファイルを消す
        プロセスが動いてたら落とす？
        -> 落とさない、linux 再起動したら落ちるし・・
        """
        if cls._find_aborting_file():
            ABORTING_FILE_PATH.unlink()

    @classmethod
    def abort(cls):
        """
        abort ファイルを作る
        """
        if not cls._find_aborting_file():
            ABORTING_FILE_PATH.touch()
