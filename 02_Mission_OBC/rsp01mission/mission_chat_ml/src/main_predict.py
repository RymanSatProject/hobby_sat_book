# coding: utf-8
"""
衛星側チャットシステム　機械学習の応答メッセージ生成プロセスメイン
"""
import signal
from pathlib import Path
import sys
import argparse
from sklearn.svm import SVC
import pickle
import dataset
from common.Logger import Logger
from common import chat_config
from common import exec_ctrl
import PredictExecutor as predic


def signalHandler(signal, frame):
    """
    シグナル受信時のハンドラ
    Args:
        signal: シグナル番号
        frame: 現在のスタックフレーム
    Returns:
        なし
    """
    exec_ctrl.setStop()


if __name__ == "__main__":
    """
    メイン
    Args:
        なし
    Returns:
        なし
    """

    logger = Logger.get_logger()
    logger.info("応答メッセージ生成プロセスメイン処理を開始します")

    try:
        # プロセス停止時の処置
        signal.signal(signal.SIGINT, signalHandler)
        signal.signal(signal.SIGTERM, signalHandler)

        executor = predic.PredictExecutor()
        executor.exec()

    except:
        import traceback
        logger.error(traceback.format_exc())

    # 本プロセスの終了時は、予期しないエラー発生などを考慮し、応答メッセージロックファイルが存在する場合は強制的に削除する
    resLockFilePath = Path(chat_config.get_path("pathResLockFile"))
    if resLockFilePath.exists():
        resLockFilePath.unlink()
        logger.info("応答メッセージロックファイルを強制的に削除しました")

    logger.info("応答メッセージ生成プロセスメイン処理を終了します")
