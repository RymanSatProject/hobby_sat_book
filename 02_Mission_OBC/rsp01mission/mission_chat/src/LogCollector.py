# -*- coding: utf-8 -*-

import json,datetime

from collections import OrderedDict

from mission_chat.src.common.Logger import Logger
from mission_chat.src.Constants import *
from mission_chat.src.ErrorCode import *
from mission_chat.src.Util import *
from pathlib import Path

"""
ログコレクタ
"""
class LogCollector:
    command_name = "CollectLog"

    __targetKey = "target"
    __datetimeKey = "datetime"
    __keywordKey = "keyword"
    __rowKey = "row"
    __resultKey = "result"
    __logKey = "log"

    __targetChat = "chat"
    __targetChatML = "ml"

    def __init__(self):
        self.logger = Logger.get_logger()

    """
        辞書型のjsonを引数にとり、結果をJSON型文字列として返す
    """
    def getlogs(self,args):
        try:
            return self.__getlogs_impl(args)
        except:
            import traceback
            self.logger.error(traceback.format_exc())
            ret = OrderedDict()
            ret[self.__resultKey] = UNKNOWN_ERROR
            return json.dumps(ret)

    """
        辞書型jsonを受け取り、JSON型文字列を返す。
        実体を持つimplメソッド
    """
    def __getlogs_impl(self,args):
        self.logger.info(self.command_name+ ": start collecting log information.")
        #返り値の初期化
        result_dict = {}

        #返り値のコード
        result = GETLOG_OK

        #引数チェック
        if not self.__validationArgs(args):
            result = REQUEST_FORMAT_ERROR
            result_dict[self.__resultKey] = result
            self.logger.error(self.command_name+ ": end collecting log information. return:{0}".format(result))
            return json.dumps(result_dict)

        #ログの取得
        target = convertStringToInt(args.get(self.__targetKey))
        _datetime = convertStringToInt(args.get(self.__datetimeKey))
        keyword = convertStringToInt(args.get(self.__keywordKey))
        row = convertStringToInt(args.get(self.__rowKey))

        result,logs = self.__getLogInfo(target=target,_datetime=_datetime,keyword=keyword,row=row)
        result_dict[self.__resultKey] = result
        result_dict[self.__logKey] = logs
        self.logger.info(self.command_name+ ": end collecting log information. return:{0}".format(result))
        return json.dumps(result_dict)

    """
    ログの取得関数
    """
    def __getLogInfo(self,target,_datetime,keyword,row):
        ret = GETLOG_OK
        ret_logstr = ""

        targetLogList = []
        self.logger.info(self.command_name + ": collecting target is {0}".format(target))
        if target == self.__targetChat:
            #datetimeよりターゲットログファイルを選定
            targetLogList.extend(self.__searchLogFiles(chatLogFolderPath,_datetime,chatLogFileName))
        elif target == self.__targetChatML:
            #datetimeよりターゲットログファイルを選定
            targetLogList.extend(self.__searchLogFiles(chatMLLogFolderPath,_datetime,chatMLLogFileName))
        else:
            #chat,ml以外ならログファイルがフルパス指定
            if os.path.exists(target):
                targetLogList.append(Path(target))
            else:
                ret = GETLOG_NO_SPECIFIED_LOGFILE
                return ret,targetLogList

        #リスト内(昇順ソート済み)のファイルからdatetime,keyword,rowを利用してファイル読み込み
        #リストの先頭にローテートしてないベースファイルがあるので移動。
        targetLogList.append(targetLogList[0])
        targetLogList.pop(0)
        startFlag = False
        endFlag = False
        readCounter = 0

        #日付オブジェクトの生成
        specifiedDate = _datetime.strip()
        dateSpecified = datetime.datetime.strptime(specifiedDate, '%Y-%m-%d %H:%M:%S')

        self.logger.debug(self.command_name + ": target log file list is {0}".format(targetLogList))
        for logfile in targetLogList:
            with open(str(logfile),"r",encoding="utf-8") as f:
                for line in f:

                    dateLineStr = line[line.find("[") + 1:line.find("]")]
                    try:
                        dateLine = datetime.datetime.strptime(dateLineStr,'%Y/%m/%d %H:%M:%S')
                    except ValueError:
                        # 行にdate部が存在しない場合スタックトレース。StartFlagが立っているなら読み込みを行う。
                        pass
                    else:
                        # ログの読み始めは必ず例外でなく通常のログになる。
                        # キーワードが見つかった時点でreturn文字列に詰め込む
                        if dateLine >= dateSpecified and keyword in line:
                            startFlag = True
                    finally:
                        if startFlag:
                            ret_logstr += line
                            readCounter += 1

                        if int(row) == readCounter:
                            endFlag = True
                            break
            if endFlag:
                break

        #処理開始フラグが立っていない場合、キーワードか日付がおかしい
        if not startFlag:
            ret = GETLOG_NO_LOG_KEYWORD_OR_DATETIME

        return ret,ret_logstr

    """
        datetimeよりkeywordを見つけるログファイルを選定
        リターンは、Pathオブジェクトのリスト
    """
    def __searchLogFiles(self,logFolderPath,_datetime,baselogfilename):
        targetLogList = []
        for logfilePath in sorted(Path(logFolderPath).glob(baselogfilename+"*")):
            logFileName = logfilePath.name
            if logFileName == baselogfilename:
                targetLogList.append(logfilePath)
            else:
                #ログファイル名よりローテートの日付を取得
                rotateDateStr = logFileName.lstrip(baselogfilename)
                date_rotate = datetime.datetime.strptime(rotateDateStr,'%Y-%m-%d')
                #インプットよりログの日付を取得
                specifiedDate = _datetime.strip().split(" ")
                date_specified = datetime.datetime.strptime(specifiedDate[0],'%Y-%m-%d')

                #ローテートした日付が指定日よりあたらいし場合、選定対象のログが含まれる
                if date_rotate >= date_specified:
                    targetLogList.append(logfilePath)
        return  targetLogList

    """
    コマンドから渡された引数の妥当性を確認する。
    返り値がFalseなら、データフォーマットが不正である。
    """
    def __validationArgs(self, args):
        target = convertStringToInt(args.get(self.__targetKey))
        _datetime = convertStringToInt(args.get(self.__datetimeKey))
        keyword = convertStringToInt(args.get(self.__keywordKey))
        row = convertStringToInt(args.get(self.__rowKey))

        self.logger.info(self.command_name+ ": Validate command Input:")
        self.logger.info(self.command_name+ ": target:{0} datetime:{1} keyword:{2} row:{3}".format(target,_datetime,keyword,row))

        if target is None:
            return False

        #target以外の引数は全てないといけない。
        if _datetime is not None and keyword is not None and row is not None:
            #日付型が適切な型か。
            try:
                datetime.datetime.strptime(_datetime,"%Y-%m-%d %H:%M:%S")
            except ValueError:
                return False
            #rowが数値か
            if type(row) is str:
                if not str.isdecimal(row):
                    return False
            return True
        else:
            return False