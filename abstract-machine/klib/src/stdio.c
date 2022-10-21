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

#define ZEROPAD 1

static char *number(char *str, int num, int base, int size, int type)
{
    /* we are called with base 8, 10 or 16, only, thus don't need "G..." */
    static const char digits[16] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */
	static const char min_int[11] = "-2147483648";//0-10构成最小数
    char tmp[66];
    char* beg = str;
   // char c,
   char sign;
    int i;
 //   c = (type & ZEROPAD) ? '0' : ' ';//c是用来补位宽的字符
    if(num == -2147483648)//特殊处理最小的数
    {
        for (int j = 0 ; j <= 10 ; j++)
            *str++ = min_int[j];
       	return str;
    }
    if(num < 0){//一般的数，取反后写入
        sign = '-';
        num = -num;
    }
    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    else
        while (num != 0)
            tmp[i++] = (digits[__do_div(num, base)]);
    
    for (int j = i ; j >= 0 ; j--)
        putch(tmp[j]);
    putch('\n');
    if (sign == '-')//处理符号位
        *str++ = sign;
    while (i--)//倒序存入
        *str++ = tmp[i];
    for (;beg < str; beg++)
        putch(*beg);
    return str;
}


int printf(const char *fmt, ...) {
  panic("Not implemented!");
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    int len;
    int num;
    int i, base;
    char *str;
    const char *s;
    int flags;        /* flags to number() */
    int field_width;    /* width of output field */
    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }
        /* process flags */
        flags = 0;
        fmt++;
//     repeat:
//        ++fmt;        
//       switch (*fmt) {
//        case '0':
//            flags |= ZEROPAD;
//            goto repeat;
//        }

 /*       
        field_width = -1;
        if (*fmt >= '0' && *fmt <= '9')
            field_width = skip_atoi(&fmt);
        if(field_width < 0)
            field_width = -field_width;
*/
        /* 原本的基设为十进制 */
        base = 10;

        switch (*fmt) {
        case 'c':
            *str++ = (unsigned char)va_arg(args, int);
            continue;
        case 's':
            s = va_arg(args, char *);
            len = strlen(s);
            for (i = 0; i < len; ++i)
                *str++ = *s++;
            continue;
        case 'd':
            num = va_arg(args, int);
            str = number(str, num, base, field_width, flags);
            continue;
        default:
			break;
        }
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
