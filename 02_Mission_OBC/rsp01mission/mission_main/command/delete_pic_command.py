# -*- coding: utf-8 -*-
import json
from collections import OrderedDict

from mission_camera.camera.camera import Camera
from mission_main.command.command_base import CommandBase


class DeletePicCommand(CommandBase):
    command_name = "deletePic"
    ORIGINAL_IMAGE_ONLY = 0
    THUMBNAIL_ONLY = 1
    ORIGINAL_AND_THUMBNAIL = 2

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        if self.__valid_args(args):
            pic_id = args.get("id")
            pic_mode = args.get("mode")
            pic_sub_id = args.get("sub_id", -1)
            if pic_sub_id < 0 or 0x8000 < pic_sub_id:
                pic_sub_id = ""
            
            result = self.delete_picture(pic_id, pic_sub_id, pic_mode)
        else:
            self.logger.error("Invalid Request: {}".format(json.dumps(args)))
            result = 1

        # 応答メッセージ作成
        response = OrderedDict()
        response['cmd'] = self.command_name
        ret_arg = OrderedDict()
        ret_arg['result'] = result
        response['arg'] = ret_arg
        encoded_response = json.dumps(response).replace(u" ", "")
        self.logger.debug("response: " + encoded_response)

        return encoded_response

    
    def __valid_args(self, args):
        pic_id = args.get("id")
        if pic_id is None or not type(pic_id) == int or pic_id < 0:
            return False
        
        pic_mode = args.get("mode")
        MODE_LIST = [
            self.ORIGINAL_IMAGE_ONLY, 
            self.THUMBNAIL_ONLY, 
            self.ORIGINAL_AND_THUMBNAIL
        ]
        if pic_mode is None or pic_mode not in MODE_LIST:
            return False

        # sub_idの指定ありの場合のみ、検証
        pic_sub_id = args.get("sub_id")
        if pic_sub_id and (not type(pic_sub_id) == int or pic_sub_id < 0):
            return False
        
        return True
    

    def delete_picture(self, pic_id, pic_sub_id, pic_mode):
        try:
            if pic_mode == self.ORIGINAL_IMAGE_ONLY:
                filename = self.camera.seq_to_filename(pic_id, pic_sub_id)
                self.camera.delete_image(filename)
            elif pic_mode == self.THUMBNAIL_ONLY:
                filename = self.camera.seq_to_thumb_filename(pic_id, pic_sub_id)
                self.camera.delete_image(filename)
            else:
                filename = self.camera.seq_to_thumb_filename(pic_id, pic_sub_id)
                self.camera.delete_image(filename)
                filename = self.camera.seq_to_filename(pic_id, pic_sub_id)
                self.camera.delete_image(filename)
            return 0
        except FileNotFoundError as ex:
            self.logger.error(ex)
            return 1
