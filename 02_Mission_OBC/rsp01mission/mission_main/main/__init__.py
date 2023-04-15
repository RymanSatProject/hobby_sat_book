# -*- coding: utf-8 -*-

import sys
import os
from logging import getLogger, StreamHandler, FileHandler, DEBUG

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/../../../mission_camera/src')


def get_module_logger():
    file_handler = FileHandler(r'./log.txt')
    file_handler.setLevel(DEBUG)

    stream_handler = StreamHandler()
    stream_handler.setLevel(DEBUG)

    logger = getLogger()  # root logger
    logger.setLevel(DEBUG)
    logger.addHandler(stream_handler)
    logger.addHandler(file_handler)
    return logger
