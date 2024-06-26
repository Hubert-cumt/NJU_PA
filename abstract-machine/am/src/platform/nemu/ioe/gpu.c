#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  // int i;
  // int h = inw(VGACTL_ADDR);
  // int w = inw(VGACTL_ADDR + 2);
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for(i = 0; i < w * h; i ++) fb[i] = i;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t code = inl(VGACTL_ADDR);
  uint32_t width = code >> 16;
  uint32_t height = code & 0xffff;
  uint32_t size = width * height * sizeof(uint32_t);

  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width, .height = height,
    .vmemsz = size
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  // No need for synchronization for now
  if(!ctl->sync && (w == 0 || h == 0)) return;

  uint32_t *pixels = ctl->pixels;
  uint32_t *fb = (uint32_t*)(uintptr_t)FB_ADDR;
  uint32_t sc_w = inl(VGACTL_ADDR) >> 16;
  for(int i = y; i < y + h; i ++) {
    for(int j = x; j < x + w; j ++) {
      fb[sc_w * i + j] = pixels[w * (i - y) + (j - x)];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
