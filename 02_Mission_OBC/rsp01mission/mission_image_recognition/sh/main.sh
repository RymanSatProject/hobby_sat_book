#!/bin/bash
# e:エラー発生時にシェル停止
set -e

# 定数
# 実行パスのルート
readonly ROOT_PATH=/rsp01/mission_image_recognition
# pyenv 環境名
readonly PYENV_NAME=image_recognition
# pyenv activateのパス
readonly PYENV_ACTIVATE=${HOME}/.pyenv/versions/${PYENV_NAME}/bin/activate

# 実行時に/mission_image_recognition からの相対パスが必要な為、カレントを移動する
cd ${ROOT_PATH}

# pyenv仮想環境をアクティベート
. ${PYENV_ACTIVATE} ${PYENV_NAME}

# main.py にすべての引数を渡し実行する
timeout 3600 python ${ROOT_PATH}/main.py $*
