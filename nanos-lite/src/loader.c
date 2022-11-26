#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_Half Elf64_Half
# define Elf_Addr Elf64_Addr
# define Elf_Word Elf64_Word
# define Elf_Xword Elf64_Xword
# define Elf_Off Elf64_Off
# define ELFCLASS ELFCLASS64
# define ELFDATA ELFDATA2LSB
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_Half Elf32_Half
# define Elf_Addr Elf32_Addr
# define Elf_Word Elf32_Word
# define Elf_Xword Elf32_Xword
# define Elf_Off Elf32_Off
# define ELFCLASS ELFCLASS32
# define ELFDATA ELFDATA2LSB
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


size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

size_t fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  //Make sure the file describer is valid
  assert(fd != -1);
  //Move the file describer to SEEK_SET
  assert(fs_lseek(fd, 0, SEEK_SET) == 0);
  Elf_Ehdr ehdr;
  //Read the elf head
  assert(fs_read(fd, &ehdr, sizeof(ehdr)) == sizeof(ehdr));
  //Check The Elf file's magic number and file type
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  //Check The Elf file's ISA type
  assert(ehdr.e_ident[4] == ELFCLASS);
  //Check The Elf file's data type
  assert(ehdr.e_ident[5] == ELFDATA);
  /*
  assert(ehdr.e_ident[0] == 0x7f);
  assert(ehdr.e_ident[1] == 0x45);
  assert(ehdr.e_ident[2] == 0x4c);
  assert(ehdr.e_ident[3] == 0x46);
  */
  int phoff = ehdr.e_phoff;
  int phpos = phoff;
  int phnum = ehdr.e_phnum;
  int phsz = ehdr.e_phentsize;
/*
  printf("phnum = %p \n", phnum);
  printf("phsz = %p \n", phsz);
  printf("phoff = %p \n", phoff);
  printf("shoff = %p \n", shoff);
*/
  for (Elf_Half num = 0 ; num < phnum ; num++)
  {
    Elf_Phdr phdr;
    phpos = phoff + num * phsz;
//    printf("num = %d \n", num);
//    printf("loader's phoff = %p \n", phoff);
    assert(fs_lseek(fd, phpos, SEEK_SET) == phpos);
    assert(fs_read(fd, &phdr, sizeof(phdr)) == sizeof(phdr));
  if(phdr.p_type == PT_LOAD)
  {
  //  Log("PT_LOAD DETECTED!!!");
    Elf_Addr vaddr = phdr.p_vaddr;
    Elf_Xword filesz = phdr.p_filesz;
    Elf_Xword memsz = phdr.p_memsz;
    assert(filesz <= memsz); //Make sure file size is less than memory size!
    Elf_Off offset = phdr.p_offset;
    /*
  printf("offset = %p \n", offset);
  printf("vaddr = %p \n", vaddr);
  printf("filesz = %p \n", filesz);
  printf("memsz = %p \n", memsz);
  */
   // assert(0);
    assert(fs_lseek(fd, offset, SEEK_SET) == offset);
    assert(fs_read(fd, (void *)vaddr, filesz) == filesz);
    //Set the remain space to 0!
    memset((void *)(vaddr + (Elf_Addr)filesz), 0, (size_t)(memsz - filesz));
  }
  }
  fs_close(fd);
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry;
  entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}