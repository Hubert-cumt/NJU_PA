#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define TEMP_SIZE 4096

int printf(const char *fmt, ...) {
  // panic("Not implemented");
  char temp[TEMP_SIZE];
  char *out = temp;

  va_list args;
  va_start(args, fmt);

  int res = vsprintf(out, fmt, args);

  va_end(args);
  
  for(int i = 0; temp[i] != '\0'; i ++) {
    putch(temp[i]);
  }
  return res;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  // panic("Not implemented");
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
      int width = 0;
      while(fmt_ptr[i] >= '0' && fmt_ptr[i] <= '9') {
        width = width * 10 + (fmt_ptr[i++] - '0');
      }
      switch (fmt_ptr[i]) {
        case 'd': {// int 
          int value = va_arg(ap, int);
          char temp[TEMP_SIZE];
          int temp_to = 0;

          // transfer int to char
          bool is_negative = false;
          if(value < 0) {
            value = -value;
            is_negative = true;
          }

          // Processing value with REVERSE
          do {
            temp[temp_to ++] = '0' + value % 10;
            value /= 10;
          } while(value > 0);

          if(is_negative) {
            temp[temp_to ++] = '-';
          }

          while(temp_to < width) {
            out_ptr[to ++] = ' ';
            width --;
          }

          while(temp_to > 0) {
            out_ptr[to ++] = temp[-- temp_to];
          }

          break;
        }

        case 's': { // string(char* char[])
          const char *str = va_arg(ap, const char*);
          char temp[TEMP_SIZE];
          int temp_to = 0;

          while(*str != '\0') {
            temp[temp_to ++] = *str ++;
          }

          if(temp_to < width) {
            out_ptr[to ++] = ' ';
            width--;
          }

          for(int i = 0; i < temp_to; i ++) {
            out_ptr[to ++] = temp[i];
          }

          // while(*str != '\0') {
          //   out_ptr[to ++] = *str ++;
          // }
          break;
        }
        
        case 'x': { // Hex
          unsigned long value = va_arg(ap, unsigned long);
          char temp[TEMP_SIZE];
          int temp_to = 0;
          int remainder = 0;

          // transfer word_t to Hexchar 
          do {
            remainder = value % 16;
            temp[temp_to ++] = remainder < 10 ? '0' + remainder : 'a' + (remainder - 10);
            value /= 16;
          }while(value > 0);

          while(temp_to < width) {
            out_ptr[to ++] = ' ';
            width --;
          }

          while(temp_to > 0) {
            out_ptr[to ++] = temp[-- temp_to];
          }

          break;
        }

        case 'p': {
          unsigned long value = va_arg(ap, unsigned long);
          char temp[TEMP_SIZE];
          int temp_to = 0;
          int remainder = 0;

          // transfer word_t to Hexchar 
          do {
            remainder = value % 16;
            temp[temp_to ++] = remainder < 10 ? '0' + remainder : 'a' + (remainder - 10);
            value /= 16;
          }while(value > 0);

          while(temp_to < width) {
            out_ptr[to ++] = ' ';
            width --;
          }

          while(temp_to > 0) {
            out_ptr[to ++] = temp[-- temp_to];
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
  va_end(ap);

  return to;
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
      int width = 0;
      while(fmt_ptr[i] >= '0' && fmt_ptr[i] <= '9') {
        width = width * 10 + (fmt_ptr[i++] - '0');
      }
      switch (fmt_ptr[i]) {
        case 'd': {// int 
          int value = va_arg(args, int);
          char temp[TEMP_SIZE];
          int temp_to = 0;

          // transfer int to char
          bool is_negative = false;
          if(value < 0) {
            value = -value;
            is_negative = true;
          }

          // Processing value with REVERSE
          do {
            temp[temp_to ++] = '0' + value % 10;
            value /= 10;
          } while(value > 0);

          if(is_negative) {
            temp[temp_to ++] = '-';
          }

          while(temp_to < width) {
            out_ptr[to ++] = ' ';
            width --;
          }

          while(temp_to > 0) {
            out_ptr[to ++] = temp[-- temp_to];
          }

          break;
        }

        case 's': { // string(char* char[])
          const char *str = va_arg(args, const char*);
          char temp[TEMP_SIZE];
          memset(temp, 0, sizeof(temp));
          int temp_to = 0;

          while(*str != '\0') {
            temp[temp_to ++] = *str ++;
          }

          if(temp_to < width) {
            out_ptr[to ++] = ' ';
            width--;
          }

          for(int i = 0; i < temp_to; i ++) {
            out_ptr[to ++] = temp[i];
          }

          // while(*str != '\0') {
          //   out_ptr[to ++] = *str ++;
          // }
          break;
        }
        
        case 'x': { // Hex
          unsigned long value = va_arg(args, unsigned long);
          char temp[TEMP_SIZE];
          int temp_to = 0;
          int remainder = 0;

          // transfer word_t to Hexchar 
          do {
            remainder = value % 16;
            temp[temp_to ++] = remainder < 10 ? '0' + remainder : 'a' + (remainder - 10);
            value /= 16;
          }while(value > 0);

          while(temp_to < width) {
            out_ptr[to ++] = ' ';
            width --;
          }

          while(temp_to > 0) {
            out_ptr[to ++] = temp[-- temp_to];
          }

          break;
        }

        // case 'f': {
        //   double value = va_arg(args, double);
        //   char temp_integer[TEMP_SIZE];
        //   char temp_decimal[TEMP_SIZE];
        //   int temp_interger_to = 0;
        //   int temp_decimal_to = 0;
        //   bool is_negative = false;

        //   if(value < 0) {
        //     value = -value;
        //     is_negative = true;
        //   }

        //   // process the integer part 
        //   int value_integer = (int)value;

        //   do {
        //     temp_integer[temp_interger_to ++] = '0' + value_integer % 10;
        //     value_integer /= 10;
        //   } while(value_integer > 0);

        //   if(is_negative) {
        //     temp_integer[temp_interger_to ++] = '-';
        //   }

        //   // process the decimal part
        //   double value_decimal = value - (int)value;

        //   while(value_decimal != 0) {
        //     value_decimal *= 10;
        //     temp_decimal[temp_decimal_to ++] = '0' + (int)value_decimal;
        //     value_decimal -= (int)value_decimal;
        //   }

        //   if(temp_decimal_to + temp_interger_to < width) {
        //     out_ptr[to ++] = ' ';
        //     width --;
        //   }

        //   while(temp_interger_to > 0) {
        //     out_ptr[to ++] = temp_integer[-- temp_interger_to];
        //   }

        //   for(int i = 0; i < temp_decimal_to; i ++){
        //     out_ptr[to ++] = temp_decimal[i];
        //   }

        //   break;
        // }


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
