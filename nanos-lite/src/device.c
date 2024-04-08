#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  const char* buffer = (const char*)buf;
  int cnt = 0;
  for(;cnt < len; cnt ++) {
    putch(buffer[cnt]);
  }
  return cnt;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  bool down = ev.keydown;
  int key = ev.keycode;

  // Log("down: %d, key: %d", down, key);

  char event_str[32];
  memset(event_str, 0, sizeof(event_str));
  if (down) {
    sprintf(event_str, "kd %s\n", keyname[key]);
  } else {
    if(key != AM_KEY_NONE) {
      sprintf(event_str, "ku %s\n", keyname[key]);
    }else {
      sprintf(event_str, "\n");
      return 0;
    }
  }

  memset(buf, 0, len);

  // write event string to buf
  int i = 0;
  // printf("strlen(event_str): %d, len: %d\n", strlen(event_str), len);
  for (; i < strlen(event_str) && i < len; i++) {
    ((char *)buf)[i] = event_str[i];
  }
  return i;
}

static uint32_t screen_w = 0, screen_h = 0;
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  char str[128];
  memset(str, 0, sizeof(str));
  sprintf(str, "WIDTH: %d\nHEIGHT: %d\n", cfg.width, cfg.height);
  screen_w = cfg.width;
  screen_h = cfg.height;
  // write the str to buf
  int i = 0;
  for (; i < strlen(str) && i < len; i++) {
    ((char *)buf)[i] = str[i];
  }

  return i;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
//   // calculate the coordinate by offset
//   AM_GPU_CONFIG_T t = io_read(AM_GPU_CONFIG);

//   offset = offset / 4;
//   int w = len / 4;

//   int y = offset / t.width;
//   int x = offset - y * t.width;

//   io_write(AM_GPU_FBDRAW, x, y, (void*)buf, w, 1, true);
//   return len;
// }
  io_write(AM_GPU_FBDRAW, (offset % (screen_w * 4)) / 4,
           offset / (screen_w * 4), (uint32_t *)buf, len / 4, 1, true);
  return 1;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
