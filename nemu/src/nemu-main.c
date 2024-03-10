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
// include this file to use FILE
#include <stdio.h>
#include "./monitor/sdb/sdb.h"

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  // // test the calculator

  // FILE *file = fopen("/home/hubert/ics2023/nemu/src/input", "r");
  // if(file == NULL) {
  //   perror("Error opening file");
  //   return 0;
  // }

  // char line[10000];
  // int cnt = 0;
  // int line_num = 0;
  // while(fgets(line, sizeof(line), file) != NULL) {
  //   line_num ++;
  //   word_t result = 0;
  //   char experssionFromFile[10000];
  //   sscanf(line, "%d %s", &result, experssionFromFile);

  //   bool *suc = malloc(sizeof(bool));
  //   *suc = true;
  //   word_t cal = expr(experssionFromFile, suc);
  //   if(*suc == false){
  //     // Log("%d", line_num);
  //     // assert(0);
  //     cnt ++;
  //     continue;
  //   }else{
  //     if(cal == result) continue;
  //     else {
  //       cnt ++;
  //       continue;
  //     }
  //   }
  // }
  // Log("%d",cnt);
  

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
