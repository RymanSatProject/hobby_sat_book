# -*- coding: utf-8 -*-
import logging
import pickle
import numpy as np
from gensim.models.word2vec import Word2Vec
from tqdm import tqdm
from sklearn.mixture import GaussianMixture
from sklearn.feature_extraction.text import TfidfVectorizer


def build_word2vec(sentences, embedding_dim, min_count, window_size, sg):
    # モデルを作る
    model = Word2Vec(sentences, size=embedding_dim,
                     min_count=min_count, window=window_size, sg=sg)
    return model


def load_word2vec(model_path):
    model = Word2Vec.load(model_path)
    return model


class SparseCompositeDocumentVectors:
    def __init__(self, model, num_clusters, embedding_dim, pname1, pname2):
        self.min_no = 0
        self.max_no = 0
        self.prob_wordvecs = {}
        self.model = model
        self.word_vectors = model.wv.syn0   # 全ての単語のベクトル
        self.num_clusters = num_clusters
        self.num_features = embedding_dim
        self.pname1 = pname1
        self.pname2 = pname2

    def cluster_GMM(self):
        # Initalize a GMM object and use it for clustering.
        clf = GaussianMixture(
            n_components=self.num_clusters,
            covariance_type="tied",
            init_params="kmeans",
            max_iter=50
        )

        # Get cluster assignments.
        clf.fit(self.word_vectors)
        idx = clf.predict(self.word_vectors)
        print("Clustering Done...")

        # Get probabilities of cluster assignments.
        idx_proba = clf.predict_proba(self.word_vectors)

        # Dump cluster assignments and probability of cluster assignments.
        pickle.dump(idx, open(self.pname1, "wb"))
        print("Cluster Assignments Saved...")
        pickle.dump(idx_proba, open(self.pname2, "wb"))
        print("Probabilities of Cluster Assignments saved...")

        return (idx, idx_proba)

    def read_GMM(self):
        # Loads cluster assignments and probability of cluster assignments.
        idx = pickle.load(open(self.pname1, "rb"))
        idx_proba = pickle.load(open(self.pname2, "rb"))
        print("Cluster Model Loaded...")
        return (idx, idx_proba)

    def restore(self):
        idx, idx_proba = self.read_GMM()
        self.word_centroid_map = dict(zip(self.model.wv.index2word, idx))
        self.word_centroid_prob_map = dict(
            zip(self.model.wv.index2word, idx_proba))

    def get_probability_word_vectors(self, corpus):
        """
        corpus: list of lists of tokens
        """
        # This function computes probability word-cluster vectors.
        idx, idx_proba = self.cluster_GMM()

        # Create a Word / Index dictionary, mapping each vocabulary word to a cluster number
        # 単語とIndexのdictを生成
        word_centroid_map = dict(zip(self.model.wv.index2word, idx))
        # Create Word / Probability of cluster assignment dictionary, mapping
        # each vocabulary word to list of probabilities of cluster assignments.
        word_centroid_prob_map = dict(zip(self.model.wv.index2word, idx_proba))

        # Comoputing tf-idf values
        tfv = TfidfVectorizer(dtype=np.float32)
        # transform corpus to get tfidf value
        corpus = [" ".join(data) for data in corpus]

        _ = tfv.fit_transform(corpus)
        featurenames = tfv.get_feature_names()
        idf = tfv._tfidf.idf_
        # Creating a dictionary with word mapped to its idf value
        print("Creating word-idf dictionary for dataset...")
        word_idf_dict = {}
        for pair in zip(featurenames, idf):
            word_idf_dict[pair[0]] = pair[1]

        for word in word_centroid_map:
            self.prob_wordvecs[word] = np.zeros(
                self.num_clusters * self.num_features, dtype="float32")
            for index in range(self.num_clusters):
                try:
                    self.prob_wordvecs[word][index*self.num_features:(index+1)*self.num_features] = \
                        self.model[word] * word_centroid_prob_map[word][index] * \
                        word_idf_dict[word]
                except:
                    continue
        self.word_centroid_map = word_centroid_map

    def create_cluster_vector_and_gwbowv(self, tokens, flag):
        # This function computes SDV feature vectors.
        bag_of_centroids = np.zeros(
            self.num_clusters * self.num_features, dtype="float32")

        #print("self.prob_wordvecs=", self.prob_wordvecs)
        for token in tokens:
            try:
                _ = self.word_centroid_map[token]
            except:
                continue
            bag_of_centroids += self.prob_wordvecs[token]
        norm = np.sqrt(
            np.einsum('...i,...i', bag_of_centroids, bag_of_centroids))
        if norm != 0:
            bag_of_centroids /= norm
        # To make feature vector sparse, make note of minimum and maximum values.
        if flag:
            self.min_no += min(bag_of_centroids)
            self.max_no += max(bag_of_centroids)
        return bag_of_centroids

    def plain_word2vec_document_vectors(self, tokens):
        bag_of_centroids = np.zeros(self.num_features, dtype="float32")
        for token in tokens:
            try:
                temp = self.model[token]
            except:
                continue
            bag_of_centroids += temp

        bag_of_centroids = bag_of_centroids / len(tokens)
        return bag_of_centroids

    def make_gwbowv(self, corpus, train=True):
        # gwbowv is a matrix which contains normalized document vectors.
        gwbowv = np.zeros((len(corpus), self.num_clusters *
                           self.num_features)).astype(np.float32)
        cnt = 0
    #    for tokens in tqdm(corpus):
        for tokens in corpus:
            gwbowv[cnt] = self.create_cluster_vector_and_gwbowv(tokens, train)
            cnt += 1

        return gwbowv

    def make_word2vec(self, corpus, model):
        self.model = model
        w2docv = np.zeros((len(corpus), self.num_features)).astype(np.float32)
        cnt = 0
#        for tokens in tqdm(corpus):
        for tokens in corpus:
            w2docv[cnt] = self.plain_word2vec_document_vectors(tokens)
            cnt += 1

        return w2docv

    def get_word2vec_document_vector(self, tokens):
        # tokens: list of tokens
        return self.plain_word2vec_document_vectors(tokens)

    def dump_gwbowv(self, gwbowv, path="gwbowv_matrix.npy", percentage=0.04):
        # Set the threshold percentage for making it sparse.
        min_no = self.min_no * 1.0 / gwbowv.shape[0]
        max_no = self.max_no * 1.0 / gwbowv.shape[0]
        print("Average min: ", min_no)
        print("Average max: ", max_no)
        thres = (abs(max_no) + abs(min_no)) / 2
        thres = thres * percentage
        # Make values of matrices which are less than threshold to zero.
        temp = abs(gwbowv) < thres
        gwbowv[temp] = 0
        np.save(path, gwbowv)
        print("SDV created and dumped...")

    def load_matrix(self, name):
        return np.load(name)
