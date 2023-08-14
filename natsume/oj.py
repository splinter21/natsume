import os
import pkg_resources
import six

if six.PY2:
    from urllib import urlretrieve
else:
    from urllib.request import urlretrieve

import tarfile

from natsume.openjtalk import OpenJTalk
from natsume.utils import MecabFeature, NJDFeature


DICT_URLS = {
    "naist-jdic": "https://github.com/r9y9/open_jtalk/releases/download/v1.11.1/open_jtalk_dic_utf_8-1.11.tar.gz"
}

# don't want to resolve multiple extensions...
DICT_PKGS = {
    "naist-jdic": "open_jtalk_dic_utf_8-1.11"
}

class OpenjtalkFrontend(object):
    def __init__(self, dict_name=None):
        self._dict_name = dict_name
        self._oj = None  # openjtalk
        self._dm = None  # dictionary manager

        self._initialize()

    def _initialize(self):
        self._dm = DictManager()
        dict_dir = self._dm.get_dict_dir(self._dict_name)
        self._oj = OpenJTalk(dn_mecab=dict_dir)

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
    
class DictManager(object):
    def __init__(self):
        self.default_dict_dir = "OPEN_JTALK_DICT_DIR"

    def get_dict_dir(self, dict_name=None):
        # TODO: check wether there's already a costumized dictionary available
        if dict_name is None:
            if self.check_dict_dir():
                # default dictionary dir is availale
                return self.default_dict_dir.encode("utf-8")
            else:
                # download naist-jdic as default dictionary
                print("No dictionary available, download {}".format(list(DICT_URLS.keys())[0]))
                dict_name = "naist-jdic"

        dict_dir = self.download_dict(dict_name)

        return dict_dir.encode("utf-8")

    def check_dict_dir(self):
        if self.default_dict_dir not in os.environ:
            return False
        return True
    
    @staticmethod
    def download_dict(dict_name):
        if dict_name not in DICT_URLS.keys():
            raise ValueError("No such dictionary available. Expected {}"
                            .format(",".join(list(DICT_URLS.keys()))))
        
        dict_url = DICT_URLS[dict_name]
        dict_pkg = DICT_PKGS[dict_name]

        filename = pkg_resources.resource_filename(__name__, "dic.tar.gz")
        print("Downloading dictionary from {}".format(dict_url))
        urlretrieve(dict_url, filename)

        print("Extracting tar file {}".format(filename))
        with tarfile.open(filename, mode="r|gz") as f:
            f.extractall(path=pkg_resources.resource_filename(__name__, ""))

        dict_dir = pkg_resources.resource_filename(__name__, dict_pkg)
        os.remove(filename)
        print("Successfully downloaded {} to {}".format(dict_name, dict_dir))

        return dict_dir
        

oj = OpenjtalkFrontend()

