#include <common.h>
#include "syscall.h"
#include <syscall.h>

// the statement of sys_XXX;

// No.0 : SYS_exit
void sys_exit();
// No.1 : SYS_yield
int sys_yield();
// 

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

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
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

int sys_yield() {
  yield();
  return 0;
}

void sys_exit(uint32_t arg1) {
  halt(arg1);
}
