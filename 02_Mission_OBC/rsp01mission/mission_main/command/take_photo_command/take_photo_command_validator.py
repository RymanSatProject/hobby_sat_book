# -*- coding: utf-8 -*-
from mission_main.command.command_validator_base import CommandValidatorBase


class TakePhotoCommandValidator(CommandValidatorBase):
    '''
    TakePhotoCommandのリクエストパラメタを検証するクラス
    '''

    def __init__(self, args):
        super().__init__()
        self.__interval = args.get('interval')
        self.__duration = args.get('duration')
        self.__start = args.get('start')

    def valid(self):
        return (
            self.__valid_interval()
            and self.__valid_duration()
            and self.__valid_start()
        )

    def to_dict(self):
        return {
            'interval_millis': self.__interval,
            'duration_millis': self.__duration,
            'start_timer': self.__start,
        }

    def __valid_interval(self):
        name = 'interval'
        value = self.__interval
        return (
            self._is_presence(name, value)
            and self._is_integer(name, value)
            # MEMO: 0 ~ 60000ms(1min)
            and self._is_include_in(0, 60000, name, value)
        )

    def __valid_duration(self):
        name = 'duration'
        value = self.__duration
        return (
            self._is_presence(name, value)
            and self._is_integer(name, value)
            # MEMO: 0 ~ 300000ms(5min)
            and self._is_include_in(0, 300000, name, value)
        )

    def __valid_start(self):
        name = 'start'
        value = self.__start
        return (
            self._is_presence(name, value)
            and self._is_integer(name, value)
            # MEMO: 0 ~ 60000ms(1min)
            and self._is_include_in(0, 60000, name, value)
        )
