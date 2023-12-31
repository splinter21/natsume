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

#ifndef NJD_SET_ACCENT_TYPE_C
#define NJD_SET_ACCENT_TYPE_C

#ifdef __cplusplus
#define NJD_SET_ACCENT_TYPE_C_START extern "C" {
#define NJD_SET_ACCENT_TYPE_C_END   }
#else
#define NJD_SET_ACCENT_TYPE_C_START
#define NJD_SET_ACCENT_TYPE_C_END
#endif                          /* __CPLUSPLUS */

NJD_SET_ACCENT_TYPE_C_START;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "njd.h"
#include "njd_set_accent_type.h"

#ifdef ASCII_HEADER
#if defined(CHARSET_EUC_JP)
#include "njd_set_accent_type_rule_ascii_for_euc_jp.h"
#elif defined(CHARSET_SHIFT_JIS)
#include "njd_set_accent_type_rule_ascii_for_shift_jis.h"
#elif defined(CHARSET_UTF_8)
#include "njd_set_accent_type_rule_ascii_for_utf_8.h"
#else
#error CHARSET is not specified
#endif
#else
#if defined(CHARSET_EUC_JP)
#include "njd_set_accent_type_rule_euc_jp.h"
#elif defined(CHARSET_SHIFT_JIS)
#include "njd_set_accent_type_rule_shift_jis.h"
#elif defined(CHARSET_UTF_8)
#include "njd_set_accent_type_rule_utf_8.h"
#else
#error CHARSET is not specified
#endif
#endif

#define MAXBUFLEN 1024

static char get_token_from_string(const char *str, int *index, char *buff)
{
   char c;
   int i = 0;

   c = str[(*index)];
   if (c != '\0') {
      while (c != '%' && c != '@' && c != '/' && c != '\0') {
         buff[i++] = c;
         c = str[++(*index)];
      }
      if (c == '%' || c == '@' || c == '/')
         (*index)++;
   }
   buff[i] = '\0';
   return c;
}


/*
TODO
1. 更新解析函数get_rule
2. 添加Mn@M
3. 实现部分规则
*/

// static void get_rule(const char *input_rule, const char *prev_pos, char *rule, int *add_type)
// {
//    int index = 0;
//    char buff[MAXBUFLEN];
//    char c = ' ';

//    if (input_rule) {
//       while (c != '\0') {
//          c = get_token_from_string(input_rule, &index, buff);
//          if ((c == '%' && strstr(prev_pos, buff) != NULL) || c == '@' || c == '/' || c == '\0') {
//             /* find */
//             if (c == '%')
//                c = get_token_from_string(input_rule, &index, rule);
//             else
//                strcpy(rule, buff);
//             if (c == '@') {
//                c = get_token_from_string(input_rule, &index, buff);
//                (*add_type) = atoi(buff);
//             } else {
//                (*add_type) = 0;
//             }
//             return;
//          } else {
//             /* skip */
//             while (c == '%' || c == '@')
//                c = get_token_from_string(input_rule, &index, buff);
//          }
//       }
//    }

//    /* not found */
//    strcpy(rule, "*");
//    (*add_type) = 0;
// }

static void get_rule(const char *input_rule, const char *prev_pos, char *rule, char *add_type)
{
   int index = 0;
   char buff[MAXBUFLEN];
   char c = ' ';

   if (input_rule) {
      while (c != '\0') {
         c = get_token_from_string(input_rule, &index, buff);
         if ((c == '%' && strstr(prev_pos, buff) != NULL) || c == '@' || c == '/' || c == '\0') {
            /* find */
            if (c == '%')
               c = get_token_from_string(input_rule, &index, rule);
            else
               strcpy(rule, buff);
            if (c == '@') {
               c = get_token_from_string(input_rule, &index, buff);
               strcpy(add_type, buff);
            } else {
               *add_type = '0';
            }
            return;
         } else {
            /* skip */
            while (c == '%' || c == '@')
               c = get_token_from_string(input_rule, &index, buff);
         }
      }
   }

   /* not found */
   strcpy(rule, "*");
   (*add_type) = '0';
}

// for rule F6
static int split_rule(const char *add_type, bool use_a) {
    int add_type_a, add_type_b = 0;
    sscanf(add_type, "%d|%d", &add_type_a, &add_type_b);
    if (use_a) 
        return add_type_a;
    else
        return add_type_b;
}


void njd_set_accent_type(NJD * njd)
{
   NJDNode *node;
   NJDNode *top_node = NULL;
   char rule[MAXBUFLEN];
   char add_type_string[MAXBUFLEN];
   int add_type = 0;
   int mora_size = 0;

   if (njd == NULL || njd->head == NULL)
      return;

   for (node = njd->head; node != NULL; node = node->next) {
      if (NJDNode_get_string(node) == NULL)
         continue;
      if ((node == njd->head) || (NJDNode_get_chain_flag(node) != 1)) {
         /* store the top node 韵律词的第一个单词的node */
         top_node = node;
         mora_size = 0;
      } else if (node->prev != NULL && NJDNode_get_chain_flag(node) == 1) {
         /* 获取当前单词的chain rule和前一个单词的词性 */
         get_rule(NJDNode_get_chain_rule(node), NJDNode_get_pos(node->prev), rule, add_type_string);

         /* change accent type */
         if (strcmp(rule, "*") == 0) 
         {  
            /* no chnage */
         } 
         else if (strcmp(rule, "F1") == 0) 
         {  
            /* 
            从属型　
            Mc = M1 
            */
         } 
         else if (strcmp(rule, "F2") == 0) 
         {
            /* 
            不完全支配型 
            Mc = N1 + M~2 (M1 == 0)
            Mc = M1 (M1 != 0) 
            */
           add_type = atoi(add_type_string);
            if (NJDNode_get_acc(top_node) == 0)
               NJDNode_set_acc(top_node, mora_size + add_type);
         } 
         else if (strcmp(rule, "F3") == 0) 
         {
            /*
            融合型
            Mc = M1 (M1 == 0)
            Mc = N1 + M~2 (M1 != 0)
            */
            add_type = atoi(add_type_string);
            if (NJDNode_get_acc(top_node) != 0)
               NJDNode_set_acc(top_node, mora_size + add_type);
         } 
         else if (strcmp(rule, "F4") == 0) 
         {
            /*
            支配型
            Mc = N1 + M~2
            */
            add_type = atoi(add_type_string);
            NJDNode_set_acc(top_node, mora_size + add_type);
         } 
         else if (strcmp(rule, "F5") == 0) 
         {
            /*
            平板化型
            Mc = 0
            */
            NJDNode_set_acc(top_node, 0);
         } 
         else if (strcmp(rule, "F6") == 0)
         {
            /* 
            F6@M~2a｜M~2b
            Mc = N1 + M~2a (M1 == 0)
            Mc = N1 + M~2b (M1 != 0)
            */
            if (NJDNode_get_acc(top_node) == 0)
               add_type = split_rule(add_type_string, true);
            else
               add_type = split_rule(add_type_string, false);
            NJDNode_set_acc(top_node, mora_size + add_type);
         }
         else if (strcmp(rule, "C1") == 0) 
         {  
            /*
            自立语结合保存型
            Mc = N1 + M2
            */
            NJDNode_set_acc(top_node, mora_size + NJDNode_get_acc(node));
         } 
         else if (strcmp(rule, "C2") == 0) 
         {
            /*
            自立语结合生起型
            Mc = N1 + 1
            */
            NJDNode_set_acc(top_node, mora_size + 1);
         } 
         else if (strcmp(rule, "C3") == 0) 
         {
            /*
            接辞结合标准型
            Mc = N1
            */
            NJDNode_set_acc(top_node, mora_size);
         } 
         else if (strcmp(rule, "C4") == 0) 
         {
            /*
            接辞结合平板化型
            Mc = 0
            */
            NJDNode_set_acc(top_node, 0);
         } 
         else if (strcmp(rule, "C5") == 0) 
         {
            /*
            从属型
            Mc = M1
            */
         } 
         else if (strcmp(rule, "P1") == 0) 
         {  
            /*
            一体化型
            Mc = 0 (M2 == 0 or M2 == N2)
            Mc = N1 + M2 (M2 != 0 and M2 != N2)
            */
            if (NJDNode_get_acc(node) == 0)
               NJDNode_set_acc(top_node, 0);
            else
               NJDNode_set_acc(top_node, mora_size + NJDNode_get_acc(node));
         } 
         else if (strcmp(rule, "P2") == 0) 
         {
            /*
            自立语结合型
            Mc = N1 + 1 (M2 == 0 or M2 == N2)
            Mc = N1 + M2 (M2 != 0 and M2 != N2)
            */
            if (NJDNode_get_acc(node) == 0 
               || NJDNode_get_acc(node) == NJDNode_get_mora_size(node))
               NJDNode_set_acc(top_node, mora_size + 1);
            else
               NJDNode_set_acc(top_node, mora_size + NJDNode_get_acc(node));
         } 
         else if (strcmp(rule, "P4") == 0)
         {
            /*
            混合型
            Mc = N1 + 1 (M2 != 0 and M2 != N2)
            Mc = N1 + M2 (M2 == 0 or M2 == N2)
            */
            if (NJDNode_get_acc(node) == 0 
               || NJDNode_get_acc(node) == NJDNode_get_mora_size(node))
               NJDNode_set_acc(top_node, mora_size + NJDNode_get_acc(node));
            else
               NJDNode_set_acc(top_node, mora_size + 1);    
            
         }
         else if (strcmp(rule, "P6") == 0) 
         {
            /*
            平板型
            Mc = 0
            */
            NJDNode_set_acc(top_node, 0);
         } 
         else if (strcmp(rule, "P13") == 0)
         {
            /*
            分离型1
            Mc = M1
            */
         }
         else if (strcmp(rule, "P14") == 0) 
         {
            /*
            分离型2
            Mc = N1 + M2
            */
            if (NJDNode_get_acc(node) != 0)
               NJDNode_set_acc(top_node, mora_size + NJDNode_get_acc(node));
         }
      }

      /* change accent type for digit */
      if (node->prev != NULL && NJDNode_get_chain_flag(node) == 1 &&
          strcmp(NJDNode_get_pos_group1(node->prev), NJD_SET_ACCENT_TYPE_KAZU) == 0 &&
          strcmp(NJDNode_get_pos_group1(node), NJD_SET_ACCENT_TYPE_KAZU) == 0) {
         if (strcmp(NJDNode_get_string(node), NJD_SET_ACCENT_TYPE_JYUU) == 0) { /* 10^1 */
            if (NJDNode_get_string(node->prev) != NULL &&
                (strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_SAN) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_YON) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_KYUU) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_NAN) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_SUU) == 0)) {
               NJDNode_set_acc(node->prev, 1);
            } else {
               NJDNode_set_acc(node->prev, 1);
            }
            if (NJDNode_get_string(node->prev) != NULL &&
                (strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_GO) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_ROKU) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_HACHI) == 0)) {
               if (node->next != NULL && NJDNode_get_string(node->next) != NULL
                   && (strcmp(NJDNode_get_string(node->next), NJD_SET_ACCENT_TYPE_ICHI) == 0
                       || strcmp(NJDNode_get_string(node->next), NJD_SET_ACCENT_TYPE_NI) == 0
                       || strcmp(NJDNode_get_string(node->next), NJD_SET_ACCENT_TYPE_SAN) == 0
                       || strcmp(NJDNode_get_string(node->next), NJD_SET_ACCENT_TYPE_YON) == 0
                       || strcmp(NJDNode_get_string(node->next), NJD_SET_ACCENT_TYPE_GO) == 0
                       || strcmp(NJDNode_get_string(node->next), NJD_SET_ACCENT_TYPE_ROKU) == 0
                       || strcmp(NJDNode_get_string(node->next), NJD_SET_ACCENT_TYPE_NANA) == 0
                       || strcmp(NJDNode_get_string(node->next), NJD_SET_ACCENT_TYPE_HACHI) == 0
                       || strcmp(NJDNode_get_string(node->next), NJD_SET_ACCENT_TYPE_KYUU) == 0))
                  NJDNode_set_acc(node->prev, 0);
            }
         } else if (strcmp(NJDNode_get_string(node), NJD_SET_ACCENT_TYPE_HYAKU) == 0) { /* 10^2 */
            if (NJDNode_get_string(node->prev) != NULL
                && strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_NANA) == 0) {
               NJDNode_set_acc(node->prev, 2);
            } else if (NJDNode_get_string(node->prev) != NULL &&
                       (strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_SAN) == 0 ||
                        strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_YON) == 0 ||
                        strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_KYUU) == 0 ||
                        strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_NAN) == 0)) {
               NJDNode_set_acc(node->prev, 1);
            } else {
               NJDNode_set_acc(node->prev,
                               NJDNode_get_mora_size(node->prev) + NJDNode_get_mora_size(node));
            }
         } else if (strcmp(NJDNode_get_string(node), NJD_SET_ACCENT_TYPE_SEN) == 0) {   /* 10^3 */
            NJDNode_set_acc(node->prev, NJDNode_get_mora_size(node->prev) + 1);
         } else if (strcmp(NJDNode_get_string(node), NJD_SET_ACCENT_TYPE_MAN) == 0) {   /* 10^4 */
            NJDNode_set_acc(node->prev, NJDNode_get_mora_size(node->prev) + 1);
         } else if (strcmp(NJDNode_get_string(node), NJD_SET_ACCENT_TYPE_OKU) == 0) {   /* 10^8 */
            if (NJDNode_get_string(node->prev) != NULL &&
                (strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_ICHI) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_ROKU) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_NANA) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_HACHI) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_IKU) == 0)) {
               NJDNode_set_acc(node->prev, 2);
            } else {
               NJDNode_set_acc(node->prev, 1);
            }
         } else if (strcmp(NJDNode_get_string(node), NJD_SET_ACCENT_TYPE_CHOU) == 0) {  /* 10^12 */
            if (NJDNode_get_string(node->prev) != NULL &&
                (strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_ROKU) == 0 ||
                 strcmp(NJDNode_get_string(node->prev), NJD_SET_ACCENT_TYPE_NANA) == 0)) {
               NJDNode_set_acc(node->prev, 2);
            } else {
               NJDNode_set_acc(node->prev, 1);
            }
         }
      }

      if (strcmp(NJDNode_get_string(node), NJD_SET_ACCENT_TYPE_JYUU) == 0 &&
          NJDNode_get_chain_flag(node) != 1 && node->next != NULL &&
          strcmp(NJDNode_get_pos_group1(node->next), NJD_SET_ACCENT_TYPE_KAZU) == 0) {
         NJDNode_set_acc(node, 0);
      }

      mora_size += NJDNode_get_mora_size(node);
   }
}

NJD_SET_ACCENT_TYPE_C_END;

#endif                          /* !NJD_SET_ACCENT_TYPE_C */
