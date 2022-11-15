#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_Half Elf64_Half
# define Elf_Addr Elf64_Addr
# define Elf_Word Elf64_Word
# define Elf_Xword Elf64_Xword
# define Elf_Off Elf64_Off

#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_Half Elf32_Half
# define Elf_Addr Elf32_Addr
# define Elf_Word Elf32_Word
# define Elf_Xword Elf32_Xword
# define Elf_Off Elf32_Off
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

#define MAX_BUFFER_SIZE 656600
static uint8_t file_buf[MAX_BUFFER_SIZE];

static uintptr_t loader(PCB *pcb, const char *filename) {
  //The Elf file is at least 4 bytes!
  assert(ramdisk_read(file_buf, 0, sizeof(Elf_Ehdr)) == sizeof(Elf_Ehdr));
  Elf_Ehdr *elf = (Elf_Ehdr *)file_buf;
  assert(*(uint32_t *)elf->e_ident == 0x464c457f);

  Elf_Off phoff = elf->e_phoff;
  Elf_Half phnum = elf->e_phnum;
  Elf_Half phentsize = elf->e_phentsize;

  for (Elf_Half num = 0 ; num < phnum ; num++)
  {
    //Read a program header section into file_buf
    assert(ramdisk_read(file_buf, (uintptr_t)elf + phoff + num * phentsize, phentsize) == phentsize);
    Elf_Phdr *phdr = (Elf_Phdr *)file_buf;
    if(phdr->p_type == PT_LOAD)
    {
      assert(phdr->p_type == PT_LOAD);
      Elf_Off offset = phdr->p_offset;
      Elf_Xword filesz = phdr->p_filesz;
      Elf_Xword memsz = phdr->p_memsz;
      Elf_Addr vaddr = phdr->p_vaddr;
      assert(filesz <= memsz);
      memcpy((void *)vaddr, (void *)offset, filesz);
      memset((void *)(vaddr + (Elf_Addr)filesz), 0, memsz - filesz);
    }
  }
/*
  printf("The Elf32 Header is %d bytes \n", sizeof(Elf32_Ehdr));
  printf("The Elf64 Header is %d bytes \n", sizeof(Elf64_Ehdr));
  printf("The Nanos Lite Elf Header is %d bytes \n", sizeof(Elf_Ehdr));
*/
  return elf->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

