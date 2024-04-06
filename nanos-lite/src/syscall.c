#include <common.h>
#include "syscall.h"
#include <syscall.h>
#include <stdio.h>

// the statement of sys_XXX;

// No.0 : SYS_exit
void sys_exit(uint32_t arg1);
// No.1 : SYS_yield
int sys_yield();
// No.4 : SYS_write
int sys_wirte(int fd, intptr_t buf, size_t count);
// No.9 : SYS_brk
int sys_brk(intptr_t NewLocation);


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case 0 : {
      sys_exit(c->GPR2);
      break;
    }
    case 1 : {
      int res = sys_yield();
      c->GPRx = res;
      break;
    }
    case 4 : {
      // printf("fd : %d\n", c->GPR2);
      int res = sys_wirte(c->GPR2, c->GPR3, c->GPR4);
      c->GPRx = res;
      break;
    }
    case 9 : {
      int res = sys_brk(c->GPR2);
      c->GPRx = res;
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  // Log txt but NOT implement bc the fprintf and fopen ... NOT IN KLIB

  // #ifdef CONFIG_STRACE
  // printf("!!!!!!!!!!!!!");
  // FILE *fp = fopen("/home/hubert/ics2023/nanos-lite/Log/syscallLog.txt", "a+");
  // if (fp != NULL) {
  //   fprintf(fp, "syscall[%d]:\tparaments[%#x][%#x][%#x]\treturn[%#x]", c->GPR1, c->GPR2, c->GPR3, c->GPR4, c->GPRx);
  //   fclose(fp);
  // } else {
  //   printf("Failed to open or create syscallLog.txt.\n");
  // }
  // #endif

  // Log by just printf
  #ifdef CONFIG_STRACE
  printf("syscall[%d]:\tparaments[%d][%x][%d]\treturn[%d]\n", a[0], a[1], a[2], a[3], c->GPRx);
  #endif
}

void sys_exit(uint32_t arg1) {
  halt(arg1);
}

int sys_yield() {
  yield();
  return 0;
}

int sys_wirte(int fd, intptr_t buf, size_t count) {
  char* buffer = (char*)buf;
  if(fd == 1 || fd == 2) {
    size_t cnt;
    for(cnt = 0; cnt < count; cnt ++) {
      putch(buffer[cnt]);
    }
    return count;
  }else {
    return -1;
  }

  return 0; 
}

int sys_brk(intptr_t NewLocation) {
  return 0;
}
