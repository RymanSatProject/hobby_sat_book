# coding: utf-8

import sys
from logging import config, getLogger
from pic import Pic, PicManager
from status import Status
from settings import WORK_TYPES, RESULTS, ALL_OR_CONTINUOUS


def run():
    """
    sys.argv
    0: file name: main.py
    1: command name: ['startPicCateg', 'getPicCateg', 'getPicCategWithTF',
                      'getPicIdsByCateg', 'checkPicCateg', 'abortPicCateg']
    """
    config.fileConfig('logging.conf')
    logger = getLogger()
    logger.info('start {}'.format(RESULTS['common']['start']))
    logger.info(sys.argv)
    result = RESULTS['root']['success']
    try:
        if len(sys.argv) < 2:
            result = RESULTS['root']['no_command_name']
        else:
            cmd_name = sys.argv[1]
            if cmd_name == 'startPicCateg':  # 画像分類処理を開始
                """
                sys.argv
                2: work type
                3: all or continuous
                """
                _logger = getLogger('start')
                _logger.info('start startPicCateg {}'.format(RESULTS['common']['start']))
                # todo プロセス数を数えて、すでに動いていたら処理を止める？
                if len(sys.argv) < 4:
                    _result = RESULTS['startPicCateg']['args_error']
                    result = RESULTS['root']['args_error']
                else:
                    work_type = sys.argv[2]
                    all_or_continuous = ALL_OR_CONTINUOUS[sys.argv[3]]
                    try:
                        Status.reset()
                        from ml.classification import Classification
                        runner = Classification.get_runner(work_type)
                        cl = Classification(runner, _logger)
                        cl.classify_all(all_or_continuous)
                        _result = RESULTS['startPicCateg']['success']
                    except Exception as e:
                        _logger.warning(e)
                        _result = RESULTS['startPicCateg']['error']
                        result = RESULTS['root']['command_error']
                _logger.info('finish startPicCateg {}'.format(_result))
            elif cmd_name == 'getPicCateg':  # 画像分類処理の結果を取得
                """
                sub id がなくても、sub id が 0 のデータを返すので注意（そういうのはないという前提の設計）
                sys.argv
                2: work type
                3: pic id
                4: pic sub id
                """
                _logger = getLogger('get')
                _logger.info('start getPicCateg {}'.format(RESULTS['common']['start']))
                if len(sys.argv) < 5:
                    _result = RESULTS['getPicCateg']['args_error']
                    result = RESULTS['root']['args_error']
                else:
                    work_type = sys.argv[2]  # todo check
                    pic_id = sys.argv[3]
                    pic_sub_id = sys.argv[4] if len(sys.argv) > 4 else None

                    # 65535が指定された場合は、subid指定なしと見なす（CDHでintで取得され、未指定にはできないのでここでケア）
                    if pic_sub_id == '65535':
                        pic_sub_id = None

                    _logger.info('work type: {}, pic id: {}, pic sub id: {}'.format(work_type, pic_id, pic_sub_id))
                    pic = Pic(pic_id, pic_sub_id)
                    if not pic.exists():
                        label = percent = cmd_result = RESULTS['getPicCateg']['no_file']
                    elif work_type == WORK_TYPES['no_tensorflow'] or pic.need_to_classify() is False:
                        cmd_result = RESULTS['getPicCateg']['success']
                        label, percent = pic.get_classification_result()
                    elif work_type in [WORK_TYPES['with_tensorflow_lite'], WORK_TYPES['with_tensorflow']]:
                        try:
                            Status.reset()
                            from ml.classification import Classification
                            runner = Classification.get_runner(work_type)
                            cl = Classification(runner, _logger)
                            cl.classify_one(pic)
                            cmd_result = RESULTS['getPicCateg']['success']
                            label, percent = pic.get_classification_result()
                        except Exception as e:
                            _logger.warning(e)
                            label = percent = cmd_result = RESULTS['getPicCateg']['classification_error']
                            result = RESULTS['root']['command_error']
                    else:
                        label = percent = cmd_result = RESULTS['getPicCateg']['work_type_error']
                        result = RESULTS['root']['args_error']
                _logger.info('finish getPicCateg {} {} {}'.format(cmd_result, label, percent))
            elif cmd_name == 'getGoodPicIds':  # 画像分類の結果から、good の画像 id を取得
                _logger = getLogger('get_ids')
                _logger.info('start getGoodPicIds {}'.format(RESULTS['common']['start']))
                pic_manager = PicManager()
                pic_ids = pic_manager.get_good_pic_ids()
                count = len(pic_ids)
                if count == 0:
                    pic_ids = [str(RESULTS['getGoodPicIds']['no_ids'])]
                _logger.info('finish getGoodPicIds {} {} {}'.format(RESULTS['getGoodPicIds']['success'],
                                                                    ','.join(pic_ids), count))
            elif cmd_name == 'checkPicCateg':  # 画像分類処理の状態を確認
                _logger = getLogger('check')
                _logger.info('start checkPicCateg {}'.format(RESULTS['common']['start']))
                status = Status.create()
                _logger.info('finish checkPicCateg {}'.format(status.value))
            elif cmd_name == 'abortPicCateg':  # 画像分類処理の停止
                _logger = getLogger('abort')
                _logger.info('start abortPicCateg {}'.format(RESULTS['common']['start']))
                Status.abort()
                _logger.info('finish abortPicCateg {}'.format(RESULTS['abortPicCateg']['success']))
            else:
                logger.warning('unknown commands')
                result = RESULTS['root']['unknown_command']
    except Exception as e:
        logger.warning(e)
        result = RESULTS['root']['unknown_error']
    logger.info('finish {}'.format(result))


if __name__ == '__main__':
    run()
