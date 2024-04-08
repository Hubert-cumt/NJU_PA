#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h> 

static int evtdev = -1;
static int fbdev = -1;
static int dispinfo = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval t;
  gettimeofday(&t, NULL); 
  return t.tv_sec * 1000000 + t.tv_usec;
}

int NDL_PollEvent(char *buf, int len) {
  // printf("NDL_PollEvent\n");
  return read(evtdev, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }

  // get the screen_w and screen_h
  if(*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
  } 
  canvas_w = *w;
  canvas_h = *h;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fd = open("/dev/fb", 0, 0);
  for(int i = 0; i < h; i++) {
    lseek(fd, (screen_w * (i + y) + x) * 4, SEEK_SET);
    write(fd, pixels + w * i, w * 4);
  }
  close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }

  char buf[64];
  int dispinfo = open("/proc/dispinfo", 0, 0);
  read(dispinfo, buf, sizeof(buf));
  sscanf(buf, "WIDTH : %d\nHEIGHT : %d\n", &screen_w, &screen_h);

  evtdev = open("/dev/events", 0, 0);
  dispinfo = open("/proc/dispinfo", 0, 0);
  return 0;
}

void NDL_Quit() {
}
