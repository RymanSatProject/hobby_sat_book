#coding: utf-8
"""
衛星側チャットシステム　Logger
"""

from logging import config,getLogger
from mission_chat.src.setting import PATH_ROOT
from mission_chat.src.Constants import *
import yaml,os

configFilePath = os.path.join(PATH_ROOT,"config",logConfFilename)
config_data = yaml.load(open(configFilePath, encoding='UTF-8').read())
logFilePath = os.path.join(PATH_ROOT, "logs", "chat.log")
if not os.path.exists(logFilePath):
    os.makedirs(os.path.join(PATH_ROOT,"logs"),exist_ok=True)
    with open(logFilePath,encoding="utf-8",mode="w") as f:
        pass
config_data["handlers"]["fileRotatingHandler"]["filename"] = logFilePath
config.dictConfig(config_data)
logger = getLogger("chat")


class Logger:
    """
    config/logging.confの内容を設定したLoggerを返す
    Args:
        なし
    Returns:
        Logger
    """
    @staticmethod
    def get_logger():
        """
        Loggerを返す。

        """
        return logger
