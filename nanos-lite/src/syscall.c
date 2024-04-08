#include <common.h>
#include "syscall.h"
#include <syscall.h>
#include <stdio.h>
#include <fs.h>
#include <config.h>
#include <sys/time.h>

// the statement of sys_XXX;

// No.0 : SYS_exit
void sys_exit(uint32_t arg1);
// No.1 : SYS_yield
int sys_yield();
// No.2 : SYS_open
int sys_open(const char * pathname, int flags, int mode);
// No.3 : SYS_read
int sys_read(int fd, void* buf, size_t count);
// No.4 : SYS_write
int sys_write(int fd, intptr_t buf, size_t count);
// No.7 : SYS_close
int sys_close(int fd);
// No.8 : SYS_lseek
off_t sys_lseek(int fd, off_t offset, int whence);
// No.9 : SYS_brk
int sys_brk(intptr_t NewLocation);
// No. 19 : SYS_gettimeofday
int sys_gettimeofday(struct timeval *tv, struct timezone *tz);

const char* syscalls_name[] = {
  "SYS_exit",
  "SYS_yield",
  "SYS_open",
  "SYS_read",
  "SYS_write",
  "",
  "",
  "SYS_close",
  "SYS_lseek",
  "SYS_brk", // 9
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "SYS_gettimeofday"
};

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
      c->GPRx = sys_yield();
      break;
    }
    case 2 : {
      c->GPRx = sys_open((const char*)a[1], a[2], a[3]);
      break;
    }
    case 3 : {
      c->GPRx = sys_read(a[1], (void*)a[2], a[3]);
      break;
    }
    case 4 : {
      // printf("fd : %d\n", c->GPR2);
      c->GPRx = sys_write(a[1], a[2], a[3]);
      break;
    }
    case 7 : {
      c->GPRx = sys_close(a[1]);
      break;
    }
    case 8 : {
      c->GPRx = sys_lseek(a[1], a[2], a[3]);
      break;
    }
    case 9 : {
      int res = sys_brk(a[1]);
      c->GPRx = res;
      break;
    }
    case 19 : {
      c->GPRx = sys_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]);
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
  // #ifdef CONFIG_STRACE
  // printf("syscall[%s]:\tparaments[%d][%x][%d]\treturn[%d]\n", syscalls_name[a[0]], a[1], a[2], a[3], c->GPRx);
  // #endif
}

void sys_exit(uint32_t arg1) {
  halt(arg1);
}

int sys_yield() {
  yield();
  return 0;
}

int sys_write(int fd, intptr_t buf, size_t count) {
  char* buffer = (char*)buf;

  if(fd == 1 || fd == 2) {
    size_t cnt;
    for(cnt = 0; cnt < count; cnt ++) {
      putch(buffer[cnt]);
    }
    return cnt;
  }else if (fd > 2) {
    return fs_write(fd, (const char*)buf, count);
  }

  return -1; 
}

int sys_brk(intptr_t NewLocation) {
  return 0;
}

int sys_open(const char * pathname, int flags, int mode) {
  return fs_open(pathname, flags, mode);
}

int sys_close(int fd) {
  return fs_close(fd);
}

off_t sys_lseek(int fd, off_t offset, int whence) {
  // Log("fd: %d", fd);
  return fs_lseek(fd, offset, whence);
}

int sys_read(int fd, void* buf, size_t count) {
  int res = fs_read(fd, buf, count);
  return res;
}

int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec =  us / (int)1e6;
  tv->tv_usec = us % (int)1e6;

  return 0;
}
