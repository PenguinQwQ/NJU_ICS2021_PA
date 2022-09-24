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
#include <memory/paddr.h>
#include <string.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NEQ, TK_AND, TK_LE, TK_LSH, TK_RSH, TK_DEREF, TK_NEG,
  TK_NUM, TK_REG, TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_LP, TK_RP
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
  {"!=", TK_NEQ},       //Not equal
  {"&&", TK_AND},       //AND
  {"<=", TK_LE},        //LE
  {">>", TK_RSH},       //RSH
  {"<<", TK_LSH},       //LSH
  {"\\-", TK_SUB},         // substract
  {"\\*", TK_MUL},         // multiply
  {"\\/", TK_DIV},         // divide
  {"\\(", TK_LP},         // left parenthesis
  {"\\)", TK_RP},         // right parenthesis
  {"[0xa-f|A-F|0-9]+", TK_NUM}, //Hex Number x...
  {"[$][$|a-z|A-Z|0-9]+", TK_REG}, //Reg Name (with $!)
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
        printf("TOKEN value is %d \n", rules[i].token_type);
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if(substr_len > 32)//this case will cause buf-overflow
         {
          printf("TK STRING OVERFLOW!!!\n");
          assert(0);
          }

        switch (rules[i].token_type) {
          case TK_NOTYPE ://is a space, just skip it and no need to record
              break;
          case TK_EQ ://is a equal symbol, so we just store the EQ val to type
              tokens[++nr_token].type = TK_EQ;
              break;
          case TK_NUM :
                tokens[++nr_token].type = TK_NUM;
                memset(tokens[nr_token].str, 0, sizeof(tokens[nr_token].str));
                strncpy(tokens[nr_token].str, substr_start, substr_len);
       //        printf("TOKEN number str is : %s \n", tokens[nr_token].str);
              break;
          case TK_REG :
                tokens[++nr_token].type = TK_REG;
                memset(tokens[nr_token].str, 0, sizeof(tokens[nr_token].str));
                strncpy(tokens[nr_token].str, substr_start, substr_len);
                printf("Reg name is %s \n", tokens[nr_token].str);
              break;
          case TK_NEQ:
                tokens[++nr_token].type = TK_NEQ;
              break;
          case TK_LE:
                tokens[++nr_token].type = TK_LE;
                break;
          case TK_LSH:
                tokens[++nr_token].type = TK_LSH;
                break;
          case TK_RSH:
                tokens[++nr_token].type = TK_RSH;
                break;
          case TK_AND:
                tokens[++nr_token].type = TK_AND;
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
      //Its Hex Number!
      if(tokens[l].type == TK_NUM && strlen(tokens[l].str) > 2 &&  tokens[l].str[0] == '0' && tokens[l].str[1] == 'x')
      {
      word_t num = 0;
      for (int i = 2 ; i <= strlen(tokens[l].str) - 1 ; i++)
        {
          num = (num << 4);
          if(tokens[l].str[i] >= '0' && tokens[l].str[i] <= '9')
            num += tokens[l].str[i] - '0';
          if(tokens[l].str[i] >= 'a' && tokens[l].str[i] <= 'f')
            num += tokens[l].str[i] - 'a' + 10;
          if(tokens[l].str[i] >= 'A' && tokens[l].str[i] <= 'F')
            num += tokens[l].str[i] - 'A' + 10;          
        }
  //    printf("token %d value is %d \n", l, num);
      return num;
      }
      //Its Decimal Number!
      if(tokens[l].type == TK_NUM && (strlen(tokens[l].str) <= 2 || tokens[l].str[1] != 'x'))
      {
      word_t num = 0;
      if(tokens[l].str[0] == '0')
        return 0;
      for (int i = 0 ; i <= strlen(tokens[l].str) - 1 ; i++)
        {
          num = (num << 3) + (num << 1) + tokens[l].str[i] - '0';
        }
  //    printf("token %d value is %d \n", l, num);
      return num;
      }
      if(tokens[l].type == TK_REG)
      {
      _Bool succ = false;
      char s[32];
      memset(s, 0, sizeof(s));
      strncpy(s, tokens[l].str + 1, strlen(tokens[l].str) - 1);
      printf("REG NAME IS: %s\n", s);
      word_t ans = isa_reg_str2val(s, &succ);
      if(succ == false)
        {
          printf("Decomposition Err!!!\n");
          ERR = true;
          assert(0);
          return 0;
        }
      return ans;
      }
      printf("ERROR!!!");
      ERR = true;
      assert(0);
      return 0;
    }
  if(check_parenthese(l, r))
    {
      return eval(l + 1, r - 1);
    }
  //find the main operator
  int in_parenthese = 0;
  int main_op_pos = r;
  int main_op_precedence = 1000, cur_op_precedence = 1000;
  for (int pos = l ; pos <= r ; pos++)
    {
        if(tokens[pos].type == TK_NUM)
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
        if(tokens[pos].type == TK_NEG || tokens[pos].type == TK_DEREF)
          {
            cur_op_precedence = 3;
          }
        if(tokens[pos].type >= TK_LSH && tokens[pos].type <= TK_RSH)
          {
            cur_op_precedence = 0;//LSH/RSH is 0 pre
          }
        if(tokens[pos].type == TK_LE)
          {
            cur_op_precedence = -1;
          }
        if(tokens[pos].type == TK_EQ || tokens[pos].type == TK_NEQ)
          {
            cur_op_precedence = -2;
          }
        if(tokens[pos].type == TK_AND)
          {
            cur_op_precedence = -3;
          }
        if(cur_op_precedence <= main_op_precedence)
            {
              main_op_pos = pos;
              main_op_precedence = cur_op_precedence;
            }
    }
  word_t ans = 0;
  if(main_op_precedence < 3)//Double Parameter Arithmetic
  {
    word_t val1, val2;
    val1 = eval(l, main_op_pos - 1);
    val2 = eval(main_op_pos + 1, r);
 //   printf("(%d,%d): %d   (%d, %d):%d   \n",l, main_op_pos - 1, val1, main_op_pos + 1, r, val2);

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
      {
        ERR = true;
        printf("DIVIDE ZERO!!!\n");
        assert(0);
      }
      else
        ans = val1 / val2;
      break;
    case TK_AND:
      ans = val1 && val2;
      break;
    case TK_EQ:
      ans = (val1 == val2);
      break;
    case TK_LE:
      ans = (val1 <= val2);
      break;
    case TK_LSH:
      ans = (val1 << val2);
      break;
    case TK_RSH:
      ans = (val1 >> val2);
      break;
    default: break;
    }
    return ans;
  }
  else //Single OP
  {
    word_t rev = -1;
    word_t cur = eval(l + 1, r);
    switch (tokens[main_op_pos].type)
    {
    case TK_NEG://TK_NEG
      ans = rev * cur;
      break;
    case TK_DEREF://TK_DIFF
      ans = paddr_read(cur, 4);
      break;
    default:
      break;
    }
    return ans;
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    printf("Invalid Expression!!!\n");
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  //Preprocess the expression
  for (int i = 1 ; i <= nr_token ; i++)
    {
      if(tokens[i].type == TK_SUB && (i == 1 || (tokens[i - 1].type != TK_REG && tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_RP)))
          tokens[i].type = TK_NEG;
      if(tokens[i].type == TK_MUL && (i == 1 || (tokens[i - 1].type != TK_REG && tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_RP)))
          tokens[i].type = TK_DEREF;  
    }
  ERR = false;//Init the ERR val
  word_t ans = eval(1, nr_token);

  return ans;
}
