# coding: utf-8

import numpy as np
import tensorflow as tf
from keras.models import load_model
from logging import config, getLogger
from PIL import Image

from pic import PicManager
from settings import TF_MODEL_FILE_PATH, TFL_MODEL_FILE_PATH, IMAGE_SIZES
from status import Status


class Classification(object):

    @classmethod
    def get_runner(cls, work_type):
        """
        default は tfl
        """
        if work_type == 'tf':
            return cls.RunnerTF()
        elif work_type == 'tfl':
            return cls.RunnerTFL()
        return cls.RunnerTFL()

    def __init__(self, runner, logger=None):
        """
        tflite を優先する
        """
        self._logger = logger if logger is not None else self._get_logger()
        self._runner = runner

    @classmethod
    def _get_logger(cls):
        config.fileConfig('logging.conf')
        return getLogger('start')

    def classify_all(self, all_or_continuous=False):
        """
        all_or_continuous が True なら最初から全件分類する
        """
        pic_manager = PicManager(all_or_continuous)
        for pic in pic_manager.next_pic():
            if pic is None:
                self._logger.debug('no pic for classification')
                return
            if Status.create() == Status.ABORTING:
                self._logger.info('find aborting file')
                return
            self.classify_one(pic, True)

    def classify_one(self, pic, by_all=False):
        if Status.create() == Status.ABORTING:
            self._logger.info('find aborting file')
            return
        self._logger.debug(pic)
        if not pic.exists():
            self._logger.debug('no file')
            return
        if not pic.need_to_classify():
            self._logger.debug('not need to classify')
            return
        label, percentage = self._runner.classify_image(pic.image_file_path)
        self._logger.debug('label: {}, percent: {}'.format(label, percentage))
        pic.save_classification_result(label, percentage, by_all)

    class RunnerTF(object):

        def __init__(self):
            self._model = load_model(str(TF_MODEL_FILE_PATH))

        def classify_image(self, image_file_path):
            image = Image.open(image_file_path).convert('RGB').resize(IMAGE_SIZES)
            img_as_array = np.array([np.asarray(image)])
            result = self._model.predict([img_as_array])[0]
            most_possible_index = result.argmax()
            percentage = int(result[most_possible_index] * 100)
            return most_possible_index, percentage

    class RunnerTFL(object):

        def __init__(self):
            self._interpreter = tf.contrib.lite.Interpreter(model_path=str(TFL_MODEL_FILE_PATH))

        def classify_image(self, image_file_path):
            image = Image.open(image_file_path).convert('RGB').resize(IMAGE_SIZES)
            img_as_array = np.array([np.asarray(image)])
            self._interpreter.allocate_tensors()
            input_details = self._interpreter.get_input_details()
            output_details = self._interpreter.get_output_details()
            self._interpreter.set_tensor(input_details[0]['index'], np.array(img_as_array, dtype=np.float32))
            self._interpreter.invoke()
            prediction = self._interpreter.get_tensor(output_details[0]['index'])
            most_possible_index = prediction.argmax()
            percentage = int(prediction[0][most_possible_index] * 100)
            return most_possible_index, percentage
