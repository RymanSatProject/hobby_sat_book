# coding: utf-8
"""
衛星側チャットシステム　実行制御
"""
from common.Logger import Logger

logger = Logger.get_logger()

# 停止フラグ
stop_flg = False


def setStop():
    global stop_flg
    stop_flg = True


def isStop():
    return stop_flg
