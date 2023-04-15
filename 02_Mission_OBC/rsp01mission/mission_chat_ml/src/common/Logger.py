# coding: utf-8
"""
衛星側チャットシステム　Logger
"""

from logging import config, getLogger
from . import chat_config
import yaml

config.dictConfig(
    yaml.load(
        open(chat_config.get_path("logConfigPath"), encoding='UTF-8').read()))
logger = getLogger()


class Logger:
    """
    Loggerクラス
    """

    @staticmethod
    def get_logger():
        """
        Loggerを返す。
        Args : なし
        Returns : Loggerインスタンス
        """

        return logger
