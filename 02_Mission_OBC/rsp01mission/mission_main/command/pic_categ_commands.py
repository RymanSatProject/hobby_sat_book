# coding: UTF-8

import time

from mission_main.command.command_base import CommandBase
from mission_main.command.mixin import ResponseMixin, ShellMixin, LogMixin
from mission_main.settings import (IMAGE_RECOGNITION_LOG_PATHS, IMAGE_RECOGNITION_EXE_RESULT_CODES,
                                   IMAGE_RECOGNITION_CMD_RESULT_CODES, IMAGE_RECOGNITION_MAIN_SH_PATH,
                                   IMAGE_RECOGNITION_SLEEP_TIME_TO_START, IMAGE_RECOGNITION_OLD_LOG_PREFIX,
                                   IMAGE_RECOGNITION_LOG_DELIMITER, IMAGE_RECOGNITION_COMMON_RESULT_CODES)


class CategBaseCommand(CommandBase, ResponseMixin, ShellMixin, LogMixin):
    common_keys_for_1byte = ['exe_result', 'cmd_result']
    default_root_log_landmark = 'finish'
    no_wait_root_log_landmark = 'start'

    def __init__(self, command_name: str, result_log_file_path: str, cmd_log_keys: list, keys_for_1byte=None):
        super().__init__(command_name)
        self.result_log_file_path = result_log_file_path
        self.cmd_log_keys = cmd_log_keys
        self.keys_for_1byte = (self.common_keys_for_1byte + keys_for_1byte
                               if type(keys_for_1byte) == list else self.common_keys_for_1byte)

    def onExecute(self, args):
        """
        shell script 起動し、終わるまで待ち、
        exe_result と cmd_result を取得し、結果を返す一番シンプルな実装
        ただし、shell script の実行が失敗すると、exe_result を returncode で上書きする
        """
        self.logger.info('{} command onExecute is called'.format(self.command_name))
        returncode, stdout, stderr = self._execute_cmd_and_wait('sh {} {}'.format(IMAGE_RECOGNITION_MAIN_SH_PATH, self.command_name))
        self.logger.debug('returncode: {}, stdout: {}, stderr: {}'.format(returncode, stdout, stderr))
        exe_result = returncode if returncode != 0 else self._get_exe_result(self.default_root_log_landmark)
        self.logger.debug('exe_result: {}'.format(exe_result))
        response_codes = self._get_cmd_log_values(self.cmd_log_keys)
        response_codes['exe_result'] = exe_result
        self.logger.debug('response_codes: {}'.format(response_codes))
        response_codes = self._replace_response_codes_if_invalid(response_codes, self.keys_for_1byte)
        self.logger.debug('replaced_response_codes: {}'.format(response_codes))
        response = self._make_response(self.command_name, response_codes)
        self.logger.debug('response: {}'.format(response))
        return response

    def _get_command_name(self) -> str:
        """
        クラス名の先頭を小文字に、末尾の Command を削除する
        """
        return type(self).__name__.lower()[0] + type(self).__name__[1: -7]

    def _get_exe_result(self, log_landmark: str, line_num_from_bottom: int = 1) -> str:
        """
        root.log にはコマンドが正常に動作したかの log が書き込まれる
        正常、及び例外を拾い終了していれば、「${log_key} ${code}」という形で log が残る
        log_landmark は start や finish が入る
        また、log が古いと「1」という prefix をつける
        todo timezone でずれるかも
        """
        try:
            last_line = self._get_code_line(IMAGE_RECOGNITION_LOG_PATHS['root'], line_num_from_bottom)
            prefix = IMAGE_RECOGNITION_OLD_LOG_PREFIX if self._is_one_hour_ago_log(last_line) else ''
            last_line_list = last_line.split(IMAGE_RECOGNITION_LOG_DELIMITER)
            if log_landmark not in last_line_list:
                return '{}{}'.format(prefix, IMAGE_RECOGNITION_EXE_RESULT_CODES['no_log_landmark'])
            return '{}{}'.format(prefix, last_line_list[-1])
        except ValueError as e:
            self.logger.warn(e)
            return IMAGE_RECOGNITION_EXE_RESULT_CODES['failed_to_convert_to_date']
        except FileNotFoundError as e:
            self.logger.warn(e)
            return IMAGE_RECOGNITION_EXE_RESULT_CODES['no_log_file']
        except IndexError as e:
            self.logger.warn(e)
            return IMAGE_RECOGNITION_EXE_RESULT_CODES['failed_to_split_log']

    def _get_cmd_log_values(self, log_keys: list, line_num_from_bottom: int = 1) -> dict:
        """
        コマンドの実行結果は、log の最終行に書き込まれている
        半角スペースで分割した際に、log の終わりから読んで、keys の数だけ値が書き込まれている
        想定外であれば、failed_to_split_log といった値をすべての value とした dict を返す
        また、log が古いと「1」という prefix をつける
        todo timezone でずれるかも
        """
        try:
            last_line = self._get_code_line(self.result_log_file_path, line_num_from_bottom)
            prefix = IMAGE_RECOGNITION_OLD_LOG_PREFIX if self._is_one_hour_ago_log(last_line) else ''
            last_line_list = last_line.split(IMAGE_RECOGNITION_LOG_DELIMITER)
            keys_len = len(log_keys)
            if len(last_line_list) < keys_len:
                return {key: '{}{}'.format(prefix, IMAGE_RECOGNITION_CMD_RESULT_CODES['unexpected_log_format'])
                        for key in log_keys}
            return {key: '{}{}'.format(prefix, last_line_list[i - keys_len]) for i, key in enumerate(log_keys)}
        except ValueError as e:
            self.logger.warn(e)
            return {key: IMAGE_RECOGNITION_CMD_RESULT_CODES['failed_to_convert_to_date'] for key in log_keys}
        except FileNotFoundError as e:
            self.logger.warn(e)
            return {key: IMAGE_RECOGNITION_CMD_RESULT_CODES['no_log_file'] for key in log_keys}
        except IndexError as e:
            self.logger.warn(e)
            return {key: IMAGE_RECOGNITION_CMD_RESULT_CODES['failed_to_split_log'] for key in log_keys}

    @classmethod
    def _replace_response_codes_if_invalid(cls, codes: dict, target_keys: list):
        for key, code in codes.items():
            if key not in target_keys:
                continue
            try:
                _code = int(code)
                if not 0 <= _code < 256:
                    codes[key] = IMAGE_RECOGNITION_COMMON_RESULT_CODES['out_of_range']
            except ValueError:
                codes[key] = IMAGE_RECOGNITION_COMMON_RESULT_CODES['not_int']
        return codes


class StartPicCategCommand(CategBaseCommand):

    def __init__(self):
        command_name = self._get_command_name()
        super().__init__(command_name, IMAGE_RECOGNITION_LOG_PATHS[command_name], ['cmd_result'])

    def onExecute(self, args):
        """
        画像分類を行う
        引数によって挙動が変わる
        wait が true だと、処理が終わるまで待つ
            tf だと、３０分以上かかる
        wait が false だと、画像分類の処理を別プロセスで実行し、正常に起動したことを確認したら地上に正常と返す
            このとき log が書き込まれるまで、念の為一定時間 sleep する
            また、log は終わりから２行目を参照する（最終行は引数が書かれてあり、コード値ではない）
        - work_type: str = tfl
            - choices: tf, no, tfl
        - all: str = false
            - choices: true, false（シリアル通信の module が True, False に変換しているっぽい）
        - wait: str = false
            - choices: true, false（シリアル通信の module が True, False に変換しているっぽい）
        """
        self.logger.info('{} command onExecute is called'.format(self.command_name))
        self.logger.info('args: {}'.format(args))
        work_type = args.get('work_type', 'tfl')
        _all = args.get('all', 'false')
        wait = args.get('wait', 'false')

        if wait in ['false', 'False', False]:
            self._execute_cmd('sh {} {} {} {}'.format(IMAGE_RECOGNITION_MAIN_SH_PATH, self.command_name,
                                                      work_type, _all))
            time.sleep(IMAGE_RECOGNITION_SLEEP_TIME_TO_START)
            exe_result = self._get_exe_result(self.no_wait_root_log_landmark, 2)
        else:
            returncode, stdout, stderr = self._execute_cmd_and_wait('sh {} {} {} {}'.format(IMAGE_RECOGNITION_MAIN_SH_PATH,
                                                                    self.command_name, work_type, _all))
            self.logger.debug('returncode: {}, stdout: {}, stderr: {}'.format(returncode, stdout, stderr))
            exe_result = returncode if returncode != 0 else self._get_exe_result(self.default_root_log_landmark, 1)
        self.logger.debug('exe_result: {}'.format(exe_result))
        response_codes = self._get_cmd_log_values(self.cmd_log_keys)
        response_codes['exe_result'] = exe_result
        self.logger.debug('response_codes: {}'.format(response_codes))
        response_codes = self._replace_response_codes_if_invalid(response_codes, self.keys_for_1byte)
        self.logger.debug('replaced_response_codes: {}'.format(response_codes))
        response = self._make_response(self.command_name, response_codes)
        self.logger.debug('response: {}'.format(response))
        return response


class GetPicCategCommand(CategBaseCommand):

    def __init__(self):
        """
        categ と percent も 0-255 の範囲に収まらない場合、別のコード値で上書きする
        """
        command_name = self._get_command_name()
        super().__init__(command_name, IMAGE_RECOGNITION_LOG_PATHS[command_name],
                         ['cmd_result', 'categ', 'percent'], ['categ', 'percent'])

    def onExecute(self, args):
        """
        引数で指定した画像 id を元に、画像分類結果を返す
        引数が不正の場合は 14, 201, 201, 201 を返す
        - work_type: str = tfl
            - choices: tf, no, tfl
        - id (required): int
        - sub_id: int = 1
        """
        self.logger.info('{} command onExecute is called'.format(self.command_name))
        self.logger.info('args: {}'.format(args))
        if 'id' not in args:
            response_codes = {
                'exe_result': IMAGE_RECOGNITION_EXE_RESULT_CODES['args_error'],
                'cmd_result': IMAGE_RECOGNITION_CMD_RESULT_CODES['not_execute'],
                'caget': IMAGE_RECOGNITION_CMD_RESULT_CODES['not_execute'],
                'percent': IMAGE_RECOGNITION_CMD_RESULT_CODES['not_execute'],
            }
        else:
            work_type = args.get('work_type', 'tfl')
            pic_id = args['id']
            pic_sub_id = args.get('sub_id', '1')
            returncode, stdout, stderr = self._execute_cmd_and_wait('sh {} {} {} {} {}'.format(IMAGE_RECOGNITION_MAIN_SH_PATH, 
                                                                    self.command_name, work_type, pic_id, pic_sub_id))
            self.logger.debug('returncode: {}, stdout: {}, stderr: {}'.format(returncode, stdout, stderr))
            exe_result = returncode if returncode != 0 else self._get_exe_result(self.default_root_log_landmark, 1)
            self.logger.debug('exe_result: {}'.format(exe_result))
            response_codes = self._get_cmd_log_values(self.cmd_log_keys)
            response_codes['exe_result'] = exe_result
        self.logger.debug('response_codes: {}'.format(response_codes))
        response_codes = self._replace_response_codes_if_invalid(response_codes, self.keys_for_1byte)
        self.logger.debug('replaced_response_codes: {}'.format(response_codes))
        response = self._make_response(self.command_name, response_codes)
        self.logger.debug('response: {}'.format(response))
        return response


class GetGoodPicIdsCommand(CategBaseCommand):
    """
    画像分類の結果、good と判定された画像の id をまとめて返す
    新しい id から数えて最大 100 まで返す
    """

    def __init__(self):
        command_name = self._get_command_name()
        super().__init__(command_name, IMAGE_RECOGNITION_LOG_PATHS[command_name],
                         ['cmd_result', 'ids', 'count'])

    def onExecute(self, args):
        """
        shell script 起動し、終わるまで待ち、
        exe_result と cmd_result を取得し、結果を返す一番シンプルな実装
        ただし、shell script の実行が失敗すると、exe_result を returncode で上書きする
        ids は配列に変換する: ["000","001_0000"]
        """
        self.logger.info('{} command onExecute is called'.format(self.command_name))
        returncode, stdout, stderr = self._execute_cmd_and_wait('sh {} {}'.format(IMAGE_RECOGNITION_MAIN_SH_PATH, self.command_name))
        self.logger.debug('returncode: {}, stdout: {}, stderr: {}'.format(returncode, stdout, stderr))
        exe_result = returncode if returncode != 0 else self._get_exe_result(self.default_root_log_landmark)
        self.logger.debug('exe_result: {}'.format(exe_result))
        response_codes = self._get_cmd_log_values(self.cmd_log_keys)
        response_codes['ids'] = self._convert_to_list(response_codes['ids'])
        response_codes['exe_result'] = exe_result
        self.logger.debug('response_codes: {}'.format(response_codes))
        response_codes = self._replace_response_codes_if_invalid(response_codes, self.keys_for_1byte)
        self.logger.debug('replaced_response_codes: {}'.format(response_codes))
        response = self._make_response(self.command_name, response_codes)
        self.logger.debug('response: {}'.format(response))
        return response

    @classmethod
    def _convert_to_list(cls, csv_line: str) -> list:
        return csv_line.split(',')


class CheckPicCategCommand(CategBaseCommand):
    """
    画像分類の処理がどうなっているかを確認し、地上に返す
    """

    def __init__(self):
        command_name = self._get_command_name()
        super().__init__(command_name, IMAGE_RECOGNITION_LOG_PATHS[command_name], ['cmd_result'])


class AbortPicCategCommand(CategBaseCommand):
    """
    画像分類の処理を停止するファイルを生成する
    プロセスを落とすような強制終了ではないので、すぐに止まらない
    """

    def __init__(self):
        command_name = self._get_command_name()
        super().__init__(command_name, IMAGE_RECOGNITION_LOG_PATHS[command_name], ['cmd_result'])
