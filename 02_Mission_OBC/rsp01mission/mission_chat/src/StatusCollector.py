# -*- coding: utf-8 -*-

import json,glob,re,subprocess,sys
from pathlib import Path
from operator import itemgetter
from collections import OrderedDict

from mission_chat.src.common.Logger import Logger
from mission_chat.src.Constants import *
from mission_chat.src.ErrorCode import *

class StatusCollector:
    # リクエストのjsonキー
    __targetKey = "target"

    # レスポンスjsonのキー
    __resultKey = "result"
    __reslockKey = "res_lock"
    __reqlockKey = "req_lock"
    __processKey = "process"
    __reqfilecountKey = "req_file_count"
    __reqfilenamesKey = "req_file_names"
    __resfilecountKey = "res_file_count"
    __resfilenamesKey = "res_file_names"
    __sepfilecountKey = "sep_file_count"
    __sepfilenamesKey = "sep_file_names"


    def __init__(self):
        self.logger = Logger.get_logger()

    """
        辞書型のjsonを引数にとり、結果をJSON型文字列として返す
    """
    def getStatus(self,args):
        try:
            return self.__getStatus_impl(args)
        except:
            import traceback
            self.logger.error(traceback.format_exc())
            ret = OrderedDict()
            ret[self.__resultKey] = UNKNOWN_ERROR
            return json.dumps(ret)

    """
        辞書型のjsonを引数にとり、結果をJSON型文字列として返す
        実体を持つimplメソッド
    """
    def __getStatus_impl(self,args):
        #返り値の初期化
        result_dict = {}

        #返り値のコード
        result = GETSTATUS_OK

        #引数判定
        target = args.get(self.__targetKey)
        if target is None:
            result = REQUEST_FORMAT_ERROR
        target = str(target)

        allCheckFlag = False
        if target == "ALL":
            allCheckFlag = True

        """
        応答メッセージ生成プロセスの確認
        """
        if target == self.__processKey or allCheckFlag:
            if self.__resMessageProcExists():
                result_dict[self.__processKey] = "1"
            else:
                result_dict[self.__processKey] = "0"

        #ロックファイルの確認
        if target == self.__reslockKey or allCheckFlag:
            if self.__reslockExists():
                result_dict[self.__reslockKey] = "1"
            else:
                result_dict[self.__reslockKey] = "0"

        if target == self.__reqlockKey or allCheckFlag:
            if self.__reqlockExists():
                result_dict[self.__reqlockKey] = "1"
            else:
                result_dict[self.__reqlockKey] = "0"

        """
        未処理メッセージ数
        """
        if target == self.__reqfilecountKey or allCheckFlag:
            result_dict[self.__reqfilecountKey] = str(len(glob.glob(os.path.join(requestFolderPath,"*.json"))))

        """
        未処理メッセージ名の取得。MSGID.jsonでreqフォルダ直下にあるので、これを昇順ソートしてLOG_COLLECT_TARGET_FILE_NUM分、csv文字列にして詰める
        """
        if target == self.__reqfilenamesKey or allCheckFlag:
            result_dict[self.__reqfilenamesKey] = self.__getjsonfiles(requestFolderPath)

        """
        応答メッセージ数
        """
        if target == self.__resfilecountKey or allCheckFlag:
            result_dict[self.__resfilecountKey] = str(len(glob.glob(os.path.join(responseRootFolderPath,"*.json"))))

        """
        応答メッセージ名の取得。MSGID.jsonでreqフォルダ直下にあるので、これを昇順ソートしてLOG_COLLECT_TARGET_FILE_NUM分、csv文字列にして詰める
        """
        if target == self.__resfilenamesKey or allCheckFlag:
            result_dict[self.__resfilenamesKey] = self.__getjsonfiles(responseRootFolderPath)

        """
        分割済み未ダウンリンクメッセージ数
        """
        if target == self.__sepfilecountKey or allCheckFlag:
            result_dict[self.__sepfilecountKey] = str(len(glob.glob(os.path.join(responseProgressFolderPath,"*_*_*.json"))))

        """
        応答メッセージ名の取得。MSGID.jsonでreqフォルダ直下にあるので、これを昇順ソートしてLOG_COLLECT_TARGET_FILE_NUM分、csv文字列にして詰める
        """
        if target == self.__sepfilenamesKey or allCheckFlag:
            result_dict[self.__sepfilenamesKey] = self.__getdividedfiles()

        result_dict[self.__resultKey] = result
        return json.dumps(result_dict)

    """
        特定のプロセスIDが存在するか確認するAPI
    """
    def __resMessageProcExists(self):
        if not os.path.exists(pidFilePath):
            return False

        with open(pidFilePath,mode="r",encoding="utf-8") as f:
            pid = f.read().rstrip('\n')

        """
        proc1 = subprocess.Popen("ps",stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        proc2 = subprocess.Popen(["grep","python"],stdin=proc1.stdout,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        proc1.stdout.close()
        out = proc2.communicate(str.encode("utf-8"))

        if pid in out:
            return True
        else:
            return False
        """
        execShell = "ps -ef | grep main_predict.py | grep -v grep | awk '{ print $2 }'"
        ret = subprocess.run(args=execShell,shell=True,stderr=subprocess.STDOUT,stdout=subprocess.PIPE)
        for line in ret.stdout.splitlines():
            if pid in line.decode(sys.stdout.encoding):
                return True
            else:
                return False

    def __reslockExists(self):
        return os.path.exists(RESPONSE_MESSAGE_LOCKFILEPATH)

    def __reqlockExists(self):
        return os.path.exists(REQUEST_SAVE_MESSAGE_LOCKFILEPATH)

    """
        targetFolderPath直下の数値.jsonファイルを昇順にソートし、LOG_COLLECT_TARGET_FILE_NUM分取得する
    """
    def __getjsonfiles(self,targetFolderPath):
        targetFolderPathobj = Path(targetFolderPath)

        # 非再帰的にフォルダ直下のjsonのパス一覧を取得
        targetFileList = targetFolderPathobj.glob("*" + JSON_EXT)

        # 拡張子除くファイル名を取得。数値.jsonを期待
        targetFileNameList = [re.search("[0-9]+",x).group() for x in map(lambda x:x.name,targetFileList)]
        # 文字列で格納されているので、数値変換しつつ昇順ソート
        targetFileNumList = sorted([int(x) for x in targetFileNameList])

        if len(targetFileNumList) == 0:
            return ""
        else:
            value = ""
            for i,targetFileNum in enumerate(targetFileNumList):
                if i >= STATUS_COLLECT_TARGET_FILE_NUM:
                    break
                elif i == STATUS_COLLECT_TARGET_FILE_NUM -1:
                    value += str(targetFileNum)
                else:
                    value += str(targetFileNum) + STATUS_COLLECT_FILENAMES_SEPARATOR
            return value[0:len(value)]

    """
    分割済みファイルを数値_数値_数値.jsonファイルを昇順にソートし、LOG_COLLECT_TARGET_FILE_NUM分取得する
    """
    def __getdividedfiles(self):
        #progressフォルダ配下の一番若いファイルのみが対象
        progressFileList = Path(responseProgressFolderPath).glob("*" + JSON_EXT)
        el = [(re.findall("[0-9]+",x)) for x in map(lambda x:os.path.basename(x.name),progressFileList)]
        # 正規表現で抽出した[レスポンスID,分割番号、分割総数]配列をintに変換して、itemgetterでソート
        sortedList = sorted(list(map(lambda x:[int(i) for i in x], el)),key=itemgetter(0,1))

        value = ""
        for i,target in enumerate(sortedList):
            if len(target) == 3:
                if i < STATUS_COLLECT_TARGET_FILE_NUM:
                    value += "{0}_{1}_{2}{3}".format(target[0],target[1],target[2],JSON_EXT)
                    if i < STATUS_COLLECT_TARGET_FILE_NUM -1:
                        value += STATUS_COLLECT_FILENAMES_SEPARATOR
        return value