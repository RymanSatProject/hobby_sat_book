# -*- coding: utf-8 -*-
from mission_camera.camera.camera_config import CameraConfigProperty
from mission_main.command.command_validator_base import CommandValidatorBase


class SetCompPicCommandValidator(CommandValidatorBase):
    '''
    SetCompPicCommandのリクエストパラメタを検証するクラス
    '''

    def __init__(self, args):
        super().__init__()
        self.__thumb_width = args.get(CameraConfigProperty.THUMB_WIDTH)
        self.__thumb_height = args.get(CameraConfigProperty.THUMB_HEIGHT)

    def valid(self):
        # MEMO: NoneであるPropertyはレスポンスに含めないため検証をしない
        if self.__thumb_width is not None and not self.__valid_thumb_width():
            return False

        if self.__thumb_height is not None and not self.__valid_thumb_height():
            return False

        return True

    def to_dict(self):
        result = {}
        if self.__thumb_width is not None:
            result['thumb_width'] = self.__thumb_width

        if self.__thumb_height is not None:
            result['thumb_height'] = self.__thumb_height

        return result

    def __valid_thumb_width(self):
        name = 'thumb_width'
        value = self.__thumb_width
        return (
            self._is_integer(name, value)
            and self._is_include_in(1, 2592, name, value)
        )

    def __valid_thumb_height(self):
        name = 'thumb_height'
        value = self.__thumb_height
        return (
            self._is_integer(name, value)
            and self._is_include_in(1, 1944, name, value)
        )
