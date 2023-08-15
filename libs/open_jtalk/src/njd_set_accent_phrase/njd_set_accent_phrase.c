/* ----------------------------------------------------------------- */
/*           The Japanese TTS System "Open JTalk"                    */
/*           developed by HTS Working Group                          */
/*           http://open-jtalk.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2008-2016  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

#ifndef NJD_SET_ACCENT_PHRASE_C
#define NJD_SET_ACCENT_PHRASE_C

#ifdef __cplusplus
#define NJD_SET_ACCENT_PHRASE_C_START extern "C" {
#define NJD_SET_ACCENT_PHRASE_C_END   }
#else
#define NJD_SET_ACCENT_PHRASE_C_START
#define NJD_SET_ACCENT_PHRASE_C_END
#endif                          /* __CPLUSPLUS */

NJD_SET_ACCENT_PHRASE_C_START;

#include <stdio.h>
#include <string.h>

#include "njd.h"
#include "njd_set_accent_phrase.h"

#ifdef ASCII_HEADER
#if defined(CHARSET_EUC_JP)
#include "njd_set_accent_phrase_rule_ascii_for_euc_jp.h"
#elif defined(CHARSET_SHIFT_JIS)
#include "njd_set_accent_phrase_rule_ascii_for_shift_jis.h"
#elif defined(CHARSET_UTF_8)
#include "njd_set_accent_phrase_rule_ascii_for_utf_8.h"
#else
#error CHARSET is not specified
#endif
#else
#if defined(CHARSET_EUC_JP)
#include "njd_set_accent_phrase_rule_euc_jp.h"
#elif defined(CHARSET_SHIFT_JIS)
#include "njd_set_accent_phrase_rule_shift_jis.h"
#elif defined(CHARSET_UTF_8)
#include "njd_set_accent_phrase_rule_utf_8.h"
#else
#error CHARSET is not specified
#endif
#endif

static int strtopcmp(const char *str, const char *pattern)
{
   int i;

   for (i = 0;; i++) {
      if (pattern[i] == '\0')
         return i;
      if (str[i] == '\0')
         return -1;
      if (str[i] != pattern[i])
         return -1;
   }
}

void njd_set_accent_phrase(NJD * njd)
{
   NJDNode *node;

   if (njd == NULL || njd->head == NULL)
      return;

   for (node = njd->head->next; node != NULL; node = node->next) {
      /* 仅作用于词典中非连锁的词（绝大多数情况） */
      if (NJDNode_get_chain_flag(node) < 0) {
         /* Rule 01 デフォルトはくっつける */
         /* 没有适用规则的情况下，chain flag设置为1 */
         NJDNode_set_chain_flag(node, 1);

         /* Rule 02 「名詞」の連続はくっつける */
         /* 如果单词的前一个单词都是名词，则单词的chain flag设置为1 */
         if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
            if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
               NJDNode_set_chain_flag(node, 1);

         /* Rule 03 「形容詞」の後に「名詞」がきたら別のアクセント句に */
         /* 如果单词是名词且前一个单词是形容词，则单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_KEIYOUSHI) == 0)
            if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
               NJDNode_set_chain_flag(node, 0);

         /* Rule 04 「名詞,形容動詞語幹」の後に「名詞」がきたら別のアクセント句に */
         /* 如果单词是名词且前一个单词的pos是名词，pos1是形容动词语干，切单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
            if (strcmp
                (NJDNode_get_pos_group1(node->prev), NJD_SET_ACCENT_PHRASE_KEIYOUDOUSHI_GOKAN) == 0)
               if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
                  NJDNode_set_chain_flag(node, 0);

         /* Rule 05 「動詞」の後に「形容詞」or「名詞」がきたら別のアクセント句に */
         /* 如果单词是形容词或名词且前一个单词是动词，则单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_DOUSHI) == 0) {
            if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_KEIYOUSHI) == 0)
               NJDNode_set_chain_flag(node, 0);
            else if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
               NJDNode_set_chain_flag(node, 0);
         }

         /* Rule 06 「副詞」，「接続詞」，「連体詞」は単独のアクセント句に */
         /* 如果单词或前一个单词是副词、接续词或连体词时，则单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_FUKUSHI) == 0
             || strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_FUKUSHI) == 0
             || strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_SETSUZOKUSHI) == 0
             || strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_SETSUZOKUSHI) == 0
             || strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_RENTAISHI) == 0
             || strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_RENTAISHI) == 0)
            NJDNode_set_chain_flag(node, 0);

         /* Rule 07 「名詞,副詞可能」（すべて，など）は単独のアクセント句に */
         /* 如果单词或前一个单词的pos是名词，pos1是副词可能，则单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
            if (strcmp(NJDNode_get_pos_group1(node->prev), NJD_SET_ACCENT_PHRASE_FUKUSHI_KANOU)
                == 0)
               NJDNode_set_chain_flag(node, 0);
         if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
            if (strcmp(NJDNode_get_pos_group1(node), NJD_SET_ACCENT_PHRASE_FUKUSHI_KANOU) == 0)
               NJDNode_set_chain_flag(node, 0);

         /* Rule 08 「助詞」or「助動詞」（付属語）は前にくっつける */
         /* 如果单词是助词或助动词，则chain flag设置为1 */
         if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_JODOUSHI) == 0)
            NJDNode_set_chain_flag(node, 1);
         if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_JOSHI) == 0)
            NJDNode_set_chain_flag(node, 1);

         /* Rule 09 「助詞」or「助動詞」（付属語）の後の「助詞」，「助動詞」以外（自立語）は別のアクセント句に */
         /* 如果单词是自立语且前一个单词是助词或助动词，则单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_JODOUSHI) == 0)
            if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_JODOUSHI) != 0 &&
                strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_JOSHI) != 0)
               NJDNode_set_chain_flag(node, 0);
         if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_JOSHI) == 0)
            if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_JODOUSHI) != 0 &&
                strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_JOSHI) != 0)
               NJDNode_set_chain_flag(node, 0);

         /* Rule 10 「*,接尾」の後の「名詞」は別のアクセント句に */
         /* 如果单词是名词且前一个单词是接尾词，则单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos_group1(node->prev), NJD_SET_ACCENT_PHRASE_SETSUBI) == 0)
            if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
               NJDNode_set_chain_flag(node, 0);

         /* Rule 11 「形容詞,非自立」は「動詞,連用*」or「形容詞,連用*」or「助詞,接続助詞,て」or「助詞,接続助詞,で」に接続する場合に前にくっつける */
         /* 如果单词是非自立的形容词（難い　ガタイ），在前一个单词是以下情况时单词的chain flag设置为1 */
         if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_KEIYOUSHI) == 0)
            if (strcmp(NJDNode_get_pos_group1(node), NJD_SET_ACCENT_PHRASE_HIJIRITSU) == 0) {
               if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_DOUSHI) == 0) {
                  if (strtopcmp(NJDNode_get_cform(node->prev), NJD_SET_ACCENT_PHRASE_RENYOU) != -1)
                     /* 前一个单词是动词的连用形 */
                     NJDNode_set_chain_flag(node, 1);
               } else if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_KEIYOUSHI)== 0) {
                  if (strtopcmp(NJDNode_get_cform(node->prev), NJD_SET_ACCENT_PHRASE_RENYOU) != -1)
                     /* 前一个单词是形容词连用形 */
                     NJDNode_set_chain_flag(node, 1);
               } else if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_JOSHI) == 0) {
                  if (strcmp(NJDNode_get_pos_group1(node->prev),NJD_SET_ACCENT_PHRASE_SETSUZOKUJOSHI) == 0) {
                     /* 前一个单词是接续助词（から） */
                     if (strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_PHRASE_TE) == 0)
                        NJDNode_set_chain_flag(node, 1);
                     else if (strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_PHRASE_DE) == 0)
                        NJDNode_set_chain_flag(node, 1);
                  }
               }
            }

         /* Rule 12 「動詞,非自立」は「動詞,連用*」or「名詞,サ変接続」に接続する場合に前にくっつける */
         /* 如果单词是非自立的动词（なおし），在前一个单词是以下情况时，chain flag设置为1 */
         if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_DOUSHI) == 0)
            if (strcmp(NJDNode_get_pos_group1(node), NJD_SET_ACCENT_PHRASE_HIJIRITSU) == 0) {
               if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_DOUSHI) == 0) {
                  if (strtopcmp(NJDNode_get_cform(node->prev), NJD_SET_ACCENT_PHRASE_RENYOU) != -1)
                     /* 前一个单词是动词连用形 */
                     NJDNode_set_chain_flag(node, 1);
               } else if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_MEISHI) == 0) {
                  if (strcmp(NJDNode_get_pos_group1(node->prev),NJD_SET_ACCENT_PHRASE_SAHEN_SETSUZOKU) == 0)
                     /* 前一个单词是サ変接続名詞（noun that can function as a verb by attaching the verb 「する」 to it） */
                     NJDNode_set_chain_flag(node, 1);
               }
            }

         /* Extra Rule 01 「名詞」は「動詞」or「形容詞」に接続する場合に前にくってける */
         /* 如果单词是名词且前一个单词是动词或形容词时，则chain flag设置为1 */
         if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_MEISHI) == 0) {
            if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_DOUSHI) == 0)
               NJDNode_set_chain_flag(node, 1);
            else if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_KEIYOUSHI) == 0)
               NJDNode_set_chain_flag(node, 1);
         }

         /* Rule 13 「名詞」の後に「動詞」or「形容詞」or「名詞,形容動詞語幹」がきたら別のアクセント句に */
         /* 如果单词是名词且前一个单词是动词、形容词或形容动词词干名词（悲惨 ヒサン），则单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_MEISHI) == 0) {
            if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_DOUSHI) == 0 ||
                strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_KEIYOUSHI) == 0 ||
                strcmp(NJDNode_get_pos_group1(node), NJD_SET_ACCENT_PHRASE_KEIYOUDOUSHI_GOKAN) == 0)
               NJDNode_set_chain_flag(node, 0);
         }

         /* Rule 14 「記号」は単独のアクセント句に */ 
         /* 标点符号单独成为一个ap */
         if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_KIGOU) == 0 ||
             strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_KIGOU) == 0)
            NJDNode_set_chain_flag(node, 0);

         /* Rule 15 「接頭詞」は単独のアクセント句に */
         /* 接头词（御 ご）单独成为一个ap */
         if (strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_SETTOUSHI) == 0)
            NJDNode_set_chain_flag(node, 0);

         /* Rule 16 「*,*,*,姓」の後の「名詞」は別のアクセント句に */
         /* 如果单词是名词且前一个单词是姓，则单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos_group3(node->prev), NJD_SET_ACCENT_PHRASE_SEI) == 0
             && strcmp(NJDNode_get_pos(node), NJD_SET_ACCENT_PHRASE_MEISHI) == 0)
            NJDNode_set_chain_flag(node, 0);

         /* Rule 17 「名詞」の後の「*,*,*,名」は別のアクセント句に */
         /* 如果单词是名且前一个单词是名词，则单词的chain flag设置为0 */
         if (strcmp(NJDNode_get_pos(node->prev), NJD_SET_ACCENT_PHRASE_MEISHI) == 0
             && strcmp(NJDNode_get_pos_group3(node), NJD_SET_ACCENT_PHRASE_MEI) == 0)
            NJDNode_set_chain_flag(node, 0);

         /* Rule 18 「*,接尾」は前にくっつける */
         /* 如果单词是接尾词（頃　ごろ），则单词的chain flag设置为1 */
         if (strcmp(NJDNode_get_pos_group1(node), NJD_SET_ACCENT_PHRASE_SETSUBI) == 0)
            NJDNode_set_chain_flag(node, 1);
      }
   }
}

NJD_SET_ACCENT_PHRASE_C_END;

#endif                          /* !NJD_SET_ACCENT_PHRASE_C */
