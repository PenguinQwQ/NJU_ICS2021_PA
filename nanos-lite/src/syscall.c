#include <common.h>
#include "syscall.h"

int sys_write(int fd, const char* buf,int len)
{
  int i = -1;
  if(fd == 1 || fd == 2)
  {
  
    for (i = 1 ; i <= len && (buf != NULL) && (*buf != '\0'); i++)
      putch(*buf++);
    return i;
  }
  return i;
}

int sys_brk(void *addr)
{
  return 0;//single thread always return 0 as true!
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: yield(); c->GPRx = 0; break;
    case SYS_exit:  c->GPRx = -1; halt(0); break;
    case SYS_write: c->GPRx = sys_write((int)a[1], (const char *)a[2], (int)a[3]); break;
    case SYS_brk: c->GPRx = sys_brk((void *)a[1]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
