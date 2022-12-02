#include <proc.h>
#include <fs.h>

#define MAX_NR_PROC 4



size_t fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

uint32_t NR_PROC = 0;

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void naive_uload(PCB *pcb, const char *filename);
Context *context_kload(PCB *pcb, void (*entry)(void *), void *arg);

 

void init_proc() {
  switch_boot_pcb();
  Log("Initializing processes...");
  naive_uload(NULL, "/bin/menu");
  pcb[0].cp = context_kload(&pcb[0], (void *)hello_fun, NULL);
  
  // load program here
}

Context *schedule(Context *prev)
{
  current->cp = prev;
  current =  &pcb[0];
  return current->cp;
}

int execve(const char *pathname, char *const argv[], char *const envp[])
{
  int fd = fs_open(pathname, 0, 0);
  if (fd == -1)
  {
    return -1;
  }
  else
    fs_close(fd);
  current->cp = pcb[0].cp;//context_kload(&pcb[0], (void *)hello_fun, NULL);
  switch_boot_pcb();
  yield();
  return 0;
}

void exit(int status)
{
  panic("in exit!");
  if (status == 0)
  {
    execve("/bin/nterm", NULL, NULL);
  }
  else
    halt(status);
}
