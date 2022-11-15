#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_Half Elf64_Half
# define Elf_Addr Elf64_Addr
# define Elf_Word Elf64_Word
# define Elf_Xword Elf64_Xword


#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_Half Elf32_Half
# define Elf_Addr Elf32_Addr
# define Elf_Word Elf32_Word
# define Elf_Xword Elf32_Xword
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

#define MAX_BUFFER_SIZE 656600
static uint8_t file_buf[MAX_BUFFER_SIZE];

static uintptr_t loader(PCB *pcb, const char *filename) {
  //The Elf file is at least 4 bytes!
  assert(ramdisk_read(file_buf, 0, 52) == 52);
  Elf_Ehdr *elf = (Elf_Ehdr *)file_buf;
  assert(*(uint32_t *)elf->e_ident == 0x464c457f);
  printf("The Elf32 Header is %d bytes \n", sizeof(Elf32_Ehdr));
  printf("The Elf64 Header is %d bytes \n", sizeof(Elf64_Ehdr));
  printf("The Nanos Lite Elf Header is %d bytes \n", sizeof(Elf_Ehdr));


  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

