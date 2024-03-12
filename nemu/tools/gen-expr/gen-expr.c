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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

uint32_t choose(uint32_t n) {
  return rand() % n;
}

static void gen(char c) {
  size_t len = strlen(buf);
  if(len < sizeof(buf) - 1) {
    buf[len] = c;
    buf[len + 1] = '\0';
  }
}

// static void gen_num() {
//   int randomDigit = rand() % 10;
//   char digitChar = '0' + randomDigit;
//   gen(digitChar);
// }

static void gen_num() {
  int randomNum = rand() % 1000000;
  char numBuffer[8] = {};
  sprintf(numBuffer, "%d", randomNum);

  for(int i = 0; numBuffer[i] != '\0'; i++) {
    gen(numBuffer[i]);
  }

  gen('u');
}

static void gen_rand_op() {
  char operators[] = {'+', '-', '*', '/'};
  gen(operators[choose(4)]);
}

static void gen_rand_expr(int depth, int maxDepth) {
  if(depth >= maxDepth) {
    gen_num();
    return;
  }

  switch (choose(3)) {
    case 0:
      gen_num(); 
      break;

    case 1: 
      gen('('); 
      gen_rand_expr(depth + 1, maxDepth); 
      gen(')');break;

    // case 2: 
    //   gen(' ');
    //   break;

    default :
      gen_rand_expr(depth + 1, maxDepth);
      gen_rand_op();
      gen_rand_expr(depth + 1, maxDepth);
      break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    // init the buf.
    buf[0] = '\0';

    gen_rand_expr(0, 10);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -pthread -Wall -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
