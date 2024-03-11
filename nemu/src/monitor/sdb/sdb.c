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
// The include of this header file allows access to rhe "expr" function. 
#include "sdb.h"
// Add utils.h then we can edit the NEMU_.. in this C file.
#include <utils.h>
// Add debug.h to debug
#include <debug.h>
// Add API of print regs but i found this file has been included. finefinefine.
// #include <isa.h>
// Add API for vaddr_read
#include <memory/vaddr.h>


static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
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
	// fix the problem of beautiful quit / exit
	nemu_state.state = NEMU_END;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  //if have no specific figures given, the default choice is one.
  int num = (args != NULL && *args) ? atoi(args) : 1;

  // run code with the time of "num".
  for(int i = 0; i < num; i++){
    // if the program has terminated, then stop the loop. 
    if(nemu_state.state == NEMU_END) break;
    cpu_exec(1);
  }

  return 0;
}

static int cmd_info(char* args) {
  // Log("this is args, %s", args);
  // use switch-case , only single parameters 
  // if long parameters : can be replaced with if-else 
  switch (*args) {
    case 'r':
      isa_reg_display();
      break;

    case 'w':
      show_watchpoints();
      break;

    default:
      printf("No valid parameters were provided\n");
  }
  
  return 0;
}

static int cmd_x(char* args) {
  // use strtok to finish the command partition.
  char* token = strtok(args, " ");
  // get the num 
  int num = atoi(token);
  token = strtok(NULL, " ");
  // Tip : vaddr_t word_t uint32_t are equal.
  // PA1 : it is assumed that expr can only be a hexadecimal number
  // the founction of strtoul : String to unsigned long 
  vaddr_t addr_beginning = (uint32_t) strtoul(token, NULL, 16);

  for(int i = 0; i < num; i++){
    // determine the validity of the address.
    if (addr_beginning >= 0x87ffffff) break;

    // the meaning of 4 is read 4 Byte every time.
    word_t temp = vaddr_read(addr_beginning, 4);
    printf("0x%08x : 0x%08x\n", addr_beginning, temp);
    addr_beginning += 4;
  }
  
  return 0;
}

static int cmd_p(char* args) {
  bool* success = malloc(sizeof(bool));
  *success = true;

  word_t res = expr(args, success);
  
  if(*success == false) {
    printf("invaild experssion. \n");
  }else {
    printf("the result of calculate is %u \n", res);
  }

  return 0;
}

static int cmd_w(char* args) {
  new_wp(args);
  return 0;
}

static int cmd_d(char* args) {
  int No = atoi(args);
  WP* wp = search_watchpoints(No);

  if(wp == NULL) {
    printf("Fail to delete!!!");
  }
  
  return 0;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
	{ "si", "let program run N steps and N is one if have no specific figures is given", cmd_si},
  { "info", "Print the enssential infomation", cmd_info},
  { "x", "scan the memory", cmd_x},
  { "p", "print the value of experssion", cmd_p},
  { "w", "set the watchpoint", cmd_w},
  { "d", "delete the watchpoint", cmd_d},

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
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

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
