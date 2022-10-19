#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void num_process(char *str, int num, int base)
{
    int buf_tmp[20000];
    int i = 0;
    int len = 0;
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
        len++;
        return;
    }
    if(sign)
    {
        *str = '-';
        str++;
        len++;
    }
    while(i--)
    {
        *str = buf_tmp[i] + '0';
        str++;
        len++;
    }
}



int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
    char *str = NULL;//%s buf
    int tot = 0;
    int d_val = 0;
    int ret = 0;
    va_list ap;//声明指向参数的指针
    va_start(ap, fmt);//指针初始化
    while(fmt != NULL && *fmt != '\0')
    {
        //如果读到了%d
        if((*fmt == '%') && (*(fmt + 1) == 'd'))
        {
            ret++;
            d_val = va_arg(ap, int);
            num_process(out, d_val, 10);
            fmt++;
            fmt++;
            continue;
        }
        //如果读到了%s
         if((*fmt == '%') && (*(fmt + 1) == 's'))
        {
            ret++;
            str = va_arg(ap, char *);
            tot = strlen(str);
			for (int i = 0 ; i < tot ; i++)
            {
                *out= *(str + i);
                out++;
            }
             fmt++;
             fmt++;
             continue;
        }
        //如果读到了其他字符
        *out = *fmt;
        out++;
       	fmt++;
    }
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
