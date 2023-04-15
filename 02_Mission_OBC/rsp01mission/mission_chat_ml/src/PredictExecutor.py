# coding: utf-8
"""
衛星側チャットシステム　機械学習の予測実行部
"""
from pathlib import Path
import os
import random
import re
from time import sleep
import pickle
import json
import datetime
import collections as cl
from common.Logger import Logger
from common import exec_ctrl
from common import chat_config
import dataset
from scdv import SparseCompositeDocumentVectors, load_word2vec
import cleaning


class PredictExecutor:
    """
    予測実行クラス
    """

    def __init__(self):
        """
        各種初期化処理
        Args:
            なし
        """
        self.logger = Logger.get_logger()

        # モデルなどの復元
        self.model = load_word2vec(chat_config.get_path('modelSavePath'))
        self.vec = pickle.load(open(chat_config.get_path('scdvSavePath'), 'rb'))
        self.clf = pickle.load(open(chat_config.get_path('svcSavePath'), 'rb'))

    def predict(self, text):
        """
        推論の実行
        Args:
            なし
        Return:
            応答メッセージ
        """
        #self.logger.debug("要求メッセージ text={}".format(text))

        # グッバイメッセージの場合は、goodbye分類を返す
        if text.strip() == chat_config.get("goodbyeMsg"):
            self.logger.info("グッバイメッセージが要求されました")
            goodbyeMsgClassfier = chat_config.get("goodbyeMsgClassfier")
            return self.classifier2text(goodbyeMsgClassfier), goodbyeMsgClassfier, 0

        # わかち書きを行う
        words = dataset.split_to_words(text)
        self.logger.debug("words=" + str(words))

        # テキストからSCDVを求める
        gwbowv = self.vec.make_gwbowv([words], train=False)

        # 分類
        classifier = self.clf.predict(gwbowv)
        proba = self.clf.predict_proba(gwbowv)

        # 確率順位
        score_dict = dict(zip(self.clf.classes_, proba[0]))
        sorted_score = self.value_sort(score_dict)
        # self.logger.debug("sorted_score=" + str(sorted_score))
        score = sorted_score[0][1]  # 1番目のValue
        # self.logger.debug("classifier=" + str(classifier))
        # self.logger.debug("score=" + str(score))

        # 分類から応答メッセージを生成
        res_msg = self.classifier2text(classifier[0])
        return res_msg, classifier[0], score


    def classifier2text(self, classifier):
        """
        分類から応答メッセージを生成する
        Args:
            分類
        Returns:
            応答メッセージ
        """
        message_file_path = chat_config.get_path("pathResMsgDir") + "/" + classifier + ".txt"

        # 分類名に合致するファイルからメッセージを取得し、ランダムにチョイス
        with open(message_file_path, "r") as message_file:
            lines = message_file.readlines()
            msg = random.choice(lines)

        # 置換文字列が存在する場合
        if re.search(r'#', msg):
            msg = self.res_msg_replace(msg)

        return msg.strip()

    def res_msg_replace(self, text):
        """
        応答メッセージのテンプレ内の置換文字を置換する
        Args:
            応答メッセージのテンプレ
        Returns:
            置換後の応答メッセージ
        """
        return text

    def create_first_msg(self, first_msg_file_path):
        """
        初回起動応答メッセージの生成
        初回起動なので、応答メッセージは存在しない為、応答メッセージ生成のロックは行わない
        要求メッセージのDirは参照しないので、そちらもロックなし
        Args:
            初回起動メッセージのテンプレファイルのパス
        Returns:
            なし
        """
        if not first_msg_file_path.exists():
            return

        self.logger.info("初回起動応答メッセージを応答します")
        with open(str(first_msg_file_path), "r") as template_file:
            res = json.load(template_file)

        # 応答日時を設定
        dt_now = datetime.datetime.now()
        res["res_gen_datetime"] = dt_now.strftime("%Y-%m-%d %H:%M:%S")

        # 応答メッセージファイルの生成
        res_file_path = chat_config.get_path("pathResMsg") + "/" + str(res["msg_id"]) + ".json"
        with open(res_file_path, "w") as res_file:
            json.dump(res, res_file, ensure_ascii=False)
            self.logger.info("初回起動応答メッセージを保存しました")

        # テンプレファイルの削除を行う
        if chat_config.get('isDelReq'):
            first_msg_file_path.unlink()
            self.logger.info("初回起動メッセージのテンプレファイルを削除しました path={}".format(first_msg_file_path))

    def exec(self):
        """
        応答メッセージ生成のメイン
        Args:
            なし
        Returns:
            なし
        """
        self.logger.info("予測実行部を開始します")

        # 初回起動メッセージ
        first_msg_file_path = Path(chat_config.get_path("pathFirstResMsg"))
        if first_msg_file_path.exists():
            self.create_first_msg(first_msg_file_path)

        # ここから永久ループ
        while True:

            # 停止要求が行われている場合は処理を終了する
            if exec_ctrl.isStop():
                self.logger.info("停止が要求されたため、予測実行処理を終了します")
                return

            self.logger.debug("予測実行部を開始します")

            # 指定時間待機
            interval = chat_config.get("resMsgProcLoopInterval")
            self.logger.debug("{}秒待機します。".format(interval))
            sleep(interval)

            # 未処理ロックファイルロックファイルが存在したら処理なし
            reqLockFilePath = Path(chat_config.get_path("pathReqLockFile"))
            if reqLockFilePath.exists():
                self.logger.debug("未処理ロックファイルが存在した為、スキップします")
                continue

            # 排他を行うため、応答メッセージロックファイルを生成する
            resLockFilePath = Path(chat_config.get_path("pathResLockFile"))
            resLockFilePath.open("w")

            # 要求メッセージ未処理ファイルを基に応答メッセージファイルを生成する
            req_msgs = self.get_req_msg_list()
            for req_msg_file_path in req_msgs:
                self.logger.debug(
                    "要求メッセージ未処理ファイル={}".format(req_msg_file_path))

                req, res_msg = self.exec_predict(req_msg_file_path)

                # 応答メッセージデータを生成し、ファイルに出力する。
                res = self.createResMsg(req, res_msg)
                self.write_res(req, res)

                # 要求メッセージ未処理ファイルの削除を行う
                if chat_config.get('isDelReq'):
                    Path(req_msg_file_path).unlink()
                    self.logger.debug(
                        "要求メッセージ未処理ファイルを削除しました path={}".format(req_msg_file_path))
                else:
                    self.logger.debug(
                        "設定値'isDelReq'がFlaseの為、要求メッセージ未処理ファイルは削除しませんでした path={}".format(req_msg_file_path))

            # 排他を解除するため、応答メッセージロックファイルを削除する（一応存在確認を行う）
            if resLockFilePath.exists():
                resLockFilePath.unlink()

    def createResMsg(self, req, res_msg):
        """
        応答メッセージデータの生成を行う
        Args:
            req :要求メッセージのdict
            res_msg :応答メッセージ
        Returns:
            応答メッセージのdict
        """
        res = cl.OrderedDict()
        res["msg_id"] = req["msg_id"]
        res["recv_datetime"] = req["recv_datetime"]
        res["msg"] = res_msg
        dt_now = datetime.datetime.now()
        res["res_gen_datetime"] = dt_now.strftime("%Y-%m-%d %H:%M:%S")

        return res

    def write_res(self, req, res):
        """
        応答メッセージファイルを出力する
        Args:
            req :要求メッセージのdict
            res :応答メッセージ
        Returns:
            なし
        """

        # 応答メッセージファイル名の生成
        res_file_path = chat_config.get_path("pathResMsg") + "/" + str(
            req["msg_id"]) + ".json"
        with open(res_file_path, "w") as res_file:
            json.dump(res, res_file, ensure_ascii=False)

    def exec_predict(self, req_msg_file_path):
        """
        予測処理の実行を行う。
        Args:
            req_msg_file_path :未処理要求メッセージファイルのパス
        Returns:
            未処理要求メッセージ
            予測結果
        """
        with open(req_msg_file_path, "r") as req_file:
            req = json.load(req_file)
            self.logger.debug("要求メッセージ={}".format(req["msg"]))

            # テキストクリーニング
            text = cleaning.remove_noise(req["msg"])

            # 分類の実行
            res_msg, classifier, score = self.predict(text)
            self.logger.debug("分類={}, 応答={}, スコア={}".format(classifier, res_msg, score))

            return req, res_msg

    def get_req_msg_list(self):
        """
        未処理要求メッセージファイルをリストで返す
        Args:
            なし
        Returns:
            未処理要求メッセージファイルパスのリスト
        """

        # パスのリストを文字列化して返す
        path = Path(chat_config.get_path("pathReqMsg"))
        return [str(req_path) for req_path in list(path.glob("*.json"))]

    def value_sort(self, dic):
        return sorted(dic.items(), key=lambda x: x[1], reverse=True)
