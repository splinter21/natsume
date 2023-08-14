from natsume.openjtalk import OpenJTalk

from natsume.utils import MecabFeature, NJDFeature


class OpenjtalkFrontend(object):
    def __init__(self, dict_dir):
        self._dict_dir = dict_dir
        self._oj = None

        self._initialize()

    def _initialize(self):
        self._oj = OpenJTalk(dn_mecab=self._dict_dir)

    def set_dict_dir(self, dict_dir):
        self._dict_dir = dict_dir
        self._initialize()

    def get_features(self, text, mode="word"):
        features = []
        if mode == "word":
            raw_features = self.get_mecab_features(text)
            for raw_feature in raw_features:
                feature = MecabFeature(raw_feature)
                features.append(feature)
        elif mode == "phrase":
            raw_features = self.get_njd_features(text)
            for raw_feature in raw_features:
                feature = NJDFeature(raw_feature)
                features.append(feature)

        return features

    def get_mecab_features(self, text):
        features = self._oj.get_mecab_features(text)

        return features
    
    def get_njd_features(self, text):

        features = self._oj.get_njd_features(text)

        return features


