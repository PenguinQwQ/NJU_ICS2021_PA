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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
//we read one command only, so static is available
  static char *line_read = NULL;//set the value to NULL

  if (line_read) {
    free(line_read);//here we free the malloced memory
    line_read = NULL;//erase the value
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args)
{
//Here we still need to extract detailed parameters to exec certain
//instructions
//args is a char* string. In cmd_si, there is probably only one argument
//We will get n or set n = 1

  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {//If no argument is given, then print all
    /* no argument given */
    cpu_exec(1);
  }
  else {//else, print the selected name
    int t = 0, len = 0;
    while(len < strlen(arg))
    {
      t = (t << 3) + (t << 1) + arg[len] - '0';
      len++;
    }
    cpu_exec(t);
  }
  return 0;
}
/*
static int cmd_p(char *args);

static int cmd_x(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static int cmd_info(char *args);
*/
static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);//Defined a 
  //pointer, which points to a function, as its input is
  //a string
  //handler is used to execute certain effects according
  //to the arguments
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
   /* TODO: Add more commands */
  { "si", "Execute the procedure's next n instructions and pause, and n is set to 1 once not given", cmd_si}
/*
  { "info", "Print the information of the procedure(info of registers/watchpoints)", cmd_info},
  { "x", "Scan the memory, give consistent N bytes memory content", cmd_x},
  { "p", "print the arithmetic result of given expression", cmd_p},
  { "w", "set watchpoints", cmd_w},
  { "d", "delete watchpoints", cmd_d}
  */
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {//If no argument is given, then print all
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {//else, print the selected name
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }
  //each time, from the stdinstream, we read a single 
  //line as a command string

  //the sdb terminal runs in a single "for" loop
  //during the for loop, we input commands and args
  //to control the abstract machine to run
  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);//get the end 
    //of the string

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }
    //NULL command, then continue;
    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    //get the pointer/address of the arguments
    //and the parse them
    if (args >= str_end) {
      args = NULL;//if there is no arguments,set the args
      //to null
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif
  //compare the cmd we extract to the cmd_table
  //and select the suitable command as the input
  //once we find the proper cmd(which derived from strcmp)
  //we put the args into the function and get a receive

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {//we find the cmd in the table!
        if (cmd_table[i].handler(args) < 0) { return; }//once the return value < 0, then its quit and return to 
        break;
      }
    }
  //if we searched all the commands and find nothing
  //then print the "Unknow command"
    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
