# -*- coding: utf-8 -*-

import subprocess,json
from collections import OrderedDict
from mission_chat.src.common.Logger import Logger
from mission_chat.src.Constants import *
from mission_chat.src.ErrorCode import *

## 応答メッセージ精製プロセスの起動コマンド
class BootPredictProcessor:
    __resultKey = "result"

    def __init__(self):
        self.logger = Logger.get_logger()

    """
    応答メッセージ生成プロセスを起動する。
    入力無しで、出力はJSON型文字列
    """
    def bootPredictProc(self):
        try:
            return self.__bootPredictProc_impl()
        except:
            import traceback
            self.logger.error(traceback.format_exc())
            ret = OrderedDict()
            ret[self.__resultKey] = UNKNOWN_ERROR
            return json.dumps(ret)

    """
    応答メッセージ生成プロセスを起動する。
    入力無しで、出力はJSON型文字列
    実体を持つメソッド
    """
    def __bootPredictProc_impl(self):
        self.logger.info("start booting predictor process.")
        execShell = '{0}'.format(predicorFilePath)
        self.logger.info("execShell={0}".format(execShell))

        #for python 2
        ret = subprocess.call(args=execShell,shell=True,stderr=subprocess.STDOUT)
        #for python 3
        #ret = subprocess.run(args=execShell,shell=True,stderr=subprocess.STDOUT,stdout=subprocess.PIPE)
        #self.logger.info("end booting predictor process. ret:" + str(ret.returncode))
        #for line in ret.stdout.splitlines():
        #    print(">>>" +line.decode("utf-8"))

        result_dict = {}
        if ret == 0:
            result_dict[self.__resultKey] = BOOTPROC_OK
        else:
            result_dict[self.__resultKey] = BOOTPROC_NG

        self.logger.info("end booting predictor process. return:{0}".format(ret))
        return json.dumps(result_dict)

if __name__ == '__main__':
    booter = BootPredictProcessor()
    print(booter.bootPredictProc())
