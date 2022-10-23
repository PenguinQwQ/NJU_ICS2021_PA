#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define __do_div(n, base) ({ \
int __res; \
__res = ((int)n) % (int) base; \
n = ((int)n) / (int) base; \
__res; })



//Here we do some macro definition
#define MAX_BUFFER_SIZE 20000
#define NOTYPE 1 //Notype output
#define ZEROPAD 1 //Zeropad the output stream
#define LEFT  2 //left aligned
#define POSITIVE  4 //positive number
#define EMPTY  8 //Empty pad

#define fmt_atoi(fmt, val) {\
	while(*(fmt) >= '0' && *(fmt) <= '9') val = (val << 3) + (val << 1) + *(fmt) - '0', fmt++;}


static char *number(char *str, int num, int base, int size, int flags)
{
    /* we are called with base 8, 10 or 16, only, thus don't need "G..." */
    static const char digits[16] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */
	static const char min_int[11] = "-2147483648";//0-10构成最小数
    char tmp[66];
    int i;
//    c = (type & ZEROPAD) ? '0' : ' ';//c是用来补位宽的字符
    if(num == -2147483648)//特殊处理最小的数
    {
        for (int j = 0 ; j <= 10 ; j++)
            *str++ = min_int[j];
       	return str;
    }
    
    if(num >= 0 && (flags & POSITIVE))
        *str++ = '+';
    if(num < 0){//一般的数，取反后写入
        *str++ = '-';
        num = -num;
    }
    i = 0;
    
    if (num == 0)
        tmp[i++] = '0';
    else
        while (num != 0)
            tmp[i++] = (digits[__do_div(num, base)]);
    while (i--)//倒序存入
        *str++ = tmp[i];
    return str;
}


int printf(const char *fmt, ...) {
  int len = 0;
  char output_buf[20000];
  va_list ap;
  va_start(ap, fmt);
  len = vsprintf(output_buf, fmt, ap);
  va_end(ap);
  len = strlen(output_buf);
  for (int i = 0 ; i < len ; i++)
    putch(output_buf[i]);
  return len;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    int num;
    int base;
    char* s;
    char *str;
    size_t flags;        /* flags to number() */
    size_t field_width;    /* width of output field */
    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }
        /* process flags */
        fmt++;
        
        flags = 0;   

        repeat:
            ++fmt;        
           switch (*fmt) {
               case '0':
                flags |= ZEROPAD;
                goto repeat;
               case '-':
                flags |= LEFT;
                goto repeat;
               case '+':
                flags |= POSITIVE;
                goto repeat;
               default:
                   flags |= NOTYPE;
                   break;
            }

        
        field_width = 0;
        if (*fmt >= '0' && *fmt <= '9')
            fmt_atoi(fmt, field_width)
       	else{
             if(*fmt == '*')
            field_width = va_arg(args, int);
            if(field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
            fmt++;
        }
        
        static char arg_str[MAX_BUFFER_SIZE];
        /* 原本的基设为十进制 */
        base = 10;
		char* tmp_str = arg_str;
        switch (*fmt) {
            case 'd':
            num = va_arg(args, int);
            tmp_str = number(tmp_str, num, base, field_width, flags);
			break;
        case 's':
            s = va_arg(args, char *);
            while(*s != '\0') *tmp_str++ = *s++;
			break;
        case 'c':
            *tmp_str++ = (unsigned char)va_arg(args, int);
			break;
        default:
			break;
        }
        *tmp_str = '\0';
        size_t s_len = strlen(arg_str);
        char c = (flags & ZEROPAD) ? '0' : ' ';
        if((flags & LEFT) == false)
        	for (int i = 0 ; i + s_len < field_width ; i++)
 				*str++ = c;
        strcpy(str, arg_str);
        str += s_len;
        if(flags & LEFT)
        	for (int i = 0 ; i + s_len < field_width ; i++)
 				*str++ = ' ';
    }
    *str = '\0';
    return str - buf;
}

int sprintf(char *out, const char *fmt, ...) {
    int len = 0;
    va_list ap;
    va_start(ap, fmt);
    len = vsprintf(out, fmt, ap);
    va_end(ap);
    return len;
}
int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
