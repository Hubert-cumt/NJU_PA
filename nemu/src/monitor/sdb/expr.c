/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

// Add the vaddr.h to use function vaddr_read() for pointer dereferencing.
#include <memory/vaddr.h>

enum
{
  TK_NOTYPE = 256, // whitespace string
  TK_INTEGER,      // decimal integer
  TK_HEX,          // hexdecimal integer
  TK_REG,          // Retrieve the value of a register
  TK_LEFT,
  TK_RIGHT,

  TK_DEREF,
  TK_NEGA, // Negative sign


  // Operator precedence

  // 300
  TK_AND = 301,

  // 310
  TK_EQ = 311, // equality symbol
  TK_NEQ,      // unequal symbol

  // 320
  TK_PLUS = 321,
  TK_MINUS,

  // 330
  TK_MULTI = 331,
  TK_DIV,

  /* TODO: Add more token types */
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE}, // spaces
    {"\\+", TK_PLUS},  // plus
    {"\\-", TK_MINUS},
    {"\\/", TK_DIV},
    {"\\*", TK_MULTI},
    {"\\(", TK_LEFT},
    {"\\)", TK_RIGHT},
    {"==", TK_EQ}, 
    {"!=", TK_NEQ},
    {"&&", TK_AND},

    /* plcing hexadecimal before decimal is done to prevent
     * the decimal matching from mistakenly capturing the '0'
     * in the '0x' perfix of the hexadecimal.
     */

    {"0x[0-9A-Fa-f]+", TK_HEX},
    {"[0-9]+u?", TK_INTEGER}, // decimal integer

    // Regular expression to match registers.
    {"\\$(ra|sp|gp|tp|t[0-6]|s[0-9]|a[0-7])", TK_REG},

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[10000] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        char token_content[32] = {};
        char *token_begin = substr_start;

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;

          // Tranfer 0x.. -> uint32_t
          case TK_HEX:
            for (int k = 0; k < substr_len; k++) {
              token_content[k] = *token_begin;
              token_begin++;
            }
            //  add the infomation of token to tokens one by one.
            tokens[nr_token].type = rules[i].token_type;
            /* the assignment to expression with array type is not
             * allowed in the c. so i decided to use memcpy.
             * tokens[nr_token].str = token_content;
             */
            // in case of TK_HEX, the val will be transfer to decimal
            char* endptr;
            word_t decimal_value = strtoul(token_content, &endptr, 16);

            if(*endptr != '\0') {
              printf("Fail to transfer hexadecimal to decimal, please check you expression.");
              return false;
            }

            sprintf(token_content, "%u", decimal_value);

            memcpy(tokens[nr_token].str, token_content, sizeof(tokens[nr_token].str));
            nr_token ++;
            break;

          // Transfer $regname -> uint32_t 
          case TK_REG:
            for(int k = 0; k < substr_len; k ++) {
              token_content[k] = *token_begin;
              token_begin ++;
            }
            tokens[nr_token].type = rules[i].token_type;

            bool* success = malloc(sizeof(bool));
            *success = true;

            // in case of TK_REG, the val need to get by API.

            // Before use API, we need to remove the header charactor "$".
            if(substr_len > 0 && token_content[0] == '$') {
              memmove(token_content, token_content + 1, substr_len - 1);
              token_content[substr_len - 1] = '\0';
            }

            word_t regVal = isa_reg_str2val(token_content, success); //API of <isa.h>
            
            if(*success == false) {
              printf("Fail to get the val of reg, please check you expression.");
              return false;
            }
            
            // uint32_t -> char*[]
            sprintf(token_content, "%u", regVal);

            memcpy(tokens[nr_token].str, token_content, sizeof(tokens[nr_token].str));
            nr_token ++;
            break;

          default:
            for(int k = 0; k < substr_len; k ++){
              token_content[k] = *token_begin;
              token_begin ++;
            }
            //  add the infomation of token to tokens one by one.
            tokens[nr_token].type = rules[i].token_type;

            /* the assignment to expression with array type is not 
             * allowed in the c. so i decided to use memcpy.
             * tokens[nr_token].str = token_content;
             */
            
            memcpy(tokens[nr_token].str, token_content, sizeof(tokens[nr_token].str));

            nr_token ++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

/* The founction of check is determine whether this expression
 * is enclosed by a matching pair of parentheses.*/
bool check_parentheses(int p, int q)
{
  // the head and tail should be parenthesis.
  if (tokens[p].type != TK_LEFT || tokens[q].type != TK_RIGHT)
    return false;

  // determin whether ...
  int balance = 0;
  for (int i = p; i <= q; i++)
  {
    if (tokens[i].type == TK_LEFT)
      balance++;
    else if (tokens[i].type == TK_RIGHT)
      balance--;

    if (balance == 0)
    {
      if (i == q) return true;
      else return false;
    }
  }

  return false;
}

word_t eval(int p, int q, bool* success){
  if(! *success || p > q) {
    *success = false;
    return 0;
  }
  else if (p == q) {
    // return atoi(tokens[p].str);
    char* endptr;
    if(tokens[p].str[strlen(tokens[p].str) - 1] == 'u') {
      tokens[p].str[strlen(tokens[p].str) - 1] = '\0';
    }
    word_t singleVal = strtoul(tokens[p].str, &endptr, 10);
    if (*endptr != '\0') {
      printf("singleVal transfer fail!");
      *success = false;
      return 0;
    }
    return singleVal;
  }
  else if (check_parentheses(p, q) == true) {
    return eval(p + 1, q - 1, success);
  }
  else if (tokens[p].type == TK_DEREF || tokens[p].type == TK_NEGA) {
    switch (tokens[p].type)
    {
    case TK_DEREF:
      word_t addr_eval = eval(p + 1, q, success);
      return vaddr_read(addr_eval, 4);
      break;
    case TK_NEGA:
      return -1 * eval(p + 1, q, success);
      break;
    default:
      *success = false;
      printf("Unary operator parsing failed");
      return 0;
    }
  }
  else {
    int balance = 0;
    int op_type = 500;
    int op = 0;
    for(int i = p; i <= q; i ++) {
      if(tokens[i].type == TK_LEFT) balance ++;
      else if(tokens[i].type == TK_RIGHT) balance --;
      
      // bad expression.
      if(balance < 0) {
        *success = false;
        return 0;
      }

      // Token enclosed within a pair of  parentheses is not the main operator.
      if(balance == 0 && tokens[i].type >= 300) {
        if(tokens[i].type / 10 <= op_type / 10) {
          op = i;
          op_type = tokens[i].type;
        }
      }
    }

    word_t val1 = eval(p, op - 1, success);
    word_t val2 = eval(op + 1, q, success);

    switch (op_type)
    {
    case TK_AND:
      return val1 && val2;
    case TK_EQ:
      return val1 == val2;
    case TK_NEQ:
      return val1 != val2;
    case TK_PLUS:
      return val1 + val2;
    case TK_MINUS:
      return val1 - val2;
    case TK_MULTI:
      return val1 * val2;
    case TK_DIV:
      if (val2 == 0)
      {
        Log("%d %d", op + 1, q);
        *success = false;
        Log("!!!!! %d %d", p, q);
        return 0;
      }
      else
      {
        return val1 / val2;
      }
      default : 
        Log("%d", op_type);
        for(int i = p; i <= q; i ++) {
          Log("%s",tokens[i].str);
        }
        assert(0);
      }
    
  }

  return 0;
}

word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }

  // Determin the TK_MULTI and TK_DERFE
  for(int i = 0; i < nr_token; i ++) {
    if(tokens[i].type == TK_MULTI &&
       (i == 0 || ((tokens[i - 1].type != TK_INTEGER) && (tokens[i - 1].type != TK_HEX) && (tokens[i - 1].type != TK_RIGHT)) )) {
        tokens[i].type = TK_DEREF;
    }
    if(tokens[i].type == TK_MINUS && 
       (i == 0 || ((tokens[i - 1].type != TK_INTEGER) && (tokens[i - 1].type != TK_HEX) && (tokens[i - 1].type != TK_RIGHT)) )) {
        tokens[i].type = TK_NEGA;
       }
  }


  /* TODO: Insert codes to evaluate the expression. */
  word_t res = eval(0, nr_token - 1, success);

  return res;
}
