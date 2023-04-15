# -*- coding: utf-8 -*-
import os
from mission_main.command.command_validator_base import CommandValidatorBase


class SendFileCommandValidator(CommandValidatorBase):
    '''
    SendFileCommandのリクエストパラメタを検証するクラス
    '''

    def __init__(self, args):
        super().__init__()
        self.__filename = args.get('filename')
        self.__pos = args.get('pos')
        self.__data = args.get('data')

    def valid(self):
        return (
            self.__valid_filename()
            and self.__valid_pos()
            and self.__valid_data()
            and self.__is_permitted_position()  # MEMO: pos, filenameが正しい形式が前提であるので最後に実行
        )

    def to_dict(self):
        return {
            'filename': self.__filename,
            'pos': self.__pos,
            'data': self.__data,
        }

    def __valid_filename(self):
        name = 'filename'
        value = self.__filename
        return (
            self._is_string(name, value)
            and self._is_presence(name, value)
        )

    def __valid_pos(self):
        name = 'pos'
        value = self.__pos
        return (
            self._is_greater_than(0, name, value)
        )

    def __valid_data(self):
        name = 'data'
        value = self.__data
        return (
            self._is_string(name, value)
            and self._is_presence(name, value)
        )

    def __is_permitted_position(self):
        # MEMO: 循環参照になるため、使う直前にインポートする
        from .send_file_command import SendFileCommand
        filepath = SendFileCommand._SAVE_DIR + self.__filename

        if os.path.exists(filepath):
            if os.path.getsize(filepath) < self.__pos:
                self.logger.warning(
                    "pos can't be over the size of target file when it is updated. value:{0}".format(self.__pos)
                )
                return False
        else:
            if self.__pos > 0:
                self.logger.warning('pos must be 0 when file is created. value:{0}'.format(self.__pos))
                return False

        return True
