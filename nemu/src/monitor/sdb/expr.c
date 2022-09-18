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
#include <string.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NUMBER, TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_LP, TK_RP
};

static struct rule {
  const char *regex;//This is the regex rule string, which is used to constrain the expr
  int token_type;//This is the token_type (Can be represented as A Int)
} rules[] = {
/*
How to ensure the token's precedence?


*/
  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
//Here we used
  {" +", TK_NOTYPE},    // spaces, should be igonred. " +" means consistent >= 1 spaces
  //should be ignored
  {"\\+", TK_ADD},         // plus
  {"==", TK_EQ},        // equal
  {"\\-", TK_SUB},         // substract
  {"\\*", TK_MUL},         // multiply
  {"\\/", TK_DIV},         // divide
  {"\\(", TK_LP},         // left parenthesis
  {"\\)", TK_RP},         // right parenthesis
  {"[1-9][0-9]*", TK_NUMBER }   // Number
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
//Here we init all the regex and compile them!
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

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
static bool ERR = false;//ERR is used to indicate the wrong
//happened during calculation

//input a regular string as expression to be made token
static bool make_token(char *e) {
  int position = 0;//the index of string
  int i;//
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
   //     printf("TOKEN value is %d \n", rules[i].token_type);
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
          case TK_NOTYPE ://is a space, just skip it and no need to record
              break;
          case TK_EQ ://is a equal symbol, so we just store the EQ val to type
              tokens[++nr_token].type = TK_EQ;
              break;
          case TK_NUMBER :
              if(substr_len > 32)//this case will cause buf-overflow
                assert(0);
              else
              {
                tokens[++nr_token].type = TK_NUMBER;
  //              if(strlen(tokens[nr_token].str) + substr_len >= 32)
            
                strncpy(tokens[nr_token].str, substr_start, substr_len);
                printf("TOKEN number str is : %s \n", tokens[nr_token].str);
              }
              break;
          case TK_ADD :
              tokens[++nr_token].type = TK_ADD;
              break;
          case TK_SUB :
              tokens[++nr_token].type = TK_SUB;
              break;
          case TK_MUL :
              tokens[++nr_token].type = TK_MUL;
              break;
          case TK_DIV :
              tokens[++nr_token].type = TK_DIV;
              break;
          case TK_LP :
              tokens[++nr_token].type = TK_LP;
              break;
          case TK_RP :
              tokens[++nr_token].type = TK_RP;
              break;
          default: 
              break;
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
//l,r are the token indices
bool check_parenthese(int l, int r)
{
  //if and only if the t[l], t[r] is LP, RP and the inner content follows the rule of parenthese match!
  if((tokens[l].type != TK_LP) || (tokens[r].type != TK_RP))
      return false;
  int stop = 0;
  for (int pos = l + 1 ; pos <= r - 1 ; pos++)
    {
      if(tokens[pos].type == TK_LP)
        stop++;
      if(tokens[pos].type == TK_RP)
        stop--;
      if(stop < 0)
        return false;//not match as ')' is more than '('
    }
  if(stop != 0)
    return false; //not match as '(' != ')'
  return true;
}
word_t eval(int l, int r)
{
  if(ERR)
    return 0;//Err occurs, stop the procedure!
  if(l > r)
    {
      ERR = true;
      return 0;
    }
  if(l == r)
    {
      if(tokens[l].type != TK_NUMBER)
        {
          ERR = true;
          return 0;
        }
      word_t num = 0;
      for (int i = 0 ; i <= strlen(tokens[l].str) - 1 ; i++)
        {
          num = (num << 3) + (num << 1) + tokens[l].str[i] - '0';
        }
      return num;
    }
    if(tokens[l].type == TK_SUB) //means its neg
    {
      word_t rev = -1;
      word_t cur = eval(l + 1, r);
      return cur * rev;
    }
  if(check_parenthese(l, r))
    {
      return eval(l + 1, r - 1);
    }
  //find the main operator
  int in_parenthese = 0;
  int main_op_pos = l;
  int main_op_precedence = 1000, cur_op_precedence = 1000;
  for (int pos = l ; pos <= r ; pos++)
    {
        if(tokens[pos].type == TK_NUMBER)
          continue;
        if(tokens[pos].type == TK_RP)
          {
            in_parenthese--;
            continue;
          }
        if(tokens[pos].type == TK_LP)
          {
            in_parenthese++;
            continue;
          }
        if(in_parenthese < 0)//Err parenthese occurs!
          {
            ERR = true;
            return 0;
          }
        if(in_parenthese > 0)//In parenthese, not to find!
          {
            continue;
          }
        //Here musn't in parenthese
        if(tokens[pos].type >= TK_ADD && tokens[pos].type <= TK_SUB)
          {
            cur_op_precedence = 1;
          }
        if(tokens[pos].type >= TK_MUL && tokens[pos].type <= TK_DIV)
          {
            cur_op_precedence = 2;
          }
        if(cur_op_precedence < main_op_precedence)
            {
              main_op_pos = pos;
              main_op_precedence = cur_op_precedence;
            }
    }
    word_t val1, val2, ans = 0;
    val1 = eval(l, main_op_pos - 1);
    val2 = eval(main_op_pos + 1, r);
    switch (tokens[main_op_pos].type)
    {
    case TK_ADD:
      ans = val1 + val2;
      break;
    case TK_SUB:
      ans = val1 - val2;
      break;
    case TK_MUL:
      ans = val1 * val2;
      break;
    case TK_DIV:
      if(val2 == 0)
        ERR = true;
      else
        ans = val1 / val2;
      break;
    default: assert(0);
    }
  return ans;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    printf("Invalid Expression!!!\n");
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  ERR = false;//Init the ERR val
  word_t ans = eval(1, nr_token);
  return ans;
}
