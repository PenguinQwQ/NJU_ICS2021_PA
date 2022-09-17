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

#include <common.h>

void init_monitor(int, char *[]);//init the monitor and call sdb
void am_init_monitor();
void engine_start();
int is_exit_status_bad();//judge the status of the nemu

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();//If the config target is abstrac-machine
  //then init the monitor in am mod
  //In fact, the generated file haven't defined this macro!
  //So it will enter the next init_monitor(?
#else
  init_monitor(argc, argv);//
#endif

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
