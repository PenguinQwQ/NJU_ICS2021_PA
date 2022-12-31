#include <memory.h>
#include <stdint.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *old_pf = pf;
  pf = pf + nr_page * 4 * 1024;
  return old_pf;
}

#ifdef HAS_VME
#define PG_SIZE 4096
static void* pg_alloc(int n) {
  assert(n % PG_SIZE == 0);
  void *p = new_page(n / PG_SIZE);
  memset(p, 0, n);
  return p;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}


#define PG_MASK 0xfff
extern PCB *current;
int mm_brk(uintptr_t brk) {
  static uint32_t OFFSET = 12;
  uint32_t  alloc_pg_num = (brk >> OFFSET) - (current->max_brk >> OFFSET) + 2;  

  if (current->max_brk <= brk){
    void *alloc_page =  new_page((brk >> OFFSET) - (current->max_brk >> OFFSET) + 2);
    while(alloc_pg_num--)
    {
      map(&current->as, (void *)(current->max_brk), (void *)(alloc_page), 1);
      current->max_brk += PG_MASK;
      alloc_page += PG_MASK;
    }

    current->max_brk = brk + PG_SIZE;
    assert(current->max_brk > brk);
    return 0;
  }

  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
