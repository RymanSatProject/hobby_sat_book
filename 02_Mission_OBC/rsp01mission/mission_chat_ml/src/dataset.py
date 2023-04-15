# -*- coding: utf-8 -*-
import MeCab
import pandas as pd
from sklearn.model_selection import train_test_split
from common import chat_config
import cleaning

tagger = MeCab.Tagger('-O wakati')


def split_to_words(text):
    try:
        res = tagger.parse(text.strip()).strip().split()
    except:
        return []
    return res


def load_corpus():
    df = pd.read_csv(chat_config.get_path('corpusPath'),
                     names=('text', 'category'))

    # 参考記事のstem関数で語幹を抽出
    category = df['category']
    docs = df['text']

    wakatis = []
    for doc in docs:
        text = cleaning.remove_noise(doc)
        wakatis.append(split_to_words(text))

    # 学習と検証用のミニバッチデータを作成
    train_x, test_x, train_t, test_t = train_test_split(
        wakatis, category, test_size=0.001)

    return wakatis, train_x, test_x, train_t, test_t
