#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

size_t serial_write(const void *buf, size_t offset, size_t len);


typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}



/* This is the information about all files in disk. */


static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};





static uint32_t file_num = sizeof(file_table)/sizeof(Finfo);

size_t fs_open(const char *pathname, int flags, int mode)
{
  for (int i = 0 ; i < file_num ; i++)
  {
    if(strcmp(file_table[i].name, pathname) == 0)
    {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  Log("The path/name %s file is not found!", pathname);
  assert(0);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{
  assert(buf);
  if(fd < 0 || fd >= file_num) return -1;//Not exists!
  size_t remain_size = file_table[fd].size - file_table[fd].open_offset;
  len = (remain_size < len) ? remain_size : len;
  assert(file_table[fd].size >= file_table[fd].open_offset);
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, void *buf, size_t len)
{
  assert(buf);
  if(fd < 0 || fd >= file_num) return -1;  
  size_t remain_size = file_table[fd].size - file_table[fd].open_offset;
  len = (remain_size < len) ? remain_size : len;
  assert(file_table[fd].size >= file_table[fd].open_offset);
  ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  if(fd < 0 || fd >= file_num) return -1;
  assert(offset <= file_table[fd].size);
  switch (whence)
  {
  case SEEK_CUR: file_table[fd].open_offset += offset; break;
  case SEEK_SET: file_table[fd].open_offset = offset; break;
  case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
  default: return -1;
  }
  assert(file_table[fd].open_offset <= file_table[fd].size);
  return file_table[fd].open_offset;
}

int fs_close(int fd)
{
  file_table[fd].open_offset = 0;
  return 0;
}


void init_fs() {
  // TODO: initialize the size of /dev/fb
}



