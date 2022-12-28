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


extern PCB *current;
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  uintptr_t max_page_end = current->max_brk; 
  uintptr_t max_page_pn = (max_page_end >> 12) - 1;
  uintptr_t brk_pn = brk >> 12;//12
  uint32_t  alloc_pg_num = brk_pn - max_page_pn + 1;

  #define PG_MASK 0xfff
  //[page_start, page_end) 所以，应该是大于等于就重新分配
  //printf("申请内存 max_page_end: %p\t to brk: %p\n", max_page_end, brk);//分配从[max_page_pn, brk_pn]的所有页
  if (brk >= max_page_end){
    void *allocted_page =  new_page(brk_pn - max_page_pn + 1);//需要申请的新的页数
    while(alloc_pg_num--)
    {
      map(&current->as, (void *)(max_page_end), (void *)(allocted_page), 1);
      max_page_end += PG_MASK;
      allocted_page += PG_MASK;
    }

    current->max_brk = (brk_pn + 1) << 12;
    assert(current->max_brk > brk);
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
