from natsume.oj import OpenjtalkFrontend
from natsume.utils import (
    features_to_tokens, tokens_to_phonemes, convert_fonts
)

class Natsume(object):
    def __init__(self,
                 dict_dir="/Users/hurui05/Desktop/natsume/natsume/naist-jdic".encode("utf-8")):
        self._oj = OpenjtalkFrontend(dict_dir)
        self._g2p_modes = ["romaji", "ipa"]
        self._token_modes = ["word", "phrase"]

    def tokenize(self, text, mode="word"):
        """Tokenize text into tokens
        """
        if mode not in self._token_modes:
            raise ValueError(
                "Invalid mode for tokenization. Expected {}, got {} instead."
                .format(", ".join(self._token_modes), mode)
            )
        
        features = self._oj.get_features(text, mode=mode)
        tokens = features_to_tokens(features, mode=mode)
        return tokens


    def g2p(self, text, phoneme_mode="romaji", token_mode="word", with_accent=False):
        """Grapheme-to-phoneme conversion
        """
        if phoneme_mode not in self._g2p_modes:
            raise ValueError(
                "Invalid mode for g2p. Expected {}, got {} instead."
                .format(", ".join(self._g2p_modes), phoneme_mode)
            )
        
        tokens = self.tokenize(text, token_mode)
        phonemes = tokens_to_phonemes(tokens, phoneme_mode, with_accent)

        return phonemes

    def convert_fonts(self, text, reverse=False):
        """Conversion between new fonts and old fonts
        """
        text = convert_fonts(text, reverse=reverse)

        return text
    
    def text2mecab(self, text):
        """Get raw MeCab features
        """
        mecab_features = self._oj.get_mecab_features(text)
        
        return mecab_features


    def tex2njd(self, text):
        """Get raw NJD features
        """
        njd_features = self._oj.get_njd_features(text)

        return njd_features


if __name__ == "__main__":
    frontend = Natsume()
    text = "そうじゃなくて。"
    print(frontend.g2p(text, phoneme_mode="ipa", token_mode="phrase", with_accent=True))
    
    # phonemes = frontend.g2p(text, phoneme_mode="ipa", token_mode="phrase")
    # print(phonemes)

    # mecab_features = frontend.text2mecab(text)
    # for f in mecab_features:
    #     print(f)
