#include <common.h>
#include <proc.h>
#include "syscall.h"

//Gettimeofday definitions
struct timeval
{
  int64_t tv_sec;
  int64_t tv_usec;
};
struct timezone {
  int tz_minuteswest;     /* minutes west of Greenwich */
  int tz_dsttime;         /* type of DST correction */
};


size_t fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static int sys_brk(void *addr)
{
  return 0;//single thread always return 0 as true!
}

static int sys_gettimeofday(struct timeval *tv, struct timezone *tz){
  assert(tv);
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = us / 1000000;
  tv->tv_usec = us % 1000000;
  return 0;
}
void naive_uload(PCB *pcb, const char *filename);
static int sys_execve(const char *fname, char * const argv[], char *const envp[]){
  naive_uload(NULL, fname);
  return -1;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: yield(); c->GPRx = 0; break;
    case SYS_exit:  c->GPRx = sys_execve("/bin/nterm",NULL,NULL); break;
    case SYS_brk: c->GPRx = sys_brk((void *)a[1]); break;
    case SYS_open:c->GPRx = fs_open((const char *)a[1], (int)a[2], (int)a[3]); break;
    case SYS_read: c->GPRx = fs_read((int)a[1], (void *)a[2], (size_t)a[3]); break;
    case SYS_write: c->GPRx = fs_write((int)a[1], (void *)a[2], (size_t)a[3]); break;
    case SYS_close: c->GPRx = fs_close((int)a[1]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_gettimeofday: c->GPRx = sys_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]); break;
     case SYS_execve: c->GPRx = sys_execve((const char *)a[1],(char * const*)a[2],(char *const*)a[3]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
