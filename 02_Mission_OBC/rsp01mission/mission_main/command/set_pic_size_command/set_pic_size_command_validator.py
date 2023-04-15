# -*- coding: utf-8 -*-
from mission_camera.camera.camera_config import CameraConfigProperty
from mission_main.command.command_validator_base import CommandValidatorBase


class SetPicSizeCommandValidator(CommandValidatorBase):
    '''
    SetPicSizeCommandのリクエストパラメタを検証するクラス
    '''

    def __init__(self, args):
        super().__init__()
        self.__width = args.get(CameraConfigProperty.WIDTH)
        self.__height = args.get(CameraConfigProperty.HEIGHT)

    def valid(self):
        if self.__width is not None and not self.__valid_width():
            return False

        if self.__height is not None and not self.__valid_height():
            return False

        return True

    def to_dict(self):
        result = {}
        if self.__width is not None:
            result['width'] = self.__width

        if self.__height is not None:
            result['height'] = self.__height

        return result

    def __valid_width(self):
        name = 'width'
        value = self.__width
        return (
            self._is_integer(name, value)
            and self._is_include_in(40, 2592, name, value)
        )

    def __valid_height(self):
        name = 'height'
        value = self.__height
        return (
            self._is_integer(name, value)
            and self._is_include_in(40, 1944, name, value)
        )
