#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_Shdr Elf64_Shdr
# define Elf_class ELFCLASS64
# define ELF_Off Elf64_Off
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_Shdr Elf32_Shdr
# define Elf_class ELFCLASS32
# define ELF_Off Elf32_Off
#endif

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);


#define NR_PAGE 8
#define PAGESIZE 4096
#define PG_MASK 0xfff

#define MAX(a, b)((a) > (b) ? (a) : (b))

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  if (fd == -1){ 
    printf("The file %s not found!", filename);
    assert(0); 
  }
  AddrSpace *as = &pcb->as;
  Elf_Ehdr elf_header;
  assert(fs_lseek(fd, 0, SEEK_SET) != -1);
  assert(fs_read(fd, &elf_header, sizeof(Elf_Ehdr)) == sizeof(Elf_Ehdr));

  assert(*(uint32_t *)elf_header.e_ident == 0x464c457f);

  ELF_Off phpos = elf_header.e_phoff;
  size_t phsize = elf_header.e_phentsize;
  int headers_entry_num = elf_header.e_phnum;
  for (int i = 0; i < headers_entry_num; ++i){
    Elf_Phdr phdr;
    assert(fs_lseek(fd, phpos, SEEK_SET) != -1);
    assert(fs_read(fd, &phdr, sizeof(Elf_Phdr)) == sizeof(Elf_Phdr));
    phpos += phsize;

    if(phdr.p_type == PT_LOAD) {
    void *p_addr;
      uintptr_t v_addr = phdr.p_vaddr;
      size_t page_num = ((v_addr + phdr.p_memsz - 1) >> 12) - (v_addr >> 12) + 1;
      void *page_start = new_page(page_num);
      for (int i = 0; i < page_num; ++i)
          map(as, (void *)((v_addr & ~PG_MASK) + i * PAGESIZE), (void *)(page_start + i * PAGESIZE), 1);
      p_addr = page_start;
      assert(fs_lseek(fd, phdr.p_offset, SEEK_SET) != -1);
      uint32_t filesz = phdr.p_filesz, memsz = phdr.p_memsz;
      assert(filesz <= memsz);
      assert(fs_read(fd, p_addr + (v_addr & PG_MASK), filesz) == filesz);
      memset(p_addr + (v_addr & PG_MASK) + filesz, 0, memsz - filesz);
      if (filesz < memsz){// 应该是.bss节
        pcb->max_brk = ROUNDUP(v_addr + memsz, PG_MASK);
      }
    }
  }
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
  assert(0);
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg){
  Area karea;
  karea.start = &pcb->cp;
  karea.end = &pcb->cp + STACK_SIZE;
  pcb->cp = kcontext(karea, entry, arg);
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  AddrSpace *as = &pcb->as;
  protect(as);
  void *alloced_page = new_page(NR_PAGE) + NR_PAGE * 4096; //得到栈顶

  //这段代码有古怪，一动就会出问题，莫动
  //这个问题确实已经被修正了，TMD，真cao dan
  // 2021/12/16
  
  map(as, as->area.end - 8 * PAGESIZE, alloced_page - 8 * PAGESIZE, 1); 
  map(as, as->area.end - 7 * PAGESIZE, alloced_page - 7 * PAGESIZE, 1);
  map(as, as->area.end - 6 * PAGESIZE, alloced_page - 6 * PAGESIZE, 1); 
  map(as, as->area.end - 5 * PAGESIZE, alloced_page - 5 * PAGESIZE, 1);
  map(as, as->area.end - 4 * PAGESIZE, alloced_page - 4 * PAGESIZE, 1); 
  map(as, as->area.end - 3 * PAGESIZE, alloced_page - 3 * PAGESIZE, 1);
  map(as, as->area.end - 2 * PAGESIZE, alloced_page - 2 * PAGESIZE, 1); 
  map(as, as->area.end - 1 * PAGESIZE, alloced_page - 1 * PAGESIZE, 1); 
  
  char *brk = (char *)(alloced_page - 4);
  intptr_t *ptr_brk = (intptr_t *)(brk);
  //这条操作会把参数的内存空间扬了，要放在最后
  uintptr_t entry = loader(pcb, filename);
  Area karea;
  karea.start = &pcb->cp;
  karea.end = &pcb->cp + STACK_SIZE;

  Context* context = ucontext(as, karea, (void *)entry);
  pcb->cp = context;
  ptr_brk -= 1;
  *ptr_brk = 0;//为了t0_buffer
  //设置了sp
  context->gpr[2]  = (uintptr_t)ptr_brk - (uintptr_t)alloced_page + (uintptr_t)as->area.end;
}
