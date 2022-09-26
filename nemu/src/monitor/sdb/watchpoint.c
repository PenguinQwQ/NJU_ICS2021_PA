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

#include "sdb.h"

#define NR_WP 32 //define the size of the watchpoints

static int WP_NUM = 0;
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;//two lists of WP

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }
  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp()
{
   WP* ptr = free_;
   WP* prev = ptr;
   if(ptr == NULL)//No empty wp exists!
    {
      assert(0);
      return NULL;
    }
   WP_NUM++;
   while(ptr->next != NULL)
   {
    prev = ptr;
    ptr = ptr->next;
   }
   memset(ptr->str, 0, sizeof(ptr->str));
   ptr->NO = WP_NUM;
   //ptr, the last; while prev the second last!
   WP* cur = head;
   //Insert the ptr into the head list.
   if(head == NULL)
    head = cur;
   else
   {
    while(cur->next != NULL)
      cur = cur->next;
    cur->next = ptr;
   }


   if(prev->next == ptr) //The Empty is the last
   {
      prev->next = NULL;
      return ptr;
   }
   //Only One WP* exists!
   if(prev == ptr && ptr == free_)
   {
      free_ = NULL;
      return ptr;
   }
   return NULL;
}

WP* find_NO_watchpoint(int num)
{
  WP* ptr = head;
  while(ptr != NULL)
  {
    if(ptr->NO == num)
      return ptr;
    ptr = ptr->next;
  }
  return NULL;
}

void free_wp(WP *wp)
{
  WP *prev = head;
  while(prev->next != wp)
    prev = prev->next;
  prev->next = wp->next;
  wp->NO = 0;
  wp->next = NULL;
  WP *ptr = free_;
  if(ptr == NULL) //free list doesn't exist
    {
      free_ = wp;
      return;
    }
  while(ptr->next != NULL)
      ptr = ptr->next;
  ptr->next = wp;//add wp to the tail!
  return;
}

void scan_watchpoints() //pause when variables changes!
{
  WP* ptr = head;
  while(ptr != NULL)
    {
      bool succ = false;
      word_t val = expr(ptr->str, &succ);
      if(succ == false)
        {
          assert(0);
          printf("Invalid expression or Err Arithmetic!!\n");
          return;
        }
      if(val != ptr->prev_val)
      {
        printf("The watchpoint NO. %d is changed\n With expression %s \n Prev_val is %u and Now_val is %u \n", ptr->NO,  ptr->str, ptr->prev_val, val);
        nemu_state.state = NEMU_STOP;
      }
    }
  if(nemu_state.state == NEMU_STOP)
    {
      printf("The watchpoint is triggered! Please check them out!\n");
    }
}

void display_watchpoints()
{
  WP* ptr = head;
  while(ptr != NULL)
    {
      printf("The watchpoint NO is : %d\n", ptr->NO);
      printf("Expression stored: %s \n", ptr->str);
      printf("The value of Expression : %u\n", ptr->prev_val);
      ptr = ptr->next;
    }
  return;
}
