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
  assert(ramdisk_read(file_buf, 0, 2000) <= 2000);
  Elf_Ehdr *elf = (Elf_Ehdr *)file_buf;
  //Check the magic number
  assert(*(uint32_t *)elf->e_ident == 0x464c457f);
  //Check the elf support ISA type
  assert(*(elf->e_ident + 4) == ELFCLASS32);
  //Check the elf aligned method
  assert(*(elf->e_ident + 5) == ELFDATA2LSB);
  //Check the exact elf entry in virtual memory
  assert(elf->e_entry == 0x830003fc);
  uintptr_t ptr = (uintptr_t)elf->e_entry;
  assert(ptr == 0x830003fc);
  Elf_Off phoff = elf->e_phoff;
  Elf_Half phnum = elf->e_phnum;
  Elf_Half phentsize = elf->e_phentsize;

  for (Elf_Half num = 0 ; num < phnum ; num++)
  {    //Read a program header section into file_buf
//    assert(ramdisk_read(file_buf, phoff + num * phentsize, phentsize) == phentsize);
    Elf_Phdr *phdr = (Elf_Phdr *)(file_buf + phoff + num * phentsize);
    if(phdr->p_type == PT_LOAD)
    {
      printf("Here PT_LOAD!");
      assert(phdr->p_type == PT_LOAD);
      uint32_t offset = phdr->p_offset;
      uint32_t filesz = phdr->p_filesz;
      uint32_t memsz = phdr->p_memsz;
      uint32_t vaddr = phdr->p_vaddr;
      assert(filesz <= memsz);
      ramdisk_read((void *)vaddr, offset, filesz);
      memset((void *)(vaddr + filesz), 0, memsz - filesz);
    }
  }
/*
  printf("The Elf32 Header is %d bytes \n", sizeof(Elf32_Ehdr));
  printf("The Elf64 Header is %d bytes \n", sizeof(Elf64_Ehdr));
  printf("The Nanos Lite Elf Header is %d bytes \n", sizeof(Elf_Ehdr));
*/
  return ptr;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry;
  entry = loader(pcb, filename);
  assert(entry == 0x830003fc);
  Log("Jump to entry = %d", entry);
  ((void(*)())entry) ();
}

