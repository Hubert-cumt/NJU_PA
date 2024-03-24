#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  // panic("Not implemented");
  va_list args;
  va_start(args, fmt);

  unsigned char *out_ptr = (unsigned char *)out;
  const unsigned char *fmt_ptr = (const unsigned char*)fmt;

  size_t i = 0, to = 0;
  while(fmt_ptr[i] != '\0') {
    if(fmt_ptr[i] != '%') {
      out_ptr[to ++] = fmt_ptr[i ++];
      // to ++;
      // i ++;
    }else {
      i ++;
      switch (fmt_ptr[i]) {
        case 'd': {// int 
          int value = va_arg(args, int);
          char temp[20];
          int temp_to = 0;

          // transfer int to char
          if(value < 0) {
            out_ptr[to ++] = '-';
            value = -value;
          }

          // Processing value with REVERSE
          do {
            temp[temp_to ++] = '0' + value % 10;
            value /= 10;
          } while(value > 0);

          while(temp_to > 0) {
            out_ptr[to ++] = temp[-- temp_to];
          }

          break;
        }

        case 's': { // string(char* char[])
          const char *str = va_arg(args, const char*);
          while(*str != '\0') {
            out_ptr[to ++] = *str ++;
          }
          break;
        }
        
        default:
          break;
      }
      // move the pointer from the placeholder to the next position
      fmt_ptr ++;
    }
  }
  out_ptr[to] = '\0';
  va_end(args);

  return to;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
