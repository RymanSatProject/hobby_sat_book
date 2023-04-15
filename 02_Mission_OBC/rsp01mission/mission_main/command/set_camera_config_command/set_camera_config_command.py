# -*- coding: utf-8 -*-
from mission_camera.camera.camera import Camera
from mission_camera.camera.camera_config import CameraConfigProperty
from mission_main.command.command_base import CommandBase
from .set_camera_config_command_validator import SetCameraConfigCommandValidator


class SetCameraConfigCommand(CommandBase):
    command_name = "setCamConfig"
    _MODE_GET = 0
    _MODE_SET = 1

    EXPOSURE_MODE = {
        0x00: 'off',
        0x01: 'auto',
        0x02: 'night',
        0x03: 'nightpreview',
        0x04: 'backlight',
        0x05: 'spotlight',
        0x06: 'sports',
        0x07: 'snow',
        0x08: 'beach',
        0x09: 'verylong',
        0x0a: 'fixedfps',
        0x0b: 'antishake',
        0x0c: 'fireworks'
    }

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")
        mode = args.get("mode")

        if mode == self._MODE_SET:
            return self.__execute_set_camera_config(args)
        elif mode == self._MODE_GET:
            return self.__execute_get_camera_config(args)
        else:
            self.logger.error(
                "{0}: mode must be 0(MODE_GET) or 1(MODE_SET). value:{1}"
                .format(self.command_name, mode)
            )
            return self._build_response(self.command_name, {'result': 1})

    def __execute_set_camera_config(self, args):
        result = 0
        validator = SetCameraConfigCommandValidator(args)
        if validator.valid():
            property_dict = validator.to_dict()
            if CameraConfigProperty.EXPOSURE in property_dict:
                exposure = property_dict[CameraConfigProperty.EXPOSURE]
                property_dict[CameraConfigProperty.EXPOSURE] = self.EXPOSURE_MODE[exposure]

            result = self.camera.set_config(property_dict)
        else:
            result = 1

        return self._build_response(self.command_name, {'result': result})

    def __execute_get_camera_config(self, args):
        propeties = self.camera.config.get_all_property()
        keys = [
            k for k, v in self.EXPOSURE_MODE.items() if v == propeties['exposure']
        ]
        return super()._build_response(
            self.command_name,
            {
                'brightness': propeties['brightness'],
                'contrast': propeties['contrast'],
                'exposure': keys[0] if keys else 0xFF,
                'iso': propeties['iso'],
                'quality': propeties['quality'],
                'saturation': propeties['saturation'],
                'sharpness': propeties['sharpness'],
                'shutter': propeties['shutter'],
            }
        )
