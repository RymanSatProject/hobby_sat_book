# -*- coding: utf-8 -*-
from mission_camera.camera.camera_config import CameraConfigProperty
from mission_main.command.command_validator_base import CommandValidatorBase


class SetCameraConfigCommandValidator(CommandValidatorBase):
    '''
    SetPicSizeCommandのSetの場合のリクエストパラメタを検証するクラス
    '''

    def __init__(self, args):
        super().__init__()
        self.__iso = args.get(CameraConfigProperty.ISO)
        self.__shutter = args.get(CameraConfigProperty.SHUTTER)
        self.__quality = args.get(CameraConfigProperty.JPEG_QUALITY)
        self.__sharpness = args.get(CameraConfigProperty.SHARPNESS)
        self.__contrast = args.get(CameraConfigProperty.CONTRAST)
        self.__brightness = args.get(CameraConfigProperty.BRIGHTNESS)
        self.__saturation = args.get(CameraConfigProperty.SATURATION)
        self.__exposure = args.get(CameraConfigProperty.EXPOSURE)

    def valid(self):
        if self.__iso is not None and not self.__valid_iso():
            return False

        if self.__shutter is not None and not self.__valid_shutter():
            return False

        if self.__quality is not None and not self.__valid_quality():
            return False

        if self.__quality is not None and not self.__valid_quality():
            return False

        if self.__sharpness is not None and not self.__valid_sharpness():
            return False

        if self.__contrast is not None and not self.__valid_contrast():
            return False

        if self.__brightness is not None and not self.__valid_brightness():
            return False

        if self.__saturation is not None and not self.__valid_saturation():
            return False

        if self.__exposure is not None and not self.__valid_exposure():
            return False

        return True

    def to_dict(self):
        result = {}
        if self.__iso is not None:
            result['iso'] = self.__iso

        if self.__shutter is not None:
            result['shutter'] = self.__shutter

        if self.__quality is not None:
            result['quality'] = self.__quality

        if self.__sharpness is not None:
            result['sharpness'] = self.__sharpness

        if self.__contrast is not None:
            result['contrast'] = self.__contrast

        if self.__brightness is not None:
            result['brightness'] = self.__brightness

        if self.__saturation is not None:
            result['saturation'] = self.__saturation

        if self.__exposure is not None:
            result['exposure'] = self.__exposure

        return result

    def __valid_iso(self):
        name = 'iso'
        value = self.__iso
        return (
            self._is_integer(name, value)
            and self._is_include_in(100, 800, name, value)
        )

    def __valid_shutter(self):
        name = 'shutter'
        value = self.__shutter
        return (
            self._is_integer(name, value)
            and self._is_include_in(0, 6000000, name, value)
        )

    def __valid_quality(self):
        name = 'quality'
        value = self.__quality
        return (
            self._is_integer(name, value)
            and self._is_include_in(0, 100, name, value)
        )

    def __valid_sharpness(self):
        name = 'sharpness'
        value = self.__sharpness
        return (
            self._is_integer(name, value)
            and self._is_include_in(-100, 100, name, value)
        )

    def __valid_contrast(self):
        name = 'contrast'
        value = self.__contrast
        return (
            self._is_integer(name, value)
            and self._is_include_in(-100, 100, name, value)
        )

    def __valid_brightness(self):
        name = 'brightness'
        value = self.__brightness
        return (
            self._is_integer(name, value)
            and self._is_include_in(0, 100, name, value)
        )

    def __valid_saturation(self):
        name = 'saturation'
        value = self.__saturation
        return (
            self._is_integer(name, value)
            and self._is_include_in(-100, 100, name, value)
        )

    def __valid_exposure(self):
        name = 'exposure'
        value = self.__exposure
        return (
            self._is_integer(name, value)
            and self._is_include_in(0x00, 0x0c, name, value)
        )
