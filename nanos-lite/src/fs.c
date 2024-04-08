#include <fs.h>
#include "syscall.h"
#include <device.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset; // offset in file
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENT, PROC_DISPINFO};
// enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT};

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
  [FD_FB]     = {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  [FD_EVENT] = {"/dev/events", 0, 0, 0, events_read, invalid_write},
  [PROC_DISPINFO] = {"/proc/dispinfo", 128, 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = cfg.width * cfg.height * 4;
}

int fs_open(const char* pathname, int flags, int mode) {
  // ignore the Last two paraments in Nanos-lite
  int file_table_size = sizeof(file_table) / sizeof(file_table[0]);
  for(int i = 0; i < file_table_size; i ++) {
    if(strcmp(file_table[i].name, pathname) == 0) {
      return i;
    }
  }

  return -1;
}

int fs_close(int fd) {
  return 0;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  switch (whence) {
    case 0 : // SEEK_SET
      if(offset >= 0 && offset <= file_table[fd].size){  
        file_table[fd].open_offset = offset;
      }else {
        return -1;
      }
      break;
    case 1 : // SEEK_CUR
      if(file_table[fd].open_offset + offset <= file_table[fd].size &&
         file_table[fd].open_offset + offset >= 0) {
          file_table[fd].open_offset += offset;
      }else {
        return -1;
      }
      break;
    case 2 : // SEEK_END
      if(file_table[fd].size + offset <= file_table[fd].size &&
         file_table[fd].size + offset >= 0) {
          file_table[fd].open_offset = file_table[fd].size + offset;
      }else {
        return -1;
      }
      break;
    default:
      break;
  } 

  return file_table[fd].open_offset;
}

size_t fs_read(int fd, void* buf, size_t len) {
  if(file_table[fd].read != NULL) {
    return file_table[fd].read(buf, file_table[fd].open_offset, len);
  } else {
    if(file_table[fd].open_offset + len <= file_table[fd].size) {
      int pos = file_table[fd].disk_offset + file_table[fd].open_offset;
      int res =  ramdisk_read(buf, pos, len);
      fs_lseek(fd, len, SEEK_CUR);
      return res;
    } else {
    // Log("offset: %d\tlen: %d", file_table[fd].open_offset, len);
    // assert(0);

    int res_offset = file_table[fd].size - file_table[fd].open_offset;
    int pos = file_table[fd].disk_offset + file_table[fd].open_offset;
    int res =ramdisk_read(buf, pos, res_offset);
    fs_lseek(fd, res, SEEK_CUR);
    return res;
    }
  }
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if(file_table[fd].write != NULL) {
    return file_table[fd].write(buf, file_table[fd].open_offset, len);
  } else {
    if(file_table[fd].open_offset + len <= file_table[fd].size) {
      // size_t cnt;
      int pos = file_table[fd].disk_offset + file_table[fd].open_offset;
      // Log("len : %d\tpos: %d", len, pos);
      int res =  ramdisk_write(buf, pos, len);
      fs_lseek(fd, res, SEEK_CUR);
      return res;
    } else {
      assert(0);
      return 0;
    }
  }
}


