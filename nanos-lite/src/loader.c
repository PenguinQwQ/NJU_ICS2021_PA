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

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
# define EXPECT_TYPE EM_386
#elif defined(__ISA_MIPS32__)
# define EXPECT_TYPE EM_MIPS_X
#elif defined(__ISA_RISCV32__)
# define EXPECT_TYPE EM_RISCV
#elif defined(__ISA_RISCV64__)
# define EXPECT_TYPE EM_RISCV
#else
# error Unsupported ISA
#endif



int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);


#define NR_PAGE 8
#define PG_MASK 0xfff
#define PG_SIZE 4096
#define MAX(a, b)((a) > (b) ? (a) : (b))

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  if (fd == -1){ 
    printf("The file %s not found!", filename);
    assert(0); 
  }
  Elf_Ehdr ehdr;
  assert(fs_lseek(fd, 0, SEEK_SET) != -1);
  assert(fs_read(fd, &ehdr, sizeof(Elf_Ehdr)) == sizeof(Elf_Ehdr));

  //Check the elf file and its property!
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  assert(ehdr.e_machine == EXPECT_TYPE);
  assert(ehdr.e_type == ET_EXEC);
  assert(ehdr.e_version == EV_CURRENT);
  assert(ehdr.e_ident[EI_DATA] == ELFDATA2LSB);
  assert(ehdr.e_ident[EI_CLASS] == Elf_class);

  //Analysis the elf file!
  ELF_Off phpos = ehdr.e_phoff;
  size_t phsize = ehdr.e_phentsize;
  int cnt = ehdr.e_phnum;
  for (int i = 0; i < cnt; ++i){
    Elf_Phdr phdr;
    assert(fs_lseek(fd, phpos, SEEK_SET) != -1);
    assert(fs_read(fd, &phdr, sizeof(Elf_Phdr)) == sizeof(Elf_Phdr));
    phpos += phsize;
    //Extract the p_type, load the PT_LOAD program header!
    if(phdr.p_type == PT_LOAD) 
    {
      //double check!
      assert(phdr.p_type == PT_LOAD);
      void *p_addr;
      uintptr_t v_addr = phdr.p_vaddr;
      //get pg_num
      size_t pg_num = ((v_addr + phdr.p_memsz - 1) >> 12) - (v_addr >> 12) + 1;
      //alloc new pg
      void *pg_st = new_page(pg_num);
      uintptr_t pg_off = 0, pg_base = (v_addr & (~PG_MASK)), v_off = (v_addr & PG_MASK);
      while(pg_num--)
      { 
          map(&pcb->as, (void *)(pg_base + pg_off), (void *)(pg_st + pg_off), 1);
          pg_off += PG_SIZE;
      }
      p_addr = pg_st;
      assert(fs_lseek(fd, phdr.p_offset, SEEK_SET) != -1);
      uint32_t filesz = phdr.p_filesz, memsz = phdr.p_memsz;
      assert(fs_read(fd, p_addr + v_off, filesz) == filesz);
      assert(filesz <= memsz);
      memset(p_addr + v_off + filesz, 0, memsz - filesz);
      //.bss section should round up!
      if (filesz < memsz)
        pcb->max_brk = ROUNDUP(v_addr + memsz, PG_MASK);
    }
  }
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
  assert(0);
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg){
  Area kstack = RANGE(pcb, (char *)pcb + STACK_SIZE);
  Context* context = kcontext(kstack, entry, arg);
  pcb->cp = context;
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  protect(&pcb->as);
  void *alloc_page = new_page(NR_PAGE) + NR_PAGE * PG_SIZE; 
  int argc = 0, envc = 0;

  while(argv != NULL && argv[argc] != NULL) argc++;
  char *(args[argc]);
  #ifdef HAS_VME
  for (int i = 8 ; i >= 1 ; i--)
      map(&pcb->as, &pcb->as.area.end - i * PG_SIZE, alloc_page - i * PG_SIZE, 1); 
  #endif
  char *brk = (char *)(alloc_page - 4);

  for (int i = 0; i < argc; ++i)
  {
    brk -= (strlen(argv[i]) + 1);
    strcpy(brk, argv[i]);
    args[i] = brk;
  }

  while(envp != NULL && envp[envc] != NULL) envc++;
  char *(envs[envc]);
  for (int i = 0; i < envc; ++i)
  {
    brk -= (strlen(envp[i]) + 1);
    strcpy(brk, envp[i]);
    envs[i] = brk;
  }

  char **sp_2 = (char **)brk;
  --sp_2;
  *sp_2 = NULL;

  for (int i = envc - 1; i >= 0; --i)
  {
    --sp_2;
    *sp_2 = envs[i];
  }

  --sp_2;
  *sp_2 = NULL;

  for (int i = argc - 1; i >= 0; --i)
  {
    --sp_2;
    *sp_2 = args[i];
  }

  --sp_2;
  *((int *)sp_2) = argc;

  intptr_t *ptr_brk = (intptr_t *)(brk);
  uintptr_t entry = loader(pcb, filename);
  Area kstack = RANGE(pcb, (char *)pcb + STACK_SIZE);
  Context* ctx = ucontext(&pcb->as, kstack, (void *)entry);
  pcb->cp = ctx;
  ptr_brk--;
}
