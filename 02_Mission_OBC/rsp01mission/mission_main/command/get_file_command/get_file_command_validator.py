# -*- coding: utf-8 -*-
from mission_main.command.command_validator_base import CommandValidatorBase


class GetFileCommandValidator(CommandValidatorBase):
    '''
    GetFileCommandのリクエストパラメタを検証するクラス
    '''

    def __init__(self, args):
        super().__init__()
        self.__filepath = args.get('filepath')
        self.__offset = args.get('pos')
        self.__size = args.get('size')

    def valid(self):
        return (
            self.__valid_filepath()
            and self.__valid_offset()
            and self.__valid_size()
        )

    def to_dict(self):
        return {
            'filepath': self.__filepath,
            'offset': self.__offset,
            'read_size': self.__size,
        }

    def __valid_filepath(self):
        name = 'filepath'
        value = self.__filepath
        return (
            self._is_string(name, value)
            and self._is_presence(name, value)
        )

    def __valid_offset(self):
        name = 'offset'
        value = self.__offset
        return (
            self._is_greater_than(0, name, value)
        )

    def __valid_size(self):
        name = 'size'
        value = self.__size
        return (
            self._is_greater_than(0, name, value)
        )
