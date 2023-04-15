# coding: utf-8

import numpy as np
import random
from keras.callbacks import TensorBoard
from keras.layers import Conv2D, MaxPooling2D, Activation, Dropout, Flatten, Dense
from keras.models import load_model, Sequential
from keras.optimizers import rmsprop
from keras.utils import np_utils
from PIL import Image

from settings import MODEL_DIR, PHOTO_DIR_FOR_LEARNING, TB_LOG_BASE_DIR, LABELS


class Learning(object):
    IMAGE_SIZES = (150, 150)
    SAMPLE_SIZE = 2500
    TEST_IMAGE_NUM = SAMPLE_SIZE / 10
    FILE_SUFFIXES = ['.png', '.jpg', '.jpeg', '.JPG']
    MODULE_NAME = __name__.split('.')[-1]
    MODEL_FILE_PATH = MODEL_DIR / (MODULE_NAME + '.h5')
    MODEL_FILE_PATH_WO_OPT = MODEL_DIR / (MODULE_NAME + '_wo_opt.h5')
    TB_LOG_DIR = TB_LOG_BASE_DIR / MODULE_NAME

    def __init__(self):
        try:
            self.TB_LOG_DIR.mkdir()
        except OSError:
            # file exists
            pass
        if self.MODEL_FILE_PATH.exists():
            self.model = load_model(str(self.MODEL_FILE_PATH))
            self.load_model_flg = True
        else:
            self.model = Sequential()
            self.load_model_flg = False

    def remove_optimizer_data(self):
        model = load_model(str(self.MODEL_FILE_PATH))
        model.save(str(self.MODEL_FILE_PATH_WO_OPT), include_optimizer=False)

    def learn(self):
        """
        model を保存し、loss と accuracy を返す
        """
        x_train, x_test, y_train, y_test = self._load_images_as_np_array()

        # tf が処理しやすいように 256 の RGB を 0 ~ 1 の範囲に変換する
        x_train = x_train.astype('float32') / 255
        x_test = x_test.astype('float32') / 255

        # 正解データを 0, 1 に変換する
        y_train = np_utils.to_categorical(y_train, len(LABELS))
        y_test = np_utils.to_categorical(y_test, len(LABELS))

        print('x_train shape:', x_train.shape)
        print(x_train.shape[0], 'train samples')
        print(x_test.shape[0], 'test samples')

        self.model = self._train_model_with_cnn(self.model, x_train, y_train, x_test, y_test, self.load_model_flg)
        self.model.save(str(self.MODEL_FILE_PATH))
        scores = self.model.evaluate(x_test, y_test, verbose=1)
        return scores[0], scores[1]

    def _load_images_as_np_array(self):
        x_train, x_test, y_train, y_test = [], [], [], []
        for index, label in enumerate(LABELS):
            photos_path = PHOTO_DIR_FOR_LEARNING / label
            files = photos_path.glob('*')
            for file_index, _file in enumerate(random.sample(list(files), self.SAMPLE_SIZE)):
                if file_index % 100 == 0:
                    print(label, file_index)
                if _file.suffix not in self.FILE_SUFFIXES:
                    print('irregular file suffix: {}'.format(_file))
                    continue
                image = Image.open(_file).convert('RGB').resize(self.IMAGE_SIZES)
                if file_index < self.TEST_IMAGE_NUM:
                    x_test.append(np.asarray(image))
                    y_test.append(index)
                else:
                    x_train.append(np.asarray(image))
                    y_train.append(index)
        return np.array(x_train), np.array(x_test), np.array(y_train), np.array(y_test)

    def _train_model_with_cnn(self, model, x_train, y_train, x_test, y_test, load_model_flg):
        if not load_model_flg:
            model.add(Conv2D(32, (3, 3), padding='same', input_shape=x_train.shape[1:]))
            model.add(Activation('relu'))
            model.add(Conv2D(32, (3, 3)))
            model.add(Activation('relu'))
            model.add(MaxPooling2D(pool_size=(2, 2)))
            model.add(Dropout(0.25))

            model.add(Conv2D(64, (3, 3), padding='same'))
            model.add(Activation('relu'))
            model.add(Conv2D(64, (3, 3)))
            model.add(Activation('relu'))
            model.add(MaxPooling2D(pool_size=(2, 2)))
            model.add(Dropout(0.25))

            model.add(Flatten())
            model.add(Dense(512))
            model.add(Activation('relu'))
            model.add(Dropout(0.5))
            model.add(Dense(len(LABELS)))
            model.add(Activation('softmax'))

            opt = rmsprop(lr=0.0001, decay=1e-6)
            model.compile(loss='categorical_crossentropy', optimizer=opt, metrics=['accuracy'])
        model.fit(x_train, y_train, batch_size=32, epochs=200,
                  validation_data=(x_test, y_test), shuffle=True, callbacks=[TensorBoard(log_dir=str(self.TB_LOG_DIR))])
        return model
