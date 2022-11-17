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
//static uint8_t file_buf[MAX_BUFFER_SIZE];

size_t fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);

  assert(fs_lseek(fd, 0, SEEK_SET) == 0);
  Elf_Ehdr ehdr;
  assert(fs_read(fd, &ehdr, sizeof(ehdr)) == sizeof(ehdr));
  assert(ehdr.e_ident[0] == 0x7f);
  assert(ehdr.e_ident[1] == 0x45);
  assert(ehdr.e_ident[2] == 0x4c);
  assert(ehdr.e_ident[3] == 0x46);

  int phoff = ehdr.e_phoff;
  int shoff = ehdr.e_shoff;
  int phnum = ehdr.e_phnum;
  int phsz = ehdr.e_phentsize;

  printf("phnum = %p \n", phnum);
  printf("phsz = %p \n", phsz);
  printf("phoff = %p \n", phoff);
  printf("shoff = %p \n", shoff);

  for (Elf_Half num = 0 ; num < phnum ; num++)
  {
    Elf_Phdr phdr;
    printf("%p \n", phoff);
    assert(fs_lseek(fd, phoff, SEEK_SET) == phoff);
    assert(fs_read(fd, &phdr, sizeof(phdr)) == sizeof(phdr));
  if(phdr.p_type == PT_LOAD)
  {
    Log("PT_LOAD DETECTED!!!");
    Elf_Addr vaddr = phdr.p_vaddr;
    Elf_Xword filesz = phdr.p_filesz;
    Elf_Xword memsz = phdr.p_memsz;
    assert(filesz <= memsz);
    Elf_Off offset = phdr.p_offset;
   // assert(0);
    assert(fs_lseek(fd, offset, SEEK_SET) == offset);
    assert(fs_read(fd, (void *)vaddr, filesz) == filesz);
    memset((void *)(vaddr + (Elf_Addr)filesz), 0, memsz - filesz);
  }
    phoff += phsz;
  }
/*


  assert(fs_read(fd, (void *)file_buf, MAX_BUFFER_SIZE) <= MAX_BUFFER_SIZE);
  fs_close(fd);
  //The Elf file is at least 4 bytes!
  Elf_Ehdr *elf = (Elf_Ehdr *)file_buf;
  //Check the magic number
  assert(*(uint32_t *)elf->e_ident == 0x464c457f);
  //Check the elf support ISA type
  assert(*(elf->e_ident + 4) == ELFCLASS32);
  //Check the elf aligned method
  assert(*(elf->e_ident + 5) == ELFDATA2LSB);

  Elf_Off phoff = elf->e_phoff;
  Elf_Half phnum = elf->e_phnum;
  Elf_Half phentsize = elf->e_phentsize;

  for (Elf_Half num = 0 ; num < phnum ; num++)
  {    //Read a program header section into file_buf
//    assert(ramdisk_read(file_buf, phoff + num * phentsize, phentsize) == phentsize);
    Elf_Phdr *phdr = (Elf_Phdr *)(file_buf + phoff + num * phentsize);
    if(phdr->p_type == PT_LOAD)
    {
      printf("Here PT_LOAD!\n");
      assert(phdr->p_type == PT_LOAD);
      uint32_t offset = phdr->p_offset;
      uint32_t filesz = phdr->p_filesz;
      uint32_t memsz = phdr->p_memsz;
      uint32_t vaddr = phdr->p_vaddr;
      assert(filesz <= memsz);
      memcpy((void *)vaddr, file_buf + offset, filesz);
      memset((void *)(vaddr + filesz), 0, memsz - filesz);
    }
  }
  */
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry;
  entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}
