# coding: utf-8
"""
衛星側チャットシステム　トレーニング前の準備を行う処理部
"""
import re
from common import cnvk

# ノイズ除去用の正規表現
NOISE_HASHTAG1_RE = re.compile(r"[#＃](.*?)[ 　]")
NOISE_HASHTAG2_RE = re.compile(r"[#＃].*$")
NOISE_URL1_RE = re.compile(r"[http\://](.*?)[ 　]")
NOISE_URL2_RE = re.compile(r"[http\://].*$")
NOISE_URL3_RE = re.compile(r"[https\://](.*?)[ 　]")
NOISE_URL4_RE = re.compile(r"[https\://].*$")
NOISE_OTHER_RE = re.compile(
    r"[、。]|[!-/:-@[-`{-~]|[︰-＠]|[゜・”]")    # TODO 多分すべて網羅できない


def remove_noise(val):
    """
    ノイズの除去。
    ・ハッシュタグの除去
    ・URLの除去
    ・句読点、その他の記号などの除去
    Args
        val: クリーニングを行う文章
    Returns クリーニング後の文章
    """
    cleane_val = val

    # ハッシュタグの除去
    cleane_val = remove_hashtag(cleane_val)

    # URLの除去
    cleane_val = remove_url(cleane_val)

    # 句読点、その他の記号などの除去
    cleane_val = re.sub(NOISE_OTHER_RE, " ", cleane_val)

    # ストップワードの除去
    cleane_val = remove_stop_word(cleane_val)

    return cleane_val


def remove_hashtag(val):
    """
    ハッシュタグの除去
    Args
        val: クリーニングを行う文章
    Returns クリーニング後の文章
    """
    cleane_val = val
    cleane_val = re.sub(NOISE_HASHTAG1_RE, " ", cleane_val)
    cleane_val = re.sub(NOISE_HASHTAG2_RE, "", cleane_val)

    return cleane_val


def remove_url(val):
    """
    URLの除去
    Args
        val: クリーニングを行う文章
    Returns クリーニング後の文章
    """
    cleane_val = val
    cleane_val = re.sub(NOISE_URL1_RE, " ", cleane_val)
    cleane_val = re.sub(NOISE_URL2_RE, "", cleane_val)
    cleane_val = re.sub(NOISE_URL3_RE, " ", cleane_val)
    cleane_val = re.sub(NOISE_URL4_RE, "", cleane_val)

    return cleane_val


def normalize(val):
    """
    正規化
    ・半角カタカナ⇒全角カタカナ
    ・全角英字⇒半角英字
    ・全角数字⇒半角数字
    ・小文字英字⇒大文字英字
    Args
        val: 正規化を行う文章
    Returns 正規化後の文章
    """
    cleane_val = val
    cleane_val = cnvk.convert(cleane_val, cnvk.H_KATA, cnvk.Z_KATA)
    cleane_val = cnvk.convert(cleane_val, cnvk.Z_ALPHA, cnvk.H_ALPHA)
    cleane_val = cnvk.convert(cleane_val, cnvk.Z_NUM, cnvk.H_NUM)
    cleane_val = cleane_val.upper()

    return cleane_val


def remove_stop_word(text):
    """
    ストップワードの除去
    Args
        val: クリーニングを行う文章
    Returns クリーニング後の文章
    """
    ret = text
    # ret = ret.replace('のですか', '')
    ret = ret.replace('?', '')
    ret = ret.replace('？', '')
    # ret = ret.replace('ですか', '')
    # ret = ret.replace('どうですか', '')
    ret = ret.replace('℃', '度')

    return ret
