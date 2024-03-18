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

#define NR_WP 32

// thie definiton has been enclosed by sdb.h
// typedef struct watchpoint {} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

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

// Awakening a watchpoint from the watchpoint pool.
WP* new_wp(char* args) {
  if(free_ == NULL) {
    printf("No free watchpoints exist!");
    assert(0);
  }

  WP* res = free_;
  free_ = free_ -> next;

  res -> next = head;
  head = res;

  // save the expression to res.
  res -> expression = malloc(strlen(args) + 1);
  strcpy(res->expression, args);
  Log("%s", res->expression);

  // Initialize the value to the current value of the expression
  bool success = true;
  res->value = expr(res->expression, &success);

  if (!success)
  {
    Log("Error initializing watchpoint value.");
    assert(0);
  }

  return res;
}


void free_wp(WP *wp) {
  if(wp == head) {
    head = wp -> next;
  }else {
    WP *prev = head;
    while(prev != NULL && prev -> next != wp) {
      prev = prev -> next;
    }
    if(prev != NULL) {
      prev -> next = wp -> next;
    }
  }

  free(wp -> expression);

  wp -> next = free_;
  free_ = wp;
}

void scan_watchpoints() {
  WP *wp = head;
  while(wp != NULL) {
    bool success = true;
    word_t new_value = expr(wp->expression, &success);
    
    if(!success) {
      printf("Error evaluating watchpoint expression.");
      assert(0);
    }

    if(new_value != wp -> value) {
      nemu_state.state = NEMU_STOP;
      printf("Watchpoint %d triggered! Expression: %s\n", wp->NO, wp->expression);
      printf("Old Value: %u, New Value: %u\n", wp->value, new_value);
      wp -> value = new_value;
    }

    wp = wp -> next;
  }
}

void show_watchpoints() {
  printf("%-5s%-15s%-20s\n", "Num", "Expression", "Current Value");

  WP *wp = head;
  while (wp != NULL) {
    printf("%-5d%-15s%-20u\n", wp->NO, wp->expression, wp->value);

    wp = wp->next;
  }
}

WP* search_watchpoints(int target) {
  WP* wp = head;

  while(wp != NULL) {
    if(wp -> NO == target) {
      return wp;
    }

    wp = wp -> next;
  }

  printf("Fail to search the target watchpoint, please check your command\n");
  return NULL;
}
