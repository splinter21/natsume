import warnings

from natsume.mappings import (
    kana_to_romaji, kana_to_ipa, new_to_old, small_to_normal,
    hira_to_kata, kata_to_manyou, vowels_romaji, vowels_ipa
)

def features_to_tokens(features, mode="word"):
    """MeCab or NJD features to tokens
    """
    tokens = []
    if mode == "word":
        for mecab_feature in features:
            surface = mecab_feature.surface()
            pron = mecab_feature.pronunciation()
            acc = mecab_feature.accent_nucleus()
            mora_size = mecab_feature.mora_size()
            token = Token(surface, pron, acc, mora_size)
            tokens.append(token)
    elif mode == "phrase":
        i = 0
        for njd_feature in features:
            chain_flag = njd_feature.chain_flag()
            surface = njd_feature.surface()
            pron = njd_feature.pronunciation()
            acc = njd_feature.accent_nucleus()
            mora_size = njd_feature.mora_size()
            if chain_flag == 1:
                pre_token = tokens[i-1]
                pre_surface = pre_token.surface() + surface
                pre_pron = pre_token.pronunciation() + pron
                pre_mora_size = pre_token.mora_size() + mora_size

                pre_token.set_surface(pre_surface)
                pre_token.set_pronunciation(pre_pron)
                pre_token.set_mora_size(pre_mora_size)
                tokens[i-1] = pre_token
            else:
                token = Token(surface, pron, acc, mora_size)
                tokens.append(token)
                i += 1

    return tokens

def tokens_to_phonemes(tokens, mode="romaji", with_accent=False):
    """Convert a list of tokens into phonemes
    """

    prons = [token.pronunciation() for token in tokens]
    accs = [token.accent_nucleus() for token in tokens]

    # TODO: use unvoiced vowels?
    prons = [pron.replace("’", "") for pron in prons]

    if mode == "romaji":
        mapping = kana_to_romaji
    elif mode == "ipa":
        mapping = kana_to_ipa
    else:
        return prons
    
    phonemes = []
    for pron, acc in zip(prons, accs):
        phoneme = convert_by_mapping(pron, mapping)
        if with_accent:
            phoneme = assign_accent(phoneme, acc, mode)
        phonemes.append(phoneme)

    return phonemes

def convert_fonts(src_fonts, reverse=False):
    """Convert fonts by mapping
    """

    mapping = new_to_old
    if reverse:
        mapping = reverse_mapping(mapping)

    trg_fonts = replace_by_mapping(src_fonts, mapping)

    return trg_fonts

def reverse_mapping(src_mapping):
    """Reverse a one-to-one mapping
    """
    trg_mapping = {}
    for key, value in src_mapping.items():
        trg_mapping[value] = key
    
    return trg_mapping

def replace_by_mapping(src_str, mapping):
    """Replace each characters by mapping
    """

    if not src_str:
        return ""

    trg_str = ""
    keys = mapping.keys()
    for src_char in src_str:
        if src_char in keys:
            trg_char = mapping[src_char]
            trg_str += trg_char
        else:
            trg_str += src_char

    return trg_str
            

def convert_by_mapping(src_str, mapping):
    """Convert source string to target string by mapping
    """
    if not src_str:
        return ""
    
    start = 0
    end = 0
    src_sub_str = ""
    trg_str = ""
    keys = mapping.keys()
    length = len(src_str)

    while end < length:
        src_sub_str = src_str[start:end+1]
        if not end + 1 >= length:
            next_char = src_str[end+1]
            # see wether there's a longer sub string in the mapping
            if src_sub_str + next_char in keys:
                src_sub_str += next_char
                end += 1
                continue

        if src_sub_str in keys:
            trg_sub_str = mapping[src_sub_str]
            trg_str += trg_sub_str
        else:
            warnings.warn("Not such key {} in mapping!".format(src_sub_str))

        end += 1
        start = end

    return trg_str

def assign_accent(phonemes, acc, mode="romaji", upstep="ꜛ", downstep="ꜜ"):
    if mode == "romaji":
        vowels = vowels_romaji
    elif mode == "ipa":
        vowels = vowels_ipa

    acc = int(acc)  
    new_phonemes = ""
    cur_mora = 0
    i = 0
    for phoneme in phonemes:
        new_phonemes += phoneme
        i += 1 # count current phonemes
        if phoneme not in vowels:
            # consonant
            continue
        cur_mora += 1
        # the first mora
        if cur_mora == 1:
            if acc == 1:
                # head-high
                new_phonemes += downstep
                break
            elif acc == 0:
                # flat
                new_phonemes += upstep
                break
            else:
                # middle-high or tail-high
                new_phonemes += upstep
                continue
            
        # for middle high and tail-high
        if cur_mora == acc:
            new_phonemes += downstep

    # append the rest phonemes
    new_phonemes += phonemes[i:]

    return new_phonemes

class MecabFeature(object):
    """Data structure for MeCab features
    """
    def __init__(self, feature):
        self._feature = feature
        self._surface = feature["surface"]
        self._pos = feature["pos"]
        self._pos_group1 = feature["pos_group1"]
        self._pos_group2 = feature["pos_group2"]
        self._pos_group3 = feature["pos_group3"]
        self._ctype = feature["ctype"]
        self._cform = feature["cform"]
        self._orig = feature["orig"]
        self._read = feature["read"]
        self._pron = feature["pron"]
        self._parse_acc_mora_size(feature["acc_mora_size"])
        self._chain_rule = feature["chain_rule"]

    def _parse_acc_mora_size(self, acc_mora_size):
        # acc/mora_size
        acc, mora_size = acc_mora_size.split("/")
        self._acc = acc
        self._mora_size = mora_size

    def feature(self):
        return self._feature
    
    def surface(self):
        return self._surface
    
    def part_of_speech(self):
        return self._pos
    
    def part_of_speech_group1(self):
        return self._pos_group1
    
    def part_of_speech_group2(self):
        return self._pos_group2
    
    def part_of_speech_group3(self):
        return self._pos_group3
    
    def conjugation_type(self):
        return self._ctype
    
    def conjugation_form(self):
        return self._cform

    def original_form(self):
        return self._orig
    
    def reading(self):
        return self._read
    
    def pronunciation(self):
        return self._pron
    
    def accent_nucleus(self):
        return self._acc
    
    def mora_size(self):
        return self._mora_size
    
    def chain_rule(self):
        return self._chain_rule

class NJDFeature(object):
    """Data structure for NJD features
    """
    def __init__(self, feature):
        self._feature = feature
        self._surface = feature["string"]
        self._pos = feature["pos"]
        self._pos_group1 = feature["pos_group1"]
        self._pos_group2 = feature["pos_group2"]
        self._pos_group3 = feature["pos_group3"]
        self._ctype = feature["ctype"]
        self._cform = feature["cform"]
        self._orig = feature["orig"]
        self._read = feature["read"]
        self._pron = feature["pron"]
        self._acc = feature["acc"]
        self._mora_size = feature["mora_size"]
        self._chain_rule = feature["chain_rule"]
        self._chain_flag = feature["chain_flag"]
    
    def feature(self):
        return self._feature
    
    def surface(self):
        return self._surface
    
    def part_of_speech(self):
        return self._pos
    
    def part_of_speech_group1(self):
        return self._pos_group1
    
    def part_of_speech_group2(self):
        return self._pos_group2
    
    def part_of_speech_group3(self):
        return self._pos_group3
    
    def conjugation_type(self):
        return self._ctype
    
    def conjugation_form(self):
        return self._cform

    def original_form(self):
        return self._orig
    
    def reading(self):
        return self._read
    
    def pronunciation(self):
        return self._pron
    
    def accent_nucleus(self):
        return self._acc
    
    def mora_size(self):
        return self._mora_size
    
    def chain_rule(self):
        return self._chain_rule
    
    def chain_flag(self):
        return self._chain_flag
    

class Token(object):
    """Data structure for tokens
    """
    def __init__(self, surface, pron, acc, mora_size):
        self._surface = surface
        self._pron = pron
        self._acc = acc
        self._mora_size = mora_size

    
    def surface(self):
        return self._surface
    
    def pronunciation(self):
        return self._pron
    
    def accent_nucleus(self):
        return self._acc
    
    def mora_size(self):
        return self._mora_size
    
    def set_surface(self, surface):
        self._surface = surface

    def set_pronunciation(self, pron):
        self._pron = pron

    def set_accent_nucleus(self, acc):
        self._acc = acc

    def set_mora_size(self, mora_size):
        self._mora_size = mora_size
