/*
#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENTS, FD_DISPINFO, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write, 0},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write, 0},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write, 0},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write, 0},
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
  if(file_table[fd].read)
    return file_table[fd].read(buf, file_table[fd].open_offset, len);

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
  if(file_table[fd].write)
    return file_table[fd].write(buf, file_table[fd].open_offset, len);
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
  printf("offset = %p\n file_table[%d].size = %p\n", offset, fd, file_table[fd].size);
  assert(offset <= file_table[fd].size);
  switch (whence)
  {
  case SEEK_CUR: file_table[fd].open_offset += offset; break;
  case SEEK_SET: file_table[fd].open_offset = offset; break;
  case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
  default: return -1;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd)
{
  file_table[fd].open_offset = 0;
  return 0;
}


void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T gpu = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = gpu.width * gpu.height * sizeof(uint32_t);
}

*/

#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENTS, FD_DISPINFO, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
  [FD_EVENTS] = {"/dev/events", 0, 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
  [FD_FB] = {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
#include "files.h"
};

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

void init_fs() {
  AM_GPU_CONFIG_T gpu = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = gpu.width * gpu.height * 4;
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode){
  int nfs = sizeof(file_table)/sizeof(Finfo);
  bool is_find = 0;
  int ret = 0;
  for(int i = 0; i < nfs; i++){
    if(strcmp(pathname,file_table[i].name) == 0){
      ret = i;
      is_find = 1;
      break;
    }
  }
  if(!is_find){
    panic("%s: No such file !\n",pathname);
  }
  return ret;
}

size_t fs_read(int fd, void *buf, size_t len){
  if(file_table[fd].read)
    return file_table[fd].read(buf, file_table[fd].open_offset, len);
  
  assert(buf);
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;

  assert(file_table[fd].size >= file_table[fd].open_offset);

  size_t readable = file_table[fd].size - file_table[fd].open_offset;
  
  if(readable == 0) return 0;

  size_t bytes = len;
  if(readable < len) bytes = readable;

  ramdisk_read(buf, offset, bytes);
  file_table[fd].open_offset += bytes;

  return bytes;
}

size_t fs_write(int fd, const void *buf, size_t len){
  if(file_table[fd].write)
    return file_table[fd].write(buf, file_table[fd].open_offset, len);
  
  //assert(fd >= 4);
  assert(buf);
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;

  assert(file_table[fd].size >= file_table[fd].open_offset);

  size_t writable = file_table[fd].size - file_table[fd].open_offset;
  
  if(writable == 0) return 0;


  size_t bytes = len;
  if(writable < len) bytes = writable;

  ramdisk_write(buf, offset, bytes);
  file_table[fd].open_offset += bytes;

  return bytes;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  printf("fd= %d , offset = %p, file_table[fd].open_offset = %p\n", fd, offset, file_table[fd].open_offset);
  switch (whence){
  case SEEK_SET:
    file_table[fd].open_offset = offset;
    break;
  case SEEK_CUR:
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  default: 
    assert(0);
    break;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd){
  file_table[fd].open_offset = 0;
  return 0;
}