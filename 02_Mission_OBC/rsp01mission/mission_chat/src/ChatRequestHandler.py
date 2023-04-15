# -*- coding: utf-8 -*-

import json, base64, bz2, shutil
from collections import OrderedDict

from mission_chat.src.common.Logger import Logger
from datetime import datetime
from pathlib import Path
from mission_chat.src.ErrorCode import *
from mission_chat.src.Constants import *
from mission_chat.src.Util import *

# チャット処理のリクエストハンドラー
class ChatRequestHandler:
    command_name = "SaveChatMessageRequest"

    # jsonデータフォーマット
    __reqIdKey = "req_id"
    __seqNumberKey = "sep_seq_no"
    __seqTotalKey = "seq_total"
    __dataKey = "comp_data"
    __msgIdKey = "msg_id"

    # 未処理メッセージファイル用json key
    __timestampKey = "recv_datetime"

    # .tmp
    __tmpExtension = ".tmp"

    # lockFileName
    __lockfilepath = os.path.join(requestFolderPath,REQUEST_SAVE_MESSAGE_LOCKFILE)

    def __init__(self):
        self.logger = Logger.get_logger()

        self.logger.info(self.__class__.__name__+ " initialize.")

        # 要求メッセージ格納用フォルダ作成
        os.makedirs(requestFolderPath, exist_ok=True)

    """
        要求メッセージ保存コマンドの実体
        引数として辞書型のJSON配列を引き受け、返り値としてJSON文字列を返す。
    """
    def saveRequestMessage(self,args):
        try:
            return self.__saveRequestMessage_impl(args)
        except:
            import traceback
            self.logger.error(traceback.format_exc())
            ret = OrderedDict()
            ret[self.__resultKey] = UNKNOWN_ERROR
            return json.dumps(ret)

    """
        要求メッセージ保存コマンドの実体
        引数として辞書型のJSON配列を引き受け、返り値としてJSON文字列を返す。
        実体を持つメソッド
    """
    def __saveRequestMessage_impl(self,args):
        self.logger.info(self.command_name + " onExecute is called")
        self.logger.debug("args: " + str(args))

        # 不正状態により未処理ロックファイルがあるかを確認
        if os.path.exists(self.__lockfilepath):
            self.logger.info(self.command_name + " return "+ SAVE_MESSAGE_LOCKED_ERROR)
            return self.__MakeReturnCode(SAVE_MESSAGE_LOCKED_ERROR)

        # 未処理ロックファイル生成
        with open(self.__lockfilepath, "w"):
            self.logger.info(self.command_name + " create lockfile.")
            pass

        if not self.__ValidateArgs(args):
            # 未処理ロックファイル削除
            os.remove(self.__lockfilepath)
            self.logger.info(self.command_name + " remove lock file:"+ self.__lockfilepath)
            self.logger.info(self.command_name + " return "+ REQUEST_FORMAT_ERROR)
            return self.__MakeReturnCode(REQUEST_FORMAT_ERROR)

        ret = SAVE_MESSAGE_OK

        if args:
            reqId = convertStringToInt(args.get(self.__reqIdKey))
            seqNumber = convertStringToInt(args.get(self.__seqNumberKey))
            seqTotal = convertStringToInt(args.get(self.__seqTotalKey))
            data = convertStringToInt(args.get(self.__dataKey))

            self.logger.debug(self.command_name + "json parse result %s:%s %s:%s %s:%s" %
                              (self.__reqIdKey, reqId,
                               self.__seqNumberKey, seqNumber,
                               self.__seqTotalKey, seqTotal))

            # Total数が1な場合、分割されてないメッセージ
            if seqTotal == 1:
                self.logger.info(self.command_name + "Undivided message comes.")

                # 要求メッセージの解凍
                rawData_list = self.__ExpansionData(data)

                # 未処理要求メッセージファイル保存
                for e in rawData_list:
                    self.__SaveMessageFile(e)
                self.logger.info(self.command_name + "Save message data")

            elif seqTotal != 1:
                # 分割された最後のメッセージが来たら
                self.logger.info(
                    self.command_name + "Divided message comes. reqId:%s sep_seq_no:%s seqTotal:%s" % (reqId, seqNumber, seqTotal))

                if seqTotal == seqNumber:
                    # ファイルが全てあることを確認
                    # 結合
                    # 分割メッセージが欠損している場合は、一時保存ファイルを削除して終了
                    self.__SaveDividedMessage(reqId, seqNumber, data)
                    data = self.__CombineDividedMessage(reqId, seqTotal)

                    # デコード処理、解凍実行
                    # TODO:tar展開するならexpansionData内部で。
                    rawData_list = self.__ExpansionData(data)

                    # jsonファイルに実保存する
                    for e in rawData_list:
                        self.__SaveMessageFile(e)

                    self.logger.info(self.command_name + "Combine and save message data")
                else:
                    # 一時ファイルの作成のみ。requestID/requesdID_seqNumberのファイル名
                    self.__SaveDividedMessage(reqId, seqNumber, data)
                    self.logger.info(self.command_name + "Save divided message data")

        # 未処理ロックファイル削除
        os.remove(self.__lockfilepath)
        self.logger.info(self.command_name + " remove lock file:" + self.__lockfilepath)
        self.logger.info(self.command_name + " return " + SAVE_MESSAGE_OK)
        return self.__MakeReturnCode(SAVE_MESSAGE_OK)

    """
        返り値用のjson生成メソッド
    """
    def __MakeReturnCode(self, code):
        ret = {"result": code}
        return json.dumps(ret)

    """
        分割メッセージを一時的に保存する
        データ格納領域に、"data/requestID/sep_seq_no.tmp"のファイルを作成し保存する
    """
    def __SaveDividedMessage(self, reqId, sep_seq_no, comp_data):
        requestIdFolder = os.path.join(requestFolderPath, str(reqId))
        os.makedirs(requestIdFolder, exist_ok=True)

        tmpFileName = str(sep_seq_no) + self.__tmpExtension
        tmpFilePath = os.path.join(requestIdFolder, tmpFileName)
        tmpFile = open(tmpFilePath, "w")
        tmpFile.write(comp_data)
        tmpFile.close()

    """
        分割メッセージをまとめて要求メッセージを返す
        保存後に__SaveDividedMessageで一時作成したフォルダを削除する
    """
    def __CombineDividedMessage(self, reqId, seqTotal):
        requestIdFolder = os.path.join(requestFolderPath, str(reqId))

        data = ""
        tmpFilePath = ""
        for i in range(seqTotal):
            tmpFilePath = os.path.join(requestIdFolder, str(i + 1) + self.__tmpExtension)
            with open(tmpFilePath, "r") as f:
                data += f.read().strip()

        # 一時フォルダの削除
        if tmpFilePath != "":
            shutil.rmtree(str(Path(tmpFilePath).parent))
        return data

    """
    comp_dataの展開。引数のargsは文字列、返り値は文字列にする。
    内部的には、文字列をバイナリ化あとにdecodeする。
    全てutf-8として処理
    Base64 decode -> bzip2解凍
    """
    def __ExpansionData(self, args):
        mode = 0

        comp_data = args.encode("utf-8")
        comp_data = base64.urlsafe_b64decode(comp_data + b'=' * (-len(comp_data) % 4))
        ret = bz2.decompress(comp_data).decode("utf-8")

        ret_array = []
        start_index = 0
        if mode == 0:
            # 非tar構造のファイルとして処理
            left_parenthesis_count = 0
            right_parenthesis_count = 0

            for i in range(0,len(ret)):
                c = ret[i]
                if c == "{":
                    left_parenthesis_count += 1
                    pass
                if c == "}":
                    right_parenthesis_count += 1

                if left_parenthesis_count != 0 and left_parenthesis_count == right_parenthesis_count:
                    ret_array.append(ret[start_index:i + 1])
                    start_index = i + 1

        elif mode == 1:
            # tar構造のファイルとして処理
            pass

        return ret_array

    """
        引数指定した文字列からフォルダ作成を行う
    """
    def __MakeDirs(self, args):
        directory = args
        dataFolderPath = os.path.dirname(directory)
        if not os.path.exists(dataFolderPath):
            print("make directory:", dataFolderPath)
            os.makedirs(dataFolderPath)

    """
        未処理メッセージファイル生成メソッド
        json型の文字列を受け取って、メッセージIDをファイル名とするjsonファイルを作成する
    """
    def __SaveMessageFile(self, args):
        json_str = args
        messageData = json.loads(json_str)

        # タイムスタンプの追加
        messageData[self.__timestampKey] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        # msgIdより未処理メッセージファイル生成

        msgId = messageData[self.__msgIdKey]
        messageFilePath = os.path.join(requestFolderPath,str(msgId)+JSON_EXT)
        json_data =json.dumps(messageData,ensure_ascii=False)
        with open(messageFilePath, "w",encoding="utf-8") as f:
            f.write(json_data)
        self.logger.info(self.command_name + " save message file:" + messageFilePath)

    """
        引数のjsonデータの妥当性確認メソッド
        引数に不正があれば、Falseを返す。
    """
    def __ValidateArgs(self, args):
        reqId = convertStringToInt(args.get(self.__reqIdKey))
        seqNumber = convertStringToInt(args.get(self.__seqNumberKey))
        seqTotal = convertStringToInt(args.get(self.__seqTotalKey))
        data = convertStringToInt(args.get(self.__dataKey))

        if reqId is None:
            self.logger.info(self.command_name + " validate NG. reqId is inappropriate. reqId: " + str(reqId))
            return False

        if seqNumber is None or seqNumber < 1:
            self.logger.info(self.command_name + " validate NG. seqNumber is inappropriate. seqNumber: " + str(seqNumber))
            return False

        if seqTotal is None or seqTotal < 1:
            self.logger.info(self.command_name + " validate NG. seqTotal is inappropriate. seqTotal: " + str(seqTotal))
            return False

        if data is None:
            self.logger.info(self.command_name + " validate NG. data is inappropriate. data: " + str(data))
            return False

        self.logger.debug(self.command_name + " onExecute is called")
        return True