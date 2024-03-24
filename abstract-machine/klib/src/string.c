#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// The strlen result NOT contains the length of '\0'
size_t strlen(const char *s) {
  // panic("Not implemented");
  // NULL pointer case
  if(s == NULL) {
    return 0;
  }

  size_t len = 0;
  const char* ptr = s;
  while(*ptr != '\0') {
    len ++;
    ptr ++;
  }

  return len;
}

char *strcpy(char *dst, const char *src) {
  // panic("Not implemented");
  // NULL pointer case
  if(dst == NULL || src == NULL) {
    return NULL;
  }
  
  size_t i = 0;
  while(src[i] != '\0') {
    dst[i] = src[i];
    i ++;
  }
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  // panic("Not implemented");
  // NULL pointer case
  if(dst == NULL || src == NULL || n <= 0) {
    return NULL;
  }

  size_t i;
  for(i = 0; i < n && src[i] != '\0'; i ++) {
    dst[i] = src[i];
  }
  for(; i < n; i ++) {
    dst[i] = '\0';
  }

  return dst;
}

char *strcat(char *dst, const char *src) {
  // panic("Not implemented");
  if(dst == NULL || src == NULL) {
    return NULL;
  }

  size_t len_dst = strlen(dst);
  size_t i = 0;
  while(src[i] != '\0') {
    dst[len_dst + i] = src[i]; 
    i ++;
  }
  dst[len_dst + i] = '\0';
  
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  // panic("Not implemented");
  while(*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
    s1 ++;
    s2 ++;
  }
  return ((unsigned char)*s1 - (unsigned char)*s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  // panic("Not implemented");
  for(size_t i = 0; i < n; i ++){
    if(s1[i] != s2[i]) {
      return (unsigned char)s1[i] - (unsigned char)s2[i];
    }
  }

  return 0;
}

void *memset(void *s, int c, size_t n) {
  // panic("Not implemented");
  unsigned char *ptr = (unsigned char*)s;

  for(size_t i = 0; i < n; i ++) {
    ptr[i] = (unsigned char)c;
  }

  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  // panic("Not implemented");
  unsigned char *dst_ptr = (unsigned char *)dst;
  const unsigned char *src_ptr = (const unsigned char *)src;

  unsigned char temp[n + 1];

  for(size_t i = 0; i < n; i ++) {
    temp[i] = src_ptr[i];
  }

  for(size_t i = 0; i < n; i ++) {
    dst_ptr[i] = temp[i];
  }

  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  // panic("Not implemented");
  // NULL pointer check
  if(out == NULL || in == NULL) {
    return NULL;
  }
  // type conversion
  unsigned char *out_ptr = (unsigned char *)out;
  const unsigned char *in_ptr = (unsigned char *)in;

  // optimization : in the case of Big Data
  size_t i;
  for (i = 0; i + sizeof(unsigned long) <= n; i += sizeof(unsigned long)) {
    *(unsigned long *)(out_ptr + i) = *(const unsigned long *)(in_ptr + i);
  }

  for(; i < n; i ++) {
    out_ptr[i] = in_ptr[i];
  }

  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  // panic("Not implemented");
  const unsigned char* s1_ptr = (const unsigned char *)s1;
  const unsigned char* s2_ptr = (const unsigned char *)s2;
  
  for(size_t i = 0; i < n; i ++) {
    if(s1_ptr[i] != s2_ptr[i]) {
      return (unsigned char)s1_ptr[i] - (unsigned char)s2_ptr[i];
    }
  }

  return 0;
}

#endif
