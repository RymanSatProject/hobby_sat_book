# -*- coding: utf-8 -*-

import bz2,base64,json,shutil
from collections import OrderedDict

from mission_chat.src.common.Logger import Logger
from mission_chat.src.Constants import *
from mission_chat.src.ErrorCode import *
from mission_chat.src.Util import *
from enum import IntEnum
from pathlib import Path
from operator import itemgetter
import re

class ChatResponseHandler:
    command_name = "GetMessageResponse"

    # 入力jsonデータフォーマット
    __requestTypeKey = "req_type"
    __responseIdKey = "res_id"
    __msgIdKey = "msg_id"
    __seqNumberKey = "sep_seq_no"

    # 出力jsonデータフォーマット
    __resultKey = "result"
    __seqTotalKey = "seq_total"
    __dataKey = "comp_data"

    def __init__(self):
        self.logger = Logger.get_logger()
        self.logger.info(self.__class__.__name__+ " initialize.")

        # 要求メッセージ格納用フォルダ作成
        os.makedirs(responseRootFolderPath, exist_ok=True)
        os.makedirs(responseProgressFolderPath,exist_ok=True)
        os.makedirs(responseDoneCompressedFolderPath,exist_ok=True)
        os.makedirs(responseDoneOriginalFolderPath,exist_ok=True)

    """
        引数のargsはloadされた辞書型を期待する。
        返り値はdumpsしたJSON形式の文字列を返す。
    """
    def getResponseMessage(self,args):
        try:
            return self.__getResponseMessage_impl(args)
        except:
            import traceback
            self.logger.error(traceback.format_exc())
            ret = OrderedDict()
            ret[self.__resultKey] = UNKNOWN_ERROR
            return json.dumps(ret)

    """
        引数のargsはloadされた辞書型を期待する。
        返り値はdumpsしたJSON形式の文字列を返す。
    """
    def __getResponseMessage_impl(self,args):
        #ロックされているかを確認
        if self.__isLocked():
            self.logger.warn(self.command_name+ ": lock file exists.")
            return self.__makeResponseData_jsonStr(RESPONSE_LOCKED_WARNING)

        #引数フォーマットがおかしければすぐに返す。
        if not self.__validationArgs(args):
            self.logger.error(self.command_name+": request format is not expected.")
            return self.__makeResponseData_jsonStr(REQUEST_FORMAT_ERROR)

        #要求タイプごとに処理を変える。
        reqType = convertStringToInt(args.get(self.__requestTypeKey))
        resId = convertStringToInt(args.get(self.__responseIdKey))
        msgId = convertStringToInt(args.get(self.__msgIdKey))
        seqNumber = convertStringToInt(args.get(self.__seqNumberKey))

        #req = 0なら、
        self.logger.info(self.command_name + ": process request type:{0}".format(reqType))
        if reqType == ReqType.SEQ:
            return self.__procSEQ()
        elif reqType == ReqType.MSGID:
            #reqType = 1なら
            return self.__procMSGID(msgId)
        elif reqType == ReqType.RESID:
            #reqType = 2なら
            return self.__procRESID(resId, seqNumber)

    """
        ロックファイルが存在するか返す関数    
        Trueならロックファイルが存在
    """
    def __isLocked(self):
        lockfileList = list(Path(responseRootFolderPath).glob(RESPONSE_MESSAGE_LOCKFILE))
        if len(lockfileList) == 1:
            return True
        return False

    """
        引数dataがDOWNLINK_MAX_DATA_SIZE_BYTEサイズ以上であれば、分割してファイル保存する。
        ファイルは、progressフォルダ直下にレスポンスID_分割番号_分割総数.jsonで保存される。
        dataは圧縮、BASE64エンコード済みであり、マルチバイト文字を含まない。
    """
    def __divideResponseData(self,data):
        dataBytes = len(data.encode("UTF-8"))
        #割って切り捨て + 1個が分割する最大値
        totalSeparateNum = int(dataBytes / DOWNLINK_MAX_DATA_SIZE_BYTE) + 1

        #全てprogressフォルダ配下にファイル生成
        for separateNum in range(totalSeparateNum):
            #ファイル名の分割番号は１にしたいのでファイル名にするときのみインクリメント
            if separateNum >= 1:
                #分割番号が1以上(=分割されてる)のときはインクリメントしない
                inclResID = False
            else:
                inclResID = True
            tmpJsonDict = self.__makeResponseData(ret_code=RESPONSE_MESSAGE_OK,comp_data=data[separateNum*DOWNLINK_MAX_DATA_SIZE_BYTE : (separateNum+1)*DOWNLINK_MAX_DATA_SIZE_BYTE],sep_seq_no=separateNum + 1,seq_total=totalSeparateNum,inclResID=inclResID)

            #ファイルフォーマットは、レスポンスID_分割番号_分割総数.json
            filePath = os.path.join(responseProgressFolderPath,"{0}_{1}_{2}{3}".format(self.__getResponseId(),separateNum +1,totalSeparateNum,JSON_EXT))

            #ファイル書き込み
            with open(filePath,"w",encoding="utf-8") as f:
                json.dump(tmpJsonDict,f)

    """
        Inprogressフォルダ配下のレスポンスIDと分割番号が若いものをJSON文字列形式で返す。
        この時、doneフォルダに移動させる。
    """
    def __getResponseMessageFileData(self):
        #progressフォルダ配下の一番若いファイルのみが対象
        progressFileList = Path(responseProgressFolderPath).glob("*" + JSON_EXT)
        el = [(re.findall("[0-9]+",x)) for x in map(lambda x:os.path.basename(x.name),progressFileList)]
        # 正規表現で抽出した[レスポンスID,分割番号、分割総数]配列をintに変換して、itemgetterでソート
        sortedList = sorted(list(map(lambda x:[int(i) for i in x], el)),key=itemgetter(0,1))

        targetFilePath = os.path.join(responseProgressFolderPath,"{0}_{1}_{2}{3}".format(sortedList[0][0],sortedList[0][1],sortedList[0][2],JSON_EXT))

        with open(targetFilePath,"r",encoding="utf-8") as f:
            ret = json.dumps(json.load(f))

        #リターン直前にファイルをDONEフォルダに移動
        shutil.move(targetFilePath,os.path.join(responseDoneCompressedFolderPath))
        return ret

    """
        リターン用のjsonを生成し、JSON文字列として返す。
        :param inclResID レスポンスIDをインクリメントするかどうかの制御引数。Trueならインクリメントして使用し、Falseならそのまま使用する
    """
    def __makeResponseData_jsonStr(self,ret_code="000",comp_data="",sep_seq_no=1,seq_total=1,inclResID=True):
        ret_dict = self.__makeResponseData(ret_code=ret_code,comp_data=comp_data,sep_seq_no=sep_seq_no,seq_total=seq_total,inclResID=inclResID)
        return json.dumps(ret_dict)

    """
        リターン用のjsonを生成し、辞書型として返す。
        :param inclResID レスポンスIDをインクリメントするかどうかの制御引数。Trueならインクリメントして使用し、Falseならそのまま使用する
    """
    def __makeResponseData(self,ret_code="000",comp_data="",sep_seq_no=1,seq_total=1,inclResID=True):
        data = {}
        data[self.__resultKey] = ret_code
        data[self.__seqNumberKey] = str(sep_seq_no)
        data[self.__seqTotalKey] = str(seq_total)
        data[self.__dataKey] = comp_data

        with open(responseIdFilePath,"r+",encoding="utf-8") as f:
            res_id  = int(f.read().strip())
            #必要に応じてインクリメント
            if inclResID:
                res_id += 1

                #ファイルを先頭まで戻して、書き込んだ数値のみでtruncateする
                f.seek(0)
                f.write(str(res_id))
                f.truncate()
                self.logger.info("response id is inclemented. res-id:{0}".format(res_id))
        data[self.__responseIdKey] = str(res_id)

        return data

    """
    レスポンスIDを取得して返す。
    返す際の方は整数型とする。
    """
    def __getResponseId(self):
        with open(responseIdFilePath,"r",encoding="utf-8") as f:
            res_id = int(f.read().strip())
            return res_id

    """
        resフォルダ直下にあるmsgID.jsonを若い順に最大COMPRESSED_TARGET_FILE_NUM分ファイル圧縮、エンコードする。
        返り値は、読み込んだファイルコンテンツを連結させた文字列オブジェクト。
        読み込んだファイルはDONEフォルダに移動させる。
    """
    def __compressSomeResponseData(self):
        responseFolderPath = Path(responseRootFolderPath)

        # jsonのパス一覧を取得
        messageFileList = responseFolderPath.glob("*" + JSON_EXT)
        # 拡張子除くファイル名を取得
        messageFileNameList = [re.search("[0-9]+",x).group() for x in map(lambda x:x.name,messageFileList)]
        # 文字列で格納されているので、数値変換しつつ昇順ソート
        messageFileNumList = sorted([int(x) for x in messageFileNameList])

        tmpData = ""

        #ファイルを番号が若い順から読み込んで連結する。
        for i,messagefilenum in enumerate(messageFileNumList):
            if i >= COMPRESSED_TARGET_FILE_NUM:
                break

            messageFilepath = os.path.join(responseRootFolderPath,"{0}{1}".format(messagefilenum,JSON_EXT))
            with open(messageFilepath,"r",encoding="utf-8") as f:
                tmpData += f.read()
            # shutil.moveではoverwriteしないため、readが終了後に、DONEフォルダにコピーしてoriginalデータを削除
            shutil.copy(messageFilepath,responseDoneOriginalFolderPath)
            os.remove(messageFilepath)

        data = self.__makeCompressedData(tmpData)
        return data

    """
        未ダウンリンクメッセージが存在しないなら、responseフォルダ直下にあるmsgID.jsonを圧縮、分割して一番分割番号が若いJSON文字列形式を返す。
        未ダウンリンクメッセージが残っているのなら、そのJSON文字列形式を返す。
    """
    def __procSEQ(self):
        if len(list(Path(responseProgressFolderPath).glob("*" + JSON_EXT))) != 0:
            # 未送信のデータがあれば、それを送信。
            self.logger.info(self.command_name + ": sends already compressed but unsent message.")
            return self.__getResponseMessageFileData()
        elif len(list(Path(responseRootFolderPath).glob("*" + JSON_EXT))) == 0:
            # 未送信データがない、かつそもそも処理済み応答メッセージがない場合、エラーコードを返す。
            self.logger.warn(self.command_name + ": there is no response message.")
            return self.__makeResponseData_jsonStr(RESPONSE_FILE_NOTEXIST_WARNING)
        else:
            # 未送信データがなければ、resフォルダ直下のjsonをまとめて圧縮
            self.logger.info(self.command_name + ": compress and send messages.")
            compressedData = self.__compressSomeResponseData()
            self.__divideResponseData(compressedData)
            return self.__getResponseMessageFileData()

    """
        指定されたmsgidのJSON文字列形式を返す。
        もし、msgidのファイルが未ダウンリンクであれば、ダウンリンク済みフォルダに移動させる。
    """
    def __procMSGID(self,msgId):
        #再帰的にmsgid.jsonのファイルを走査
        messageFileList = list(Path(responseRootFolderPath).glob("**/{0}{1}".format(msgId,JSON_EXT)))
        if len(messageFileList) == 1:
            self.logger.info(self.command_name+ ": there is the specified msg id file. msgid:".format(msgId))
            #msgIDに該当するファイルを読み込む。
            messageFilePath = messageFileList[0]
            with open(str(messageFilePath), "r",encoding="utf-8") as f:
                tmpdata = f.read()

            #未ダウンリンクのファイルが指定された場合は、ダウンリンク済みフォルダに移動させる
            messageFileAbsPath = messageFilePath.resolve()
            if not DONE_FOLDER_NAME in str(messageFileAbsPath):
                shutil.move(str(messageFilePath),responseDoneOriginalFolderPath)

            #それを圧縮・エンコードして返す。
            compressedData = self.__makeCompressedData(tmpdata)
            #必要なら分割して返す
            self.__divideResponseData(compressedData)
            return self.__getResponseMessageFileData()

        elif len(messageFileList) > 1:
            #指定したMSGIDに一致するファイルが複数ある場合、
            self.logger.error(self.command_name+ ": there are same msgid files. msgid:{0}".format(msgId))
            return self.__makeResponseData_jsonStr(SPECIFIED_MSGID_MULTIPLE_ERROR)
        else:
            #指定したMSGIDに一致するファイルが１つもない場合
            self.logger.warn(self.command_name+ ": there is no specified msgid file. msgid:{0}".format(msgId))
            return self.__makeResponseData_jsonStr(SPECIFIED_MSGID_NOTEXIST_WARNING)

    """
    指定されたレスポンスIDと分割番号のメッセージファイルをJSON文字列形式で返す。
    ダウンリンク済みか未ダウンリンクかは問わない。
    もし未ダウンリンクであれば、ファイルをDONEフォルダに移動させる。
    """
    def __procRESID(self, resId, sepNo):
        #再帰的にresid_sepNo_*.jsonのファイルを走査
        dividedMessageFileList = list(Path(responseRootFolderPath).glob("**/{0}_{1}_*{2}".format(resId,sepNo,JSON_EXT)))
        if len(dividedMessageFileList) == 1:
            self.logger.info(self.command_name+ ": there is the specified resid and sepno file. resid:{0},sepno{1}".format(resId,sepNo))
            #msgIDに該当するファイルを読み込む。
            dividedMessageFilePath = dividedMessageFileList[0]
            with open(str(dividedMessageFilePath), "r",encoding="utf-8") as f:
                jsonStr = json.dumps(json.load(f))

            # 未ダウンリンクのファイルが指定された場合は、ダウンリンク済みフォルダに移動させる
            messageFileAbsPath = dividedMessageFilePath.resolve()
            if not DONE_FOLDER_NAME in str(messageFileAbsPath):
                shutil.move(str(dividedMessageFilePath),responseDoneCompressedFolderPath)

            return jsonStr

        elif len(dividedMessageFileList) > 1:
            #指定したREQIDに一致するものが複数ある場合
            self.logger.error(self.command_name+ ": there are same resid and sepno files. resid:{0},sepno{1}".format(resId,sepNo))
            return self.__makeResponseData_jsonStr(SPECIFIED_RESID_SEPNO_MULTIPLE_ERROR)
        else:
            #指定したREQIDに一致するものが１つもない場合
            self.logger.warn(self.command_name+ ": there is no specified resid and sepno files. resid:{0},sepno{1}".format(resId,sepNo))
            return self.__makeResponseData_jsonStr(SPECIFIED_RESID_SEPNO_NOTEXIST_WARNING)

    """
    コマンドから渡された引数の妥当性を確認する。
    返り値がFalseなら、データフォーマットが不正である。
    
    """
    def __validationArgs(self,args):
        reqType = convertStringToInt(args.get(self.__requestTypeKey))
        resId = convertStringToInt(args.get(self.__responseIdKey))
        msgId = convertStringToInt(args.get(self.__msgIdKey))
        seqNumber = convertStringToInt(args.get(self.__seqNumberKey))
        self.logger.info(self.command_name + ": parameters: reqType:{0}, resId:{1},msgId:{2}, seqNumber:{3}".format(reqType,resId,msgId,seqNumber))

        if reqType == ReqType.SEQ:
            return True
        elif reqType == ReqType.MSGID:
            if msgId is None:
                return False
            else:
                return True
        elif reqType == ReqType.RESID:
            if resId is not None and seqNumber is not None:
                return True
            else:
                return False
        else:
            return False

    """
        データ圧縮用関数
        文字列で受け取ったデータをbzip圧縮->base64encodeして文字列化して返す
    """
    def __makeCompressedData(self, data):
        tmp = bz2.compress(data.encode("utf-8"))
        ret = base64.urlsafe_b64encode(tmp).decode("utf-8")
        return ret

    """
        デバッグ用関数
    """
    def __updateDOWNLINK_MAX_DATA_SIZE_BYTE(self,size):
        global DOWNLINK_MAX_DATA_SIZE_BYTE
        DOWNLINK_MAX_DATA_SIZE_BYTE = size
        print("test")

##要求タイプを区別するためのenumクラス
class ReqType(IntEnum):
    #順次送信
    SEQ = 0
    #メッセージID指定送信
    MSGID = 1
    #レスポンスID、分割番号指定送信
    RESID = 2