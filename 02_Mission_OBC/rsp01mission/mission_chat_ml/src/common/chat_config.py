# coding: utf-8
"""
衛星側チャットシステム　設定ファイル関係
"""

import pathlib
import yaml

# ルートディレクトリ
root_dir = str(pathlib.Path(__file__).parent.parent.parent.resolve())

# config.ymlの読み込み
configPath = root_dir + "/config/config.yml"
_config = None
with open(configPath, 'r') as yml:
    _config = yaml.load(yml)

# 本番環境以外の設定で上書き
activeProfile = _config["activeProfile"]
if activeProfile != "pro":
    configPathEx = root_dir + "/config/config-" + activeProfile + ".yml"
    with open(configPathEx, 'r') as yml:
        _configEx = yaml.load(yml)
        _config.update(_configEx)


def get_path(key):
    """
    パスが設定されているキーをconfig.ymlから検索し、絶対パスで返す。
    Args:
        key: キー
    Returns: 絶対パス
    """
    return root_dir + get(key)


def get(key):
    """
    指定キーの値を返す。
    Args:
        key: キー
    Returns: 設定値
    """
    return _config[key]
