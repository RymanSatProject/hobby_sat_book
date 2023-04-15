# coding: utf-8

from pathlib import Path

BASE_DIR = Path('.').resolve()

# model
MODEL_DIR = BASE_DIR / 'ml/models'
TF_MODEL_FILE_PATH = MODEL_DIR / 'main.h5'
TFL_MODEL_FILE_PATH = MODEL_DIR / 'main.tflite'

# label
LABELS = ['good', 'bad']

# images
PHOTO_DIR_FOR_CLASSIFICATION = Path('/rsp01/img')
PHOTO_DIR_FOR_LEARNING = BASE_DIR / 'img/learning'
IMAGE_SIZES = (150, 150)

# tb log
TB_LOG_BASE_DIR = BASE_DIR / 'ml/logs'

# db
DB_NAME = 'classification.db'
DB_FILE_PATH = BASE_DIR / DB_NAME

# log
LOG_DIR = BASE_DIR / 'log'
IMAGE_RECOGNITION_LOG_PATH = LOG_DIR / 'start.log'  # logging.conf にも設定が書かれてあるのて注意
ABORTING_FILE_PATH = BASE_DIR / 'sys/aborting'

# work type
WORK_TYPES = {'with_tensorflow': 'tf', 'no_tensorflow': 'no', 'with_tensorflow_lite': 'tfl'}

# all or continuous
ALL_OR_CONTINUOUS = {'true': True, 'false': False, True: True, False: False, 'True': True, 'False': False}

# images
MAX_IDS_NUM = 100
MAX_SKIP_BLANK_NUM = 30

# return code to earth system
# todo: 文字列にする？
RESULTS = {
    'root': {
        'success': 10,
        'unknown_command': 11,
        'unknown_error': 12,
        'command_error': 13,
        'args_error': 14,
        'no_command_name': 71
    },
    'startPicCateg': {
        'success': 22,
        'error': 21,
        'args_error': 23
    },
    'getPicCateg': {
        'success': 60,
        'no_file': 62,
        'classification_error': 63,
        'work_type_error': 64,
        'no_classification': 61,
        'args_error': 65
    },
    'abortPicCateg': {
        'success': 40,
        'error': 41
    },
    'checkPicCateg': {
        'working': 30,
        'stop': 31,
        'aborting': 32,
        'error': 33
    },
    'getGoodPicIds': {
        'success': 50,
        'error': 51,
        'no_ids': 52
    },
    'common': {
        'start': 20
    }
}
