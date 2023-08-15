# Natsume/棗

[![PyPI](https://img.shields.io/pypi/v/natsume.svg)](https://pypi.python.org/pypi/natsume)

## Introduction

Natsume is a toolkit for Japanese text frontend processing. It's based on the open source project [OpenJTalk](http://open-jtalk.sp.nitech.ac.jp/) and its python wrapper [pyopenjalk](https://github.com/r9y9/pyopenjtalk).

Natsume mainly focuses on grapheme-to-phoneme (g2p) conversion and accent annotation. Since the NJD features in OpenJTalk are already enough for determining the boundary and accent nucleus of an accent phrase, Natsume doesn't use [full context labels](http://hts.sp.nitech.ac.jp/archives/2.3/HTS-demo_NIT-ATR503-M001.tar.bz2) for simplicity and accuracy.

Also, Natsume provides some other functions, including one converting Japanese new fonts to old fonts.

## Build Requirements

- C/C++ compilers
- cmake 
- cython

## Platforms

- Mac OSX

- Linux

## Installation

```bash
pip install natsume
```

If you're having problems installing, please refer to [common problems](docs/common_problems.md)

## Usage

### Grapheme-to-Phoneme

```python
from natsume import Natsume

frontend = Natsume()

text = "天気がいいから、散歩しましょう。"
```

**Romaji**

Convert text to Romaji.

```python
phonemes = frontend.g2p(text, phoneme_mode="romaji", token_mode="phrase")
print(" ".join(phonemes))
```

```
teNkiga iikara , saNpo shimasho: .
```

**IPA**

Convert text to IPA (broad transcription).

```python
phonemes = frontend.g2p(text, phoneme_mode="ipa", token_mode="phrase")
print(" ".join(phonemes))
```

```
teNkiga iikaɾa , saNpo ɕimaɕo: .
```

**With Accent**

Convert text to phonemes with accent.

```python
phonemes = frontend.g2p(text, phoneme_mode="romaji", token_mode="phrase", with_accent=True)
print(" ".join(phonemes))
```

```
teꜜNkiga iꜜikara , saꜛNpo shiꜛmashoꜜ: .
```

### MeCab Features

Get intermediate [MeCab](https://taku910.github.io/mecab/) features.

```python
from natsume import Natsume

frontend = Natsume()

text = "人間は「食べて寝て」を繰り返すと牛になる。"

mecab_features = frontend.text2mecab(text)

for mecab_feature in mecab_features:
    surface = mecab_feature["surface"]
    feature_string = ",".join(list(mecab_feature.values())[1:])
    print("{}\t{}".format(surface, feature_string))
```

```
人間	名詞,一般,*,*,*,*,人間,ニンゲン,ニンゲン,0/4,C2
は	助詞,係助詞,*,*,*,*,は,ハ,ワ,0/1,名詞%F1/動詞%F2@0/形容詞%F2@0
「	記号,括弧開,*,*,*,*,「,「,「,*/*,*
食べ	動詞,自立,*,*,一段,連用形,食べる,タベ,タベ,2/2,*
て	助詞,接続助詞,*,*,*,*,て,テ,テ,0/1,動詞%F1/形容詞%F1/名詞%F5
寝	動詞,自立,*,*,一段,連用形,寝る,ネ,ネ,1/1,*
て	助詞,接続助詞,*,*,*,*,て,テ,テ,0/1,動詞%F1/形容詞%F1/名詞%F5
」	記号,括弧閉,*,*,*,*,」,」,」,*/*,*
を	助詞,格助詞,一般,*,*,*,を,ヲ,ヲ,0/1,動詞%F5/名詞%F1
繰り返す	動詞,自立,*,*,五段・サ行,基本形,繰り返す,クリカエス,クリカエス,3/5,*
と	助詞,接続助詞,*,*,*,*,と,ト,ト,0/1,形容詞%F1/動詞%F1
牛	名詞,一般,*,*,*,*,牛,ウシ,ウシ,0/2,C3
に	助詞,格助詞,一般,*,*,*,に,ニ,ニ,0/1,動詞%F5/形容詞%F1/名詞%F1
なる	動詞,自立,*,*,五段・ラ行,基本形,なる,ナル,ナル,1/2,*
。	記号,句点,*,*,*,*,。,。,。,*/*,*
```

### Font Conversion

Convert new fonts to old fonts and vice versa. 

Please note that 「弁」 has several old fonts. Now, for simplicity, Natsume converts it to 「辯」. 

```python
from natsume import Natsume

frontend = Natsume()

new = "桜、桜、うたかたに。"

old = frontend.convert_fonts(new, reverse=False)
print(old)
```
```
櫻、櫻、うたかたに。
```

## LICENCE

- Natsume: GPL license ([LICENSE](LICENSE))
- pyopenjtalk: MIT license ([LICENSE.md](https://github.com/r9y9/pyopenjtalk/LICENSE.md))
- OpenJTalk: Modified BSD license ([COPYING](https://github.com/r9y9/open_jtalk/blob/1.10/src/COPYING))

## References

- [OpenJTalk](https://open-jtalk.sourceforge.net/)
- [pyopenjtalk](https://github.com/r9y9/pyopenjtalk)
- [OpenJTalkの解析資料](https://www.negi.moe/negitalk/openjtalk.html)
- [Wikipedia: Hiragana](https://en.wikipedia.org/wiki/Hiragana)
- [新旧字体対照表](https://hagitaka.work/wp-content/uploads/2021/07/%E6%96%B0%E6%97%A7%E5%AD%97%E4%BD%93%E5%AF%BE%E7%85%A7%E8%A1%A8-1.pdf)

