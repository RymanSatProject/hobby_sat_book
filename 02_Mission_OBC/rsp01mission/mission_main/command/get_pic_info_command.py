# -*- coding: utf-8 -*-
import glob
import json
from collections import OrderedDict

from mission_camera.camera.camera import Camera
from mission_main.command.command_base import CommandBase


class GetPicInfoCommand(CommandBase):
    command_name = "getPicInfo"
    THUMBNAIL_ONLY = 0
    TYPE_ORIGINAL = 1

    def __init__(self):
        super().__init__(self.command_name)
        self.camera = Camera.get_instance()

    def onExecute(self, args):
        self.logger.info(self.command_name + " command onExecute is called")

        result = 0
        info_list = []

        if self.__valid_args(args):
            info_list = self.__get_pic_info(args.get("start"), args.get("end"), args.get("type"))
        else:
            result = 1

        # 応答メッセージ作成
        response = OrderedDict()
        response["cmd"] = self.command_name
        ret_arg = OrderedDict()
        ret_arg["result"] = result
        ret_arg["count"] = len(info_list)
        ret_arg["info"] = info_list

        # ret_arg["earth"] = 1  # TODO 機械学習モジュールから取得？
        response["arg"] = ret_arg
        encoded_response = json.dumps(response).replace(u" ", "")
        self.logger.debug("response: " + encoded_response)

        return encoded_response

    def __valid_args(self, args):
        start_id = args.get("start", -1)
        end_id = args.get("end", -1)
        image_type = args.get("type", -1)

        if type(start_id) is not int or start_id < 0:
            return False

        if type(end_id) is not int or end_id < 0:
            return False

        if image_type not in [self.THUMBNAIL_ONLY, self.TYPE_ORIGINAL]:
            return False

        return True

    def __get_pic_info(self, start_id, end_id, image_type):
        file_list = glob.glob("/rsp01/img/image_*.jpg")
        id_list = []
        info_list = []

        for file in file_list:
            if file.find("thumb") == -1:
                file_name = (file.split("/"))[-1]
                pic_id = file_name[6:9]  # idを取得

                try:
                    if int(pic_id) in range(int(start_id), int(end_id) + 1):
                        id_list.append(file_name[6:-4])  # image_ と .jpgを取り除きIDとSubIDを取得
                except ValueError as e:
                    self.logger.exception(e)

        id_list.sort()

        for id in id_list:
            pic_id = id[0:3]
            pic_sub_id = ""
            if len(id) > 3:
                pic_sub_id = id[-4:]

            if image_type == self.TYPE_ORIGINAL:
                filename = self.camera.seq_to_filename(pic_id, pic_sub_id)
            else:
                filename = self.camera.seq_to_thumb_filename(pic_id, pic_sub_id)

            try:
                width, height, filesize = self.camera.get_image_info(filename)
            except FileNotFoundError as ex:
                self.logger.error(ex)
                continue

            if pic_sub_id == "":
                # sub_idがないこと示す65535(0xFFFF)を代入
                pic_sub_id = "65535"

            info = OrderedDict()
            info["id"] = int(pic_id)
            info["sub_id"] = int(pic_sub_id)
            info["type"] = image_type
            info["width"] = width
            info["height"] = height
            info["size"] = filesize

            info_list.append(info)

        return info_list
