# coding: utf-8
# cython: boundscheck=True, wraparound=True
# cython: c_string_type=unicode, c_string_encoding=ascii

import numpy as np

cimport numpy as np
np.import_array()

cimport cython
from libc.stdlib cimport calloc

from .openjtalk.mecab cimport Mecab, Mecab_initialize, Mecab_load, Mecab_analysis, Mecab_print, Mecab_get_feature
from .openjtalk.mecab cimport Mecab_get_feature, Mecab_get_size, Mecab_refresh, Mecab_clear
from .openjtalk.njd cimport NJD, NJD_initialize, NJD_refresh, NJD_print, NJD_clear
from .openjtalk cimport njd as _njd
from .openjtalk.text2mecab cimport text2mecab
from .openjtalk.mecab2njd cimport mecab2njd

cdef mecab_feature_get_string(char* feature):
    return (<bytes>(feature)).decode("utf-8")

cdef njd_node_get_string(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_string(node))).decode("utf-8")

cdef njd_node_get_pos(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_pos(node))).decode("utf-8")

cdef njd_node_get_pos_group1(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_pos_group1(node))).decode("utf-8")

cdef njd_node_get_pos_group2(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_pos_group2(node))).decode("utf-8")

cdef njd_node_get_pos_group3(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_pos_group3(node))).decode("utf-8")

cdef njd_node_get_ctype(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_ctype(node))).decode("utf-8")

cdef njd_node_get_cform(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_cform(node))).decode("utf-8")

cdef njd_node_get_orig(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_orig(node))).decode("utf-8")

cdef njd_node_get_read(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_read(node))).decode("utf-8")

cdef njd_node_get_pron(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_pron(node))).decode("utf-8")

cdef njd_node_get_acc(_njd.NJDNode* node):
    return _njd.NJDNode_get_acc(node)

cdef njd_node_get_mora_size(_njd.NJDNode* node):
    return _njd.NJDNode_get_mora_size(node)

cdef njd_node_get_chain_rule(_njd.NJDNode* node):
    return (<bytes>(_njd.NJDNode_get_chain_rule(node))).decode("utf-8")

cdef njd_node_get_chain_flag(_njd.NJDNode* node):
      return _njd.NJDNode_get_chain_flag(node)

cdef mecab2feature(Mecab* m):
    cdef int size = m.size
    features = []
    for i in range(size):
        values = ["*"] * 12 # TODO: don't hardcode
        feature_string = mecab_feature_get_string(m.feature[i])
        new_values= feature_string.split(",")  # split feature string into fields

        for i, new_value in enumerate(new_values):
            values[i] = new_value

        feature = {
            "surface": values[0],
            "pos": values[1],
            "pos_group1": values[2],
            "pos_group2": values[3],
            "pos_group3": values[4],
            "ctype": values[5],
            "cform": values[6],
            "orig": values[7],
            "read": values[8],
            "pron": values[9],
            "acc_mora_size": values[10],
            "chain_rule": values[11],
        }
        features.append(feature)
    return features

cdef node2feature(_njd.NJDNode* node):
  return {
    "string": njd_node_get_string(node),
    "pos": njd_node_get_pos(node),
    "pos_group1": njd_node_get_pos_group1(node),
    "pos_group2": njd_node_get_pos_group2(node),
    "pos_group3": njd_node_get_pos_group3(node),
    "ctype": njd_node_get_ctype(node),
    "cform": njd_node_get_cform(node),
    "orig": njd_node_get_orig(node),
    "read": njd_node_get_read(node),
    "pron": njd_node_get_pron(node),
    "acc": njd_node_get_acc(node),
    "mora_size": njd_node_get_mora_size(node),
    "chain_rule": njd_node_get_chain_rule(node),
    "chain_flag": njd_node_get_chain_flag(node),
  }


cdef njd2feature(_njd.NJD* njd):
    cdef _njd.NJDNode* node = njd.head
    features = []
    while node is not NULL:
      features.append(node2feature(node))
      node = node.next
    return features

cdef class OpenJTalk(object):
    """OpenJTalk

    Args:
        dn_mecab (bytes): Dictionaly path for MeCab.
    """
    cdef Mecab* mecab
    cdef NJD* njd

    def __cinit__(self, bytes dn_mecab=b"/usr/local/dic"):
        self.mecab = new Mecab()
        self.njd = new NJD()

        Mecab_initialize(self.mecab)
        NJD_initialize(self.njd)

        r = self._load(dn_mecab)
        if r != 1:
          self._clear()
          raise RuntimeError("Failed to initialize MeCab! Maybe you forget to download a dictionary?")

    def _clear(self):
      Mecab_clear(self.mecab)
      NJD_clear(self.njd)

    def _load(self, bytes dn_mecab):
        return Mecab_load(self.mecab, dn_mecab)

    def get_mecab_features(self, text):
        """Get MeCab features
        """
        cdef char buff[8192]

        if isinstance(text, str):
            text = text.encode("utf-8")
        text2mecab(buff, text)
        Mecab_analysis(self.mecab, buff)
        features = mecab2feature(self.mecab)
        Mecab_refresh(self.mecab)

        return features

    def get_njd_features(self, text):
        """Get NJD features
        """
        cdef char buff[8192]

        if isinstance(text, str):
            text = text.encode("utf-8")
        text2mecab(buff, text)
        Mecab_analysis(self.mecab, buff)
        mecab2njd(self.njd, Mecab_get_feature(self.mecab), Mecab_get_size(self.mecab))
        _njd.njd_set_pronunciation(self.njd)
        _njd.njd_set_digit(self.njd)
        _njd.njd_set_accent_phrase(self.njd)
        _njd.njd_set_accent_type(self.njd)
        _njd.njd_set_unvoiced_vowel(self.njd)
        _njd.njd_set_long_vowel(self.njd)
        features = njd2feature(self.njd)

        # Note that this will release memory for njd feature
        NJD_refresh(self.njd)
        Mecab_refresh(self.mecab)

        return features

    def __dealloc__(self):
        self._clear()
        del self.mecab
        del self.njd
