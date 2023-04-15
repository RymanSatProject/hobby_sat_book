from pathlib import Path

# common
ROOT_DIR = Path(__file__).parents[1]

# image recognition
IMAGE_RECOGNITION_APP_DIR = ROOT_DIR / 'mission_image_recognition'
IMAGE_RECOGNITION_LOG_DIR = IMAGE_RECOGNITION_APP_DIR / 'log'
IMAGE_RECOGNITION_MAIN_SH_PATH = str(IMAGE_RECOGNITION_APP_DIR / 'sh/main.sh')
IMAGE_RECOGNITION_LOG_PATHS = {
    'root': str(IMAGE_RECOGNITION_LOG_DIR / 'root.log'),
    'startPicCateg': str(IMAGE_RECOGNITION_LOG_DIR / 'start.log'),
    'getPicCateg': str(IMAGE_RECOGNITION_LOG_DIR / 'get.log'),
    'getGoodPicIds': str(IMAGE_RECOGNITION_LOG_DIR / 'get_ids.log'),
    'checkPicCateg': str(IMAGE_RECOGNITION_LOG_DIR / 'check.log'),
    'abortPicCateg': str(IMAGE_RECOGNITION_LOG_DIR / 'abort.log'),
}

# log を参照して入れる値は、log が古い場合は先頭に「1」をつけるので、mission_image_recognition 含めすべて２桁にする
# それ以外は 200 台にとする
# ただし、shell script を実行した際に returncode が 0 出ない場合は、returncode で exe_result を上書きする
IMAGE_RECOGNITION_EXE_RESULT_CODES = {
    'args_error': '214',
    'no_log_landmark': '15',
    'no_log_file': '216',
    'failed_to_split_log': '217',
    'failed_to_convert_to_date': '222',
}
IMAGE_RECOGNITION_CMD_RESULT_CODES = {
    'unexpected_log_format': '19',
    'no_log_file': '218',
    'failed_to_split_log': '219',
    'not_execute': '201',
    'failed_to_convert_to_date': '223',
}
IMAGE_RECOGNITION_COMMON_RESULT_CODES = {
    'not_int': '220',
    'out_of_range': '221',
}
IMAGE_RECOGNITION_OLD_LOG_PREFIX = '1'

IMAGE_RECOGNITION_SLEEP_TIME_TO_START = 30
IMAGE_RECOGNITION_LOG_DELIMITER = ' '
