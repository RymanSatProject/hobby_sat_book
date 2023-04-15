# -*- coding: utf-8 -*-
import glob
import json
from collections import OrderedDict

from mission_camera.camera.camera import Camera
from mission_main.command.command_base import CommandBase


class GetPicListCommand(CommandBase):
    command_name = "getPicList"

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")
        id_list = []
        result = 0
        
        if self.__valid_args(args):
            start_id = args.get("start")
            end_id = args.get("end")
            id_list = self.__get_pic_list(start_id, end_id)
        else:
            result = 1
        
        # 応答メッセージ作成
        response = OrderedDict()
        response['cmd'] = self.command_name
        ret_arg = OrderedDict()
        ret_arg['list'] = id_list
        ret_arg['count'] = len(id_list)
        ret_arg['result'] = result
        response['arg'] = ret_arg
        encoded_response = json.dumps(response).replace(u" ", "")
        self.logger.debug("response: " + encoded_response)

        return encoded_response
    

    def __valid_args(self, args):
        start_id = args.get("start", -1)
        end_id = args.get("end", -1)

        if not type(start_id) is int or start_id < 0:
            return False

        if not type(end_id) is int or end_id < 0:
            return False

        return True

    
    def __get_pic_list(self, start_id, end_id):
        file_list = glob.glob("/rsp01/img/image_*.jpg")
        id_list = []

        for file in file_list:
            if file.find("thumb") == -1:
                file_name = (file.split("/"))[-1]
                pic_id = file_name[6:9]  # idを取得
                self.logger.debug(str(pic_id) + ", " + str(start_id) + ", " + str(end_id))

                try:
                    if int(pic_id) in range(int(start_id), int(end_id) + 1):
                        id_list.append(file_name[6:-4])  # image_ と .jpgを取り除きIDとSubIDを取得
                except ValueError as e:
                    self.logger.exception(e)
        
        id_list.sort()
        return id_list
