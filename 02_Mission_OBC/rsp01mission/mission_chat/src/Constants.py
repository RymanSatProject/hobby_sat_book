# -*- coding: utf-8 -*-
import os

##共通部

# 自ファイルのフォルダパス
baseFolderPath = os.path.dirname(os.path.abspath(__file__))

# レポジトリルートパス
repositoryRootPath = os.path.normpath(os.path.join(baseFolderPath,"..",".."))

# json拡張子
JSON_EXT = ".json"

# logging 設定ファイル
logConfFilename = "logging.conf"

##要求メッセージ保存関連
requestFolderPath = os.path.normpath(os.path.join(baseFolderPath, "..", "data", "req"))
REQUEST_SAVE_MESSAGE_LOCKFILE = "lock.lock"
REQUEST_SAVE_MESSAGE_LOCKFILEPATH = os.path.join(requestFolderPath,REQUEST_SAVE_MESSAGE_LOCKFILE)

##応答メッセージ関連

# レスポンスデータ保存ディレクトリルートパス
responseRootFolderPath = os.path.normpath(os.path.join(baseFolderPath, "..", "data", "res"))
# レスポンスデータ　送信中ディレクトリルートパス
responseProgressFolderPath = os.path.normpath(os.path.join(responseRootFolderPath, "progress"))
# レスポンスデータ 送信済みオリジナルファイル保存用 ディレクトリルートパス
responseDoneOriginalFolderPath = os.path.normpath(os.path.join(responseRootFolderPath, "done", "original"))
# レスポンスデータ 圧縮分割済みファイル保存用 ディレクトリルートパス
responseDoneCompressedFolderPath = os.path.normpath(os.path.join(responseRootFolderPath, "done", "compressed"))
# 送信済みルートディレクトリ名
DONE_FOLDER_NAME = "done"

# 応答メッセージロックファイル
RESPONSE_MESSAGE_LOCKFILE = "lock.lock"
RESPONSE_MESSAGE_LOCKFILEPATH = os.path.join(responseRootFolderPath,RESPONSE_MESSAGE_LOCKFILE)

#configフォルダーパス
configFolderPath = os.path.normpath(os.path.join(baseFolderPath,"..","config"))

# レスポンスID格納ファイル
RESPONSE_ID_FILENAME = "response_id.dat"

# レスポンスIDフォルダーパス
responseIdFilePath = os.path.normpath(os.path.join(configFolderPath,RESPONSE_ID_FILENAME))

# 応答メッセージ圧縮時の分割サイズ(comp_data用のサイズ, Byte)
DOWNLINK_MAX_DATA_SIZE_BYTE = 128

# 応答メッセージ返却時に圧縮対象とするファイルの最大数
COMPRESSED_TARGET_FILE_NUM = 10

## 状態取得系コマンド

# 状態取得時にファイル名を指定したときに、ログに含むファイル件数
STATUS_COLLECT_TARGET_FILE_NUM = 10

# 状態取得時にファイル名を指定したときのセパレータ
STATUS_COLLECT_FILENAMES_SEPARATOR= ","

# 応答メッセージ生成プロセスのpidが格納されるファイルパス
pidFilePath = os.path.join(baseFolderPath,"..","..","mission_chat_ml","sh","pid")

## ログ取得コマンド
chatLogFolderPath = os.path.join(baseFolderPath,"..","logs")
chatMLLogFolderPath = os.path.join(baseFolderPath,"..","..","mission_chat_ml","logs")
chatLogFileName = "chat.log"
chatMLLogFileName = "chat_ml.log"
##応答メッセージ生成プロセス関連
#pyenvの環境名
pyenvChatName = "chat01"
#実行ファイルパス
predicorFilePath = os.path.normpath(os.path.join(repositoryRootPath,"mission_chat_ml","sh","start_predict.sh"))
