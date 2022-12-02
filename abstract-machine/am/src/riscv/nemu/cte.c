#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 11:
              if(c->GPR1 == -1) ev.event = EVENT_YIELD;
              if(c->GPR1 >= 0 && c->GPR1 <= 19) ev.event = EVENT_SYSCALL;
              c->mepc += 4;
              break;
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

Context *kcontext(Area kstack, void (*entry)(void *), void *arg)
{
  // return NULL;
  Context *ctx = (Context *)((uint8_t *)(kstack.end) - sizeof(Context));
  memset(ctx, 0, sizeof(ctx));

  ctx->gpr[0] = 0;
  ctx->mepc = (uintptr_t)entry;
  ctx->mstatus = 0x1800 | 0x80;
  ctx->GPRx = (uintptr_t)arg;
  // ctx->pdir = NULL;
  ctx->mscratch = 0;
  // printf("args is %d\n", *((int *)ctx->gpr[10]));
  // while (1)
  // {
  //   /* code */
  // }

  // printf("ctx gpr 0 is %d\n", ctx->gpr[0]);
  return ctx;
}

void yield() { // From the assembly code, we know that Yield Event will fill the a7 with -1!!! 
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
