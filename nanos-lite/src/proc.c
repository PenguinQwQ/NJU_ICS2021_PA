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
Context *context_uload(PCB *pcb, char *filename, char *const argv[], char *const envp[]);
uintptr_t p1, p2;
uint32_t up1 = 1, up2 = 2;

void init_proc() {
  p1 = (uintptr_t)&up1;
  p2 = (uintptr_t)&up2;
//  pcb[0].cp = context_kload(&pcb[0], (void *)hello_fun, (void *)p1);
  pcb[0].cp = context_uload(&pcb[0], "/bin/dummy", NULL, NULL);
  switch_boot_pcb();
  Log("Initializing processes...");
//  naive_uload(NULL, "/bin/menu");
  // load program here
}

Context *schedule(Context *prev)
{
  //save the current pcb context pointer
  current->cp = prev;
  //Load the pcb[0] process
  current =  &pcb[0];
//  current = (current == &pcb[0]) ? (&pcb[1]) : (&pcb[0]);
  //Return the current context pointer
  return current->cp;
}

int execve(const char *pathname, char *const argv[], char *const envp[])
{
  assert(0);
  // current->cp = pcb[0].cp;//context_kload(&pcb[0], (void *)hello_fun, NULL);
  yield();
  return 0;
}

void exit(int status)
{
  assert(0);
  if (status == 0)
  {
    execve("/bin/nterm", NULL, NULL);
  }
  else
    halt(status);
}
