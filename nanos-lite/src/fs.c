#include <fs.h>
#define min(a,b) ((a)<(b))?(a):(b)


typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

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

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write, 0},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write, 0},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write, 0},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write, 0},
#include "files.h"
};

static int file_num = sizeof(file_table) / sizeof(Finfo);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();



int fs_open(const char *pathname, int flags, int mode){
  for(int i = 0; i < file_num; i++){
    if(strcmp(pathname, file_table[i].name) == 0){
      file_table[i].open_offset = 0;
      return i;
    }
  }
  printf("The path: %s file not found!!!\n", pathname);
  assert(0);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len){
  assert(buf != NULL);
  if(fd < 0 || fd >= file_num)
    {
      panic("Invalid File Describer!!!");
      return -1;
    }
  
  //Check The inner VFS read function and call it directly!
  if(file_table[fd].read)
    return file_table[fd].read(buf, file_table[fd].open_offset, len);
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
  size_t read_len = file_table[fd].size - file_table[fd].open_offset;
  if(read_len == 0) return 0;
  size_t read_bytes = min(read_len, len);
  ramdisk_read(buf, offset, read_bytes);
  assert(file_table[fd].size >= file_table[fd].open_offset);
  file_table[fd].open_offset += read_bytes;
  return read_bytes;
}

size_t fs_write(int fd, const void *buf, size_t len){
  assert(buf);
  if(fd == 0)
  {
    Log("Machine should never write to stdin!");
    assert(0);
    return -1;
  }
  if(fd < 0 || fd >= file_num)
  {
    Log("Invalid File Describer!!!");
    assert(0);
    return -1;
  }
  if(file_table[fd].write)
    return file_table[fd].write(buf, file_table[fd].open_offset, len);
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
  size_t write_len = file_table[fd].size - file_table[fd].open_offset;
  if(write_len == 0) return 0;
  size_t write_bytes = min(len, write_len);
  ramdisk_write(buf, offset, write_bytes);
  assert(file_table[fd].size >= file_table[fd].open_offset);
  file_table[fd].open_offset += write_bytes;
  return write_bytes;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  //Check the file describer is valid!
    if(fd < 2 || fd >= file_num)
    {
      panic("Invalid File Describer!!!");
      return -1;
    }
  //Check the offset&size is compatiable.
  assert(offset <= file_table[fd].size);
//  printf("fd= %d , offset = %p, file_table[fd].open_offset = %p\n", fd, offset, file_table[fd].open_offset);
  switch (whence){
  case SEEK_SET: file_table[fd].open_offset = offset; break;
  case SEEK_CUR: file_table[fd].open_offset += offset; break;
  case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
  default: printf("Invalid Pointer Placement!!!\n"); assert(0); return -1;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd){
  assert(fd >= 3 && fd < file_num);
  file_table[fd].open_offset = 0;
  return 0;
}

void init_fs() {
  AM_GPU_CONFIG_T gpu = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = gpu.width * gpu.height * sizeof(uint32_t);
}