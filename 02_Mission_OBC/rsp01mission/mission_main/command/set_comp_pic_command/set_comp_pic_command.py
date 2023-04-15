# -*- coding: utf-8 -*-
from mission_camera.camera.camera import Camera
from mission_main.command.command_base import CommandBase
from .set_comp_pic_command_validator import SetCompPicCommandValidator


class SetCompPicCommand(CommandBase):
    command_name = "setCompPic"

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        validator = SetCompPicCommandValidator(args)
        if validator.valid():
            result = self.camera.set_config(validator.to_dict())
        else:
            result = 1

        response = self._build_response(self.command_name, {'result': result})
        self.logger.debug("response: {0}".format(response))

        return response
