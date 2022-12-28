#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);
  printf("kas.ptr addr: %p\n", kas.ptr);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  if (c->pdir != NULL){ //自行添加
    //printf("在__am_get_cur_as中设置为由%p，地址为%p，更改为，", c->pdir, &c->pdir);
    c->pdir = (vme_enable ? (void *)get_satp() : NULL);
    //printf("%p\n", c->pdir);
  }
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    //printf("在__am_switch中设置satp为%p\n", c->pdir);
    set_satp(c->pdir);
  }
}

#define OFFSET 12
#define PG_SIZE 4096
#define MAP_MASK ~0xfff
static uint32_t V_PGN_MASK1 = 0x003FF000u, V_PGN_MASK2 = 0xFFC00000u, PTE_MASK = 0xFFFFFC00u;
static uint32_t V_PGN_1(uint32_t x)
{
  return (x & V_PGN_MASK1) >> OFFSET;
}
static uint32_t V_PGN_2(uint32_t x)
{
  return (x & V_PGN_MASK2) >> (OFFSET + 10);
}
static uint32_t PTE_PGN(uint32_t x)
{
  return (x & PTE_MASK) >> 10;
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  va = (void *)(((uint32_t)va) & MAP_MASK);
  pa = (void *)(((uint32_t)pa) & MAP_MASK);

  PTE *page_table_entry = as->ptr;
  page_table_entry += V_PGN_2((uint32_t)va) * sizeof(uint32_t);

  if ((*page_table_entry & PTE_V) == false){ // 说明二级表未分配
    void *alloced_page = pgalloc_usr(PGSIZE);
    *page_table_entry &= ~PTE_MASK;
    *page_table_entry |= (PTE_MASK & ((uint32_t)alloced_page >> 2));
    *page_table_entry |=  PTE_V;
  }
  // 找到二级表中的表项
  PTE *leaf_page_table_entry = (PTE *)(PTE_PGN(*page_table_entry) * PG_SIZE + V_PGN_1((uint32_t)va) * sizeof(uint32_t));
  *leaf_page_table_entry = (PTE_MASK & ((uint32_t)pa >> 2));
  *leaf_page_table_entry |= (PTE_V | PTE_R | PTE_W | PTE_X);
  *leaf_page_table_entry |= (prot ? PTE_U : 0);
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry)
{
  Context *ctx = (Context *)((uint8_t *)(kstack.end) - sizeof(Context));
  memset(ctx, 0, sizeof(ctx));
  ctx->mepc = (uintptr_t)entry;
  ctx->mstatus = 0x1800 | 0x80;
  ctx->gpr[0] = 0;
  ctx->mscratch = (uintptr_t)kstack.end;
  //ctx->GPRx = (uintptr_t)(heap.end);
  // printf("the heap end is %p\n", heap.end);

  return ctx;
}
