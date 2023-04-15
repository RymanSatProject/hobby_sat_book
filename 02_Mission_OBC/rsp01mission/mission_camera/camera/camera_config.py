# -*- coding: utf-8 -*-
import json
import os
from enum import Enum
from collections import OrderedDict
from mission_camera.camera import get_module_logger


class CameraConfigProperty(str, Enum):
    SEQ = "seq"  # 現在の撮影シーケンスID（画像ファイルの連番）
    WIDTH = "width"  # 撮影画像の幅
    HEIGHT = "height"  # 撮影画像の高さ
    THUMB_WIDTH = "thumb_width"  # サムネイル画像の幅
    THUMB_HEIGHT = "thumb_height"  # サムネイル画像の高さ
    JPEG_QUALITY = "quality"  # JPEG品質 0 - 100
    ISO = "iso"  # ISO感度 100 - 800
    SHARPNESS = "sharpness"  # -100 - 100
    CONTRAST = "contrast"  # -100 - 100
    BRIGHTNESS = "brightness"  # 0 - 100
    SATURATION = "saturation"  # -100 - 100
    EXPOSURE = "exposure"  # -10 - 10
    SHUTTER = "shutter"  # 0 - 6000000us


class CameraConfig:
    _CONFIG_FILE = "/rsp01/mission_camera/camera_config.json"

    CAMERA_CONTROL_OPTION = {
        CameraConfigProperty.WIDTH: "-w",
        CameraConfigProperty.HEIGHT: "-h",
        CameraConfigProperty.JPEG_QUALITY: "-q",
        CameraConfigProperty.ISO: "-ISO",
        CameraConfigProperty.SHARPNESS: "-sh",
        CameraConfigProperty.CONTRAST: "-co",
        CameraConfigProperty.BRIGHTNESS: "-br",
        CameraConfigProperty.SATURATION: "-sa",
        CameraConfigProperty.EXPOSURE: "-ex",
        CameraConfigProperty.SHUTTER: "-ss"
    }

    def __init__(self):
        self.logger = get_module_logger(__name__)
        self._seq = 0
        self._width = 0
        self._height = 0
        self._thumb_width = 0
        self._thumb_height = 0
        self._quality = 0
        self._iso = 0
        self._sharpness = 0
        self._contrast = 0
        self._brightness = 0
        self._saturation = 0
        self._exposure = 0x00
        self._shutter = 0

        self.load()

    @property
    def seq(self):
        return self._seq

    @seq.setter
    def seq(self, seq):
        self._seq = seq

    @property
    def width(self):
        return self._width

    @width.setter
    def width(self, width):
        self._width = width

    @property
    def height(self):
        return self._height

    @height.setter
    def height(self, height):
        self._height = height

    @property
    def thumb_width(self):
        return self._thumb_width

    @thumb_width.setter
    def thumb_width(self, thumb_width):
        self._thumb_width = thumb_width

    @property
    def thumb_height(self):
        return self._thumb_height

    @thumb_height.setter
    def thumb_height(self, thumb_height):
        self._thumb_height = thumb_height

    @property
    def quality(self):
        return self._quality

    @quality.setter
    def quality(self, quality):
        self._quality = quality

    @property
    def iso(self):
        return self._iso

    @iso.setter
    def iso(self, iso):
        self._iso = iso

    @property
    def sharpness(self):
        return self._sharpness

    @sharpness.setter
    def sharpness(self, sharpness):
        self._sharpness = sharpness

    @property
    def contrast(self):
        return self._contrast

    @contrast.setter
    def contrast(self, contrast):
        self._contrast = contrast

    @property
    def brightness(self):
        return self._brightness

    @brightness.setter
    def brightness(self, brightness):
        self._brightness = brightness

    @property
    def saturation(self):
        return self._saturation

    @saturation.setter
    def saturation(self, saturation):
        self._saturation = saturation

    @property
    def exposure(self):
        return self._exposure

    @exposure.setter
    def exposure(self, exposure):
        self._exposure = exposure

    @property
    def shutter(self):
        return self._shutter

    @shutter.setter
    def shutter(self, shutter):
        self._shutter = shutter

    def load(self):
        if not os.path.exists(self._CONFIG_FILE):
            self.logger.error("Camera config file is not exist")
            return

        with open(self._CONFIG_FILE, 'r') as config_file:
            config = json.load(config_file)
            self.seq = config[CameraConfigProperty.SEQ]
            self.width = config[CameraConfigProperty.WIDTH]
            self.height = config[CameraConfigProperty.HEIGHT]
            self.thumb_width = config[CameraConfigProperty.THUMB_WIDTH]
            self.thumb_height = config[CameraConfigProperty.THUMB_HEIGHT]
            self.quality = config[CameraConfigProperty.JPEG_QUALITY]
            self.iso = config[CameraConfigProperty.ISO]
            self.sharpness = config[CameraConfigProperty.SHARPNESS]
            self.contrast = config[CameraConfigProperty.CONTRAST]
            self.brightness = config[CameraConfigProperty.BRIGHTNESS]
            self.saturation = config[CameraConfigProperty.SATURATION]
            self.exposure = config[CameraConfigProperty.EXPOSURE]
            self.shutter = config[CameraConfigProperty.SHUTTER]

            self.logger.debug("Load camera config")
            self.logger.debug(json.dumps(self.get_all_property()))

    def update(self, property_dict):
        try:
            if CameraConfigProperty.ISO in property_dict:
                self.iso = property_dict[CameraConfigProperty.ISO]

            if CameraConfigProperty.SHUTTER in property_dict:
                self.shutter = property_dict[CameraConfigProperty.SHUTTER]

            if CameraConfigProperty.JPEG_QUALITY in property_dict:
                self.quality = property_dict[CameraConfigProperty.JPEG_QUALITY]

            if CameraConfigProperty.SHARPNESS in property_dict:
                self.sharpness = property_dict[CameraConfigProperty.SHARPNESS]

            if CameraConfigProperty.CONTRAST in property_dict:
                self.contrast = property_dict[CameraConfigProperty.CONTRAST]

            if CameraConfigProperty.BRIGHTNESS in property_dict:
                self.brightness = property_dict[CameraConfigProperty.BRIGHTNESS]

            if CameraConfigProperty.SATURATION in property_dict:
                self.saturation = property_dict[CameraConfigProperty.SATURATION]

            if CameraConfigProperty.EXPOSURE in property_dict:
                self.exposure = property_dict[CameraConfigProperty.EXPOSURE]

            if CameraConfigProperty.WIDTH in property_dict:
                self.width = property_dict[CameraConfigProperty.WIDTH]

            if CameraConfigProperty.HEIGHT in property_dict:
                self.height = property_dict[CameraConfigProperty.HEIGHT]

            if CameraConfigProperty.THUMB_WIDTH in property_dict:
                self.thumb_width = property_dict[CameraConfigProperty.THUMB_WIDTH]

            if CameraConfigProperty.THUMB_HEIGHT in property_dict:
                self.thumb_height = property_dict[CameraConfigProperty.THUMB_HEIGHT]

            self.save()
            return 0
        except Exception as ex:
            self.logger.error(ex)
            return 1

    def save(self):
        with open(self._CONFIG_FILE, 'w') as config_file:
            json.dump(self.get_all_property(), config_file, indent=4, sort_keys=True)

    def get_all_property(self):
        result = OrderedDict()
        result[CameraConfigProperty.SEQ] = self.seq
        result[CameraConfigProperty.WIDTH] = self.width
        result[CameraConfigProperty.HEIGHT] = self.height
        result[CameraConfigProperty.THUMB_WIDTH] = self.thumb_width
        result[CameraConfigProperty.THUMB_HEIGHT] = self.thumb_height
        result[CameraConfigProperty.JPEG_QUALITY] = self.quality
        result[CameraConfigProperty.ISO] = self.iso
        result[CameraConfigProperty.SHARPNESS] = self.sharpness
        result[CameraConfigProperty.CONTRAST] = self.contrast
        result[CameraConfigProperty.BRIGHTNESS] = self.brightness
        result[CameraConfigProperty.SATURATION] = self.saturation
        result[CameraConfigProperty.EXPOSURE] = self.exposure
        result[CameraConfigProperty.SHUTTER] = self.shutter
        return result
