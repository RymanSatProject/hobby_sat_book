# -*- coding: utf-8 -*-
import datetime
import glob
import subprocess
from threading import Lock

import os
from PIL import Image

from mission_camera.camera import get_module_logger
from mission_camera.camera.camera_config import CameraConfig


class Camera:
    """
    カメラクラス。シングルトンとして存在し、各モジュールから呼び出して使用する。
    """
    _unique_instance = None
    _lock = Lock()  # クラスロック
    IMAGE_DIR = "/rsp01/img/"
    IMAGE_NAME_PREFIX = "image_"
    SUCCESS = 0
    ONE_MINUTE = 60000
    _ROTATION_NUM = 1000

    def __new__(cls):
        raise NotImplementedError('Cannot initialize via Constructor')

    @classmethod
    def __internal_new__(cls):
        return super().__new__(cls)

    @classmethod
    def get_instance(cls):
        if not cls._unique_instance:
            with cls._lock:
                if not cls._unique_instance:
                    cls._unique_instance = cls.__internal_new__()
                    cls.initialize(cls)
        return cls._unique_instance

    def initialize(self):
        self.logger = get_module_logger(__name__)
        if not os.path.exists(self.IMAGE_DIR):
            os.makedirs(self.IMAGE_DIR)
        self.config = CameraConfig()

    def set_config(self, config_dict):
        """
        カメラの設定を更新して保存する。
        :param config_dict:設定したいカメラ設定(Dict)
        :return:
        """
        return self.config.update(config_dict)

    def increment_sequence_number(self):
        next_seq = (self.config.seq + 1) % self._ROTATION_NUM
        self.config.seq = next_seq
        self.config.save()

    def capture(self, start_timer, interval_millis, duration_millis):
        """
        撮影を実行する。
        :param start_timer: 撮影を開始するまでのタイマーをミリ秒で指定する、0のときは即時撮影する。
        :param interval_millis: 連続撮影の撮影間隔をミリ秒で指定する、0のときはベストエフォートで連続撮影を実施する。
        :param duration_millis: 連続撮影を行う期間をミリ秒で指定する、0のときは1枚のみ撮影する。
        :return: 成功時 -> 0, 失敗時 -> 0以外
        """
        cmd = ""
        file_name = self.IMAGE_NAME_PREFIX + "{0:03d}".format(self.config.seq)

        capture_cmd = self.capture_cmd(
            self.IMAGE_DIR,
            file_name,
            interval_millis,
            duration_millis,
        )
        create_thumb_cmd = self.create_thumb_cmd(
            self.IMAGE_DIR,
            file_name,
            self.config.width,
            self.config.height,
            self.config.thumb_width,
            self.config.thumb_height,
        )

        if start_timer > 0:
            sleepenh_cmd, sleepenh_at_cmd = self.sleepenh_cmd(start_timer)
            cmd = "{0} && {1} && {2}".format(sleepenh_cmd, capture_cmd, create_thumb_cmd)

            if sleepenh_at_cmd:
                cmd = cmd.replace('\"', '\'')
                cmd = "echo \"" + cmd + "\"" + sleepenh_at_cmd
        else:
            cmd = "{0} && {1}".format(capture_cmd, create_thumb_cmd)

        # Memo: バックグラウンドで実行させる
        cmd += " &"

        self.logger.debug("exec command: " + cmd)
        return_code = subprocess.call(cmd, shell=True)
        if return_code == self.SUCCESS:
            self.increment_sequence_number()

        return return_code

    def capture_cmd(self, image_dir, file_name, interval_millis, duration_millis):
        cmd = ""
        file_path = image_dir + file_name

        if interval_millis >= 0 and duration_millis > 0:
            cmd = "raspistill -o {0}_%04d.jpg -tl {1} -t {2}" \
                .format(file_path, interval_millis, duration_millis)
        else:
            cmd = "raspistill -o {0}.jpg".format(file_path)

        cmd += " " + self.get_camera_option()

        return cmd

    def sleepenh_cmd(self, start_timer):
        cmd = ""
        at_cmd = ""
        if 0 < start_timer < self.ONE_MINUTE:
            cmd = "sleepenh {0} > /dev/null 2>&1".format(self.sleep_time(start_timer))
        elif start_timer >= self.ONE_MINUTE:
            # MEMO: atコマンドで指定。分指定なのでそれ以下はsleepenhコマンドで指定
            cmd = "sleepenh {0}".format(self.sleep_time(start_timer))
            at_cmd = " | at -M {0} > /dev/null 2>&1" \
                .format(self.target_time(start_timer).strftime("%H:%M %m%d%Y"))

        return (cmd, at_cmd)

    def target_time(self, start_timer):
        delta = datetime.timedelta(
            milliseconds=(start_timer % 1000),
            seconds=int(start_timer / 1000 % 60),
            minutes=int(start_timer / 1000 / 60 % 60),
            hours=int(start_timer / 1000 / 60 / 60),
        )
        return datetime.datetime.now() + delta

    def sleep_time(self, start_timer):
        if start_timer < self.ONE_MINUTE:
            return (float)(start_timer % self.ONE_MINUTE) / 1000
        else:
            target = self.target_time(start_timer)
            return "{0}.{1}".format(target.strftime("%S"), target.strftime("%f")[:3])

    def create_thumb_cmd(self, image_dir, file_name, width, height, thumb_width, thumb_height):
        return "find {0} -type f -name '{1}*.jpg'".format(image_dir, file_name) \
            + " -execdir sh -ec \"f={}; " \
            + "convert " \
            + "-verbose " \
            + "-define jpeg:size={0}x{1} ".format(width, height) \
            + "-thumbnail {0}x{1} ".format(thumb_width, thumb_height) \
            + "-quality 100 " \
            + "\\${f##*/} \\${f%.*}_thumb.jpg\" \\;"

    def get_list_images(self):
        """
        画像保存ディレクトリのファイル名一覧を取得
        :return: ファイル名一覧
        """
        return glob.glob(self.IMAGE_DIR)

    def delete_image(self, filename):
        """
        画像保存ディレクトリ内の画像ファイルを削除する
        :param filename: 削除するファイルの名前
        :return:
        """
        filepath = self.IMAGE_DIR + filename
        os.remove(filepath)

    def create_thumbnail(self, filename):
        """
        画像のサムネイルを作成する。サムネイル画像ファイル名は元画像のファイル名末尾に"_thumb"を付加したものになる。
        :param filename: サムネイルを作成する元画像のファイル名
        :return:
        """
        name, ext = os.path.splitext(filename)
        filename_thumb = name + "_thumb" + ext
        img = Image.open(self.IMAGE_DIR + filename, 'r')
        thumbnail_size = (self.config.thumb_width, self.config.thumb_height)
        img.thumbnail(thumbnail_size)
        img.save(self.IMAGE_DIR + filename_thumb, "JPEG")

    def get_image_info(self, filename):
        """
        画像の幅、高さ、ファイルサイズを取得する
        :param filepath 情報を取得するファイルの名前
        :return (width, height, size) 画像の幅, 画像の高さ, 画像ファイルサイズ
        """
        filepath = self.IMAGE_DIR + filename
        image = Image.open(filepath)
        width, height = image.size
        size = os.path.getsize(filepath)
        return width, height, size

    def seq_to_filename(self, seq, sub_seq):
        """
        撮影シーケンス番号から画像ファイルのパスに変換する
        :param seq: 撮影シーケンス番号
        :param sub_seq: 連続撮影時のサブシーケンス番号、省略時はから文字を指定
        :return: 画像ファイルパス
        """
        if sub_seq == "":
            return self.IMAGE_NAME_PREFIX + "{0:03d}".format(int(seq)) + ".jpg"
        else:
            return self.IMAGE_NAME_PREFIX + "{0:03d}".format(int(seq)) + "_{0:04d}".format(int(sub_seq)) + ".jpg"

    def seq_to_thumb_filename(self, seq, sub_seq):
        """
        撮影シーケンス番号からサムネイル画像ファイルのパスに変換する
        :param seq: 撮影シーケンス番号
        :param sub_seq: 連続撮影時のサブシーケンス番号、省略時はから文字を指定
        :return: 画像ファイルパス
        """
        if sub_seq == "":
            return self.IMAGE_NAME_PREFIX + "{0:03d}".format(int(seq)) + "_thumb.jpg"
        else:
            return self.IMAGE_NAME_PREFIX + "{0:03d}".format(int(seq)) + "_{0:04d}".format(
                int(sub_seq)) + "_thumb.jpg"

    def get_camera_option(self):
        camera_option = ""
        properties = self.config.get_all_property()

        for key, val in properties.items():
            if key in self.config.CAMERA_CONTROL_OPTION:
                opt = self.config.CAMERA_CONTROL_OPTION.get(key)
                opt = opt + " " + str(val)
                camera_option += opt + " "

        self.logger.debug("current camera option: " + camera_option)
        return camera_option

    def cancel_capture_timer(self):
        """
        1分以上のタイマー撮影をすべてキャンセルする。1分未満の撮影はキャンセルできない。
        :return: 成功時 -> 0, 失敗時 -> 0以外
        """
        cmd = "atq | cut -f 1 | xargs atrm"
        self.logger.debug("exec command: " + cmd)

        return_code = subprocess.call(cmd, shell=True)
        return return_code
