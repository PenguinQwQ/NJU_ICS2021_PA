/*
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
    printf("num = %d \n", num);
    printf("loader's phoff = %p \n", phoff);
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
  printf("offset = %p \n", offset);
  printf("vaddr = %p \n", vaddr);
  printf("filesz = %p \n", filesz);
  printf("memsz = %p \n", memsz);
   // assert(0);
    assert(fs_lseek(fd, offset, SEEK_SET) == offset);
    assert(fs_read(fd, (void *)vaddr, filesz) == filesz);
    memset((void *)(vaddr + (Elf_Addr)filesz), 0, (size_t)(memsz - filesz));
  }
    phoff += phsz;
  }
  fs_close(fd);

*/
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
 /*
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry;
  entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

*/

#include <proc.h>
#include <fs.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_Shdr Elf64_Shdr
# define Elf_class ELFCLASS64
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_Shdr Elf32_Shdr
# define Elf_class ELFCLASS32
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
size_t fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

int check_elf(const Elf_Ehdr * ehdr){
  const unsigned char * e_ident=ehdr->e_ident;
  if(e_ident[EI_MAG0]!=ELFMAG0||e_ident[EI_MAG1]!=ELFMAG1||e_ident[EI_MAG2]!=ELFMAG2||e_ident[EI_MAG3]!=ELFMAG3) return 1;
  if(e_ident[EI_CLASS]!=Elf_class){
    unsigned char t=e_ident[EI_CLASS];
    return t==ELFCLASS32||t==ELFCLASS64||t==ELFCLASSNONE?2:1;
  }
  if(e_ident[EI_DATA]!=ELFDATA2LSB){
    unsigned char t=e_ident[EI_VERSION];
    return t==ELFDATANONE||t==ELFDATA2MSB?3:1;
  }
  if(e_ident[EI_VERSION]!=EV_CURRENT) return e_ident[EI_VERSION]==EV_NONE?4:1;
  for(int i=EI_PAD;i<EI_NIDENT;i++) if(e_ident[i]!=0) return 1;
  
  if(ehdr->e_type!=ET_EXEC) return 5;
  if(ehdr->e_machine!=EXPECT_TYPE) return 6;
  return 0;
}

#define safe_read(buf,offset,len)\
  do{\
    assert(fs_lseek(fd,(offset),SEEK_SET)==(offset));\
    assert(fs_read(fd,(buf),(len))==(len));\
  } while (0)

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd=fs_open(filename,0,0);

  Elf_Ehdr ehdr;
  safe_read(&ehdr,0,sizeof(Elf_Ehdr));
  int ret=check_elf(&ehdr);
  if(ret!=0) panic("Error %d exists when loading programs.",ret);
  size_t total=ehdr.e_phnum;
  if(total==PN_XNUM){
    Elf_Shdr shdr;
    safe_read(&shdr,ehdr.e_shoff,sizeof(Elf_Shdr));
    total=shdr.sh_info;
  }
  for(uintptr_t addr=ehdr.e_phoff;total;addr+=ehdr.e_phentsize,total--) {
    Elf_Phdr phdr;
    safe_read(&phdr,addr,sizeof(Elf_Phdr));
    if(phdr.p_type==PT_LOAD){
      safe_read((void *)phdr.p_vaddr,phdr.p_offset,phdr.p_filesz);
      memset((void *)(phdr.p_vaddr+phdr.p_filesz),0,phdr.p_memsz-phdr.p_filesz);
    }
  }
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

