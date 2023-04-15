#!/bin/bash

# e:エラー発生時はシェル停止
# u:未定義の変数が出現したらシェル停止
set -e

#
# チャット応答メッセージ生成プロセスの起動シェル
#

# 定数
# 実行パスのルート
readonly ROOT_PATH=/rsp01/mission_chat_ml
# pyenv環境名
readonly PYENV_NAME=chat_ml
# /home/pi/.pyenv/bin/pyenv
readonly PYENV_PATH=~/.pyenv/bin/pyenv
# pyenv activateのパス
readonly PYENV_ACTIVATE=~/.pyenv/versions/${PYENV_NAME}/bin/activate
# ログ出力先
readonly LOG_FILE=${ROOT_PATH}/logs/${0##*/}.log
# 二起動チェックのコマンド
readonly PROC_CHK_CMD='ps -ef | grep -v grep | grep main_predict.py | wc -l'

# ログ出力を行う
function log() {
  local fname=${BASH_SOURCE[1]##*/}
  echo -e "$(date '+%Y/%m/%d %H:%M:%S') (${fname}:${BASH_LINENO[0]}:${FUNCNAME[1]}) $@" | tee -a ${LOG_FILE}
}

# 二重起動チェック
proc_count=$(eval ${PROC_CHK_CMD})
if [ $proc_count != "0" ]; then
  log "すでにチャット応答プロセスが起動しているため、起動を行わなずに終了します"
  exit 9
fi

log "チャット応答メッセージ生成プロセスの起動シェル　開始"

# チャット機械学習のルートに移動
cd ${ROOT_PATH}

# pyenv仮想環境をアクティベート
. ${PYENV_ACTIVATE} ${PYENV_NAME}
set | grep PYENV
ret=`python -V`
log "pyenv仮想環境をアクティベート python version = ${ret}"
ret=`${PYENV_PATH} version`
log "pyenv仮想環境をアクティベート pyenv name = ${ret}"

# チャット応答メッセージ生成プロセスの起動
nohup python src/main_predict.py 2>&1 &
pid=$!
log "チャット応答メッセージ生成プロセスを起動しました PID=${pid}"

# PIDをファイルに保存
echo ${pid} > $(dirname $0)/pid
log "PIDを保存しました $(dirname $0)/pid"

log "チャット応答メッセージ生成プロセスの起動シェル　終了"

exit 0
