#!/bin/bash

#
# チャット応答メッセージ生成プロセスの停止シェル
#

# 定数
# 実行パスのルート
readonly ROOT_PATH="/rsp01/mission_chat_ml"
# ログ出力先
readonly LOG_FILE="${ROOT_PATH}/logs/${0##*/}.log"

# ログ出力を行う
function log() {
  local fname=${BASH_SOURCE[1]##*/}
  echo -e "$(date '+%Y-%m-%dT%H:%M:%S') (${fname}:${BASH_LINENO[0]}:${FUNCNAME[1]}) $@" | tee -a ${LOG_FILE}
}

log "チャット応答メッセージ生成プロセスの停止シェル　開始"

# チャット機械学習のルートに移動
cd ${ROOT_PATH}

# PIDを取得し、チャット応答メッセージ生成プロセスを停止する
pid=`cat sh/pid`
log "チャット応答メッセージ生成プロセスを停止します PID=${pid}"
kill -15 ${pid}

rm -f sh/pid
ret=$?
log "PIDファイルを削除しました ret=${ret}"

log "チャット応答メッセージ生成プロセスの停止シェル　終了"

exit 0
