# coding: utf-8
"""
衛星側チャットシステム　機械学習のトレーニングのメイン
"""
import sys
import argparse
from sklearn.svm import SVC
from scdv import SparseCompositeDocumentVectors, build_word2vec
import pickle
from common.Logger import Logger
import dataset
from common import chat_config


def parse_args():
    """
    引数パーサ
    Args:
        なし
    Returns:
        パース結果
    """
    parser = argparse.ArgumentParser(description="Word2VecとSCDVのパラメータの設定")
    parser.add_argument(
        # Word2Vec、SCDV共有パラメータ size:単語ベクトルの次元数
        # TODO　多分、学習データの語彙数によって多くする必要がある
        '--embedding_dim', type=int, default=100)
    parser.add_argument(
        # Word2Vecパラメータ min_count:総頻度がこれより低い単語をすべて無視します
        #        '--min_count', type=int, default=5
        '--min_count', type=int, default=0)
    parser.add_argument(
        # Word2Vecパラメータ window: 文中の現在の単語と予測された単語との間の最大距離
        '--window_size', type=int, default=5)
    parser.add_argument(
        # Word2Vecパラメータ sg: 学習アルゴリズム：1:スキップグラム、そうでなければCBOW
        '--sg', type=int, default=1)
    parser.add_argument(
        # SCDVパラメータ num_clusters：GMMのクラスタ数で使用される
        '--num_clusters', type=int, default=50)
    parser.add_argument(
        # SCDVパラメータ pname1：GMMでクラスタリングしたPythonオブジェクトの格納先
        '--pname1', type=str, default=chat_config.get_path("gmmClusterSavePath"))
    parser.add_argument(
        # SCDVパラメータ pname2：
        '--pname2', type=str, default=chat_config.get_path("gmmProbClusterSavePath"))

    return parser.parse_args()


def train(args):
    """
    学習
    Args:
        引数パラメータ
    Returns:
        なし
    """
    sentences, train_x, test_x, train_t, test_t = dataset.load_corpus()
    print("test_x=" + str(test_x))

    # Word2Vecを作る
    model = build_word2vec(sentences, args.embedding_dim, args.min_count,
                           args.window_size, args.sg)

    vec = SparseCompositeDocumentVectors(
        model, args.num_clusters, args.embedding_dim, args.pname1, args.pname2)
    # 確率重み付き単語ベクトルを求める
    vec.get_probability_word_vectors(train_x)
    # 訓練データからSCDVを求める
    train_gwbowv = vec.make_gwbowv(train_x)
    # テストデータからSCDVを求める
    test_gwbowv = vec.make_gwbowv(test_x)

    # Test
#    clf = SVC(decision_function_shape='ovr', kernel="linear")
    clf = SVC(decision_function_shape='ovo', kernel="linear", probability=True)
#    clf = SVC(decision_function_shape='ovr', kernel="poly")
#    clf = SVC(decision_function_shape='ovr', kernel="rbf", gamma=0.1)
#    clf = SVC(decision_function_shape='ovo', kernel="rbf", gamma=0.1)
    clf.fit(train_gwbowv, train_t)
    y_pred = clf.predict(test_gwbowv)
    print('test_x=' + str(test_x))
    print('y_pred=' + str(y_pred))

    result = sum(1 * (p == t)
                 for p, t in zip(y_pred.tolist(), test_t)) / len(test_t)
    print("Accuracy: {:.2f}".format(result))

    print("save model")
    model.save(chat_config.get_path('modelSavePath'))
    pickle.dump(clf, open(chat_config.get_path('svcSavePath'), 'wb'))
    pickle.dump(vec, open(chat_config.get_path('scdvSavePath'), 'wb'))


if __name__ == "__main__":
    """
    メイン
    Args:
        なし
    Returns:
        なし
    """

    logger = Logger.get_logger()
    logger.info("トレーニングのメイン処理を開始します")

    try:
        train(parse_args())
    except:
        import traceback
        logger.error(traceback.format_exc())

    logger.info("トレーニングのメイン処理を終了します")
