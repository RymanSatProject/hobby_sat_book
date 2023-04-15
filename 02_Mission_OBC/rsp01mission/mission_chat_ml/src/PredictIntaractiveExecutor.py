# coding: utf-8
"""
衛星側チャットシステム　機械学習の予測実行部
"""
import sys
from pathlib import Path
import os
from time import sleep
import pickle
import json
import datetime
import collections as cl
from common.Logger import Logger
from common import exec_ctrl
from common import chat_config
import PredictExecutor as predic
import dataset
from scdv import SparseCompositeDocumentVectors, load_word2vec
import cleaning


class PredictIntaractiveExecutor(predic.PredictExecutor):
    """
    対話型（確認用）予測実行クラス
    """

    def input_line(self):
        print("> ", end="")
        sys.stdout.flush()
        line = sys.stdin.readline()
        return cleaning.remove_noise(line)
        
    def exec(self):
        """
        応答メッセージ生成のメイン
        Args:
            なし
        Returns:
            なし
        """
        self.logger.info("予測実行部を開始します")

        # 要求の入力
        line = self.input_line()

        # ここから永久ループ
        while line:
            if len(line.strip()) == 0:
                line = self.input_line()
                continue

            msg, classifier, score = self.predict(line)
            print(msg)

            # 要求の入力
            line = self.input_line()

        
    def exec_batch(self, line):
        """
        バッチ応答メッセージ生成のメイン
        Args:
            line: 要求メッセージ
        Returns:
            なし
        """

        if len(line.strip()) == 0:
            line = self.input_line()
            return ""

        return self.predict(line)
