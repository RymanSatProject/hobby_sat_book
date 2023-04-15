# -*- coding: utf-8 -*-

from logging import getLogger, StreamHandler, FileHandler, DEBUG


def get_module_logger(modname):
    logger = getLogger(modname)
    logger.setLevel(DEBUG)
    logger.propagate = True
    return logger
