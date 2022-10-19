#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void num_process(char *str, long num, int base)
{
    char buf_tmp[20000];
    int i = 0;
    bool sign = false, zero = false;
	if(num < 0)
    {
		sign = true;
        num = -num;
    }
    if(num == 0)
    {
        zero = true;
    }
    while(num > 0)
    {
        buf_tmp[++i] = num % base;
        num = num / base;
    }
    if(zero)
    {
        *str = '0';
        str++;
        return;
    }
    if(sign)
    {
        *str = '-';
        str++;
    }
    while(i--)
    {
        *str = buf_tmp[i];
        str++;
    }
    return;
}



int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int cnt = 0;
  int val = 0;
  int len = 0;
  int base = 10;
  char *s = NULL;
  char *str = NULL;
  for (str = out ; fmt != NULL && *fmt != '\0' ; fmt++) {
	if(*fmt != '%'){//If fmt is not %，input directly!
        *str = *fmt;
        str++;
        continue;
    }
    /* Here we process some flags*/
    /* The Only Flag we should process in PA 2.2 is the minus! */ 
    cnt++;
    fmt++;//Jump over the %
    /* We check the conversion qualifier */
    switch (*fmt) {
        case 's':     
            s = va_arg(ap, char *);
            len = strlen(s);
            for (int i = 0 ; i < len ; i++)
            {
				*str = *s;
                str++;
                s++;
            }
        case 'd':
            base = 10;
            val = va_arg(ap, int);
            num_process(str, val, base);
    }
	}
  return cnt;
}

int sprintf(char *out, const char *fmt, ...) {
    int ret = 0;
    va_list ap;//声明指向参数的指针
    va_start(ap, fmt);//指针初始化
    ret = vsprintf(out, fmt, ap);
    va_end(ap);
  	return ret;
}
int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
