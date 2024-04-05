#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  // test the Context
  // printf("General Purpose Registers:\n");
  // for (int i = 0; i < NR_REGS; i++) {
  //     printf("gpr[%d]: %x\n", i, c->gpr[i]);
  // }
  // printf("mcause: %x\n", c->mcause);
  // printf("mstatus: %x\n", c->mstatus);
  // printf("mepc: %x\n", c->mepc);
  // printf("pdir: %p\n", c->pdir);

  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0xb : {
        c->mepc += 4;
        if(c->GPR1 == -1) {
          ev.event = EVENT_YIELD;
          break;
        }else if(c->GPR1 >= 0 && c->GPR1 <=19) {
          ev.event = EVENT_SYSCALL;
          break;
        }else{
          printf("Match Fail!\n");
        }
      }
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
