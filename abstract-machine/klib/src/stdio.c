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
    int d_val = 0;//%d buf
    char* str = NULL;
    char s[20000];//%d s buffer
    int tot = 0;
    int cnt = 0;
    int ret = 0;
    va_list ap;//声明指向参数的指针
    va_start(ap, fmt);//指针初始化
    while(fmt != NULL && *fmt != '\0')
    {
        //如果读到了%d
        if((*fmt == '%') && (*(fmt + 1) == 'd'))
        {
            ret++;
            tot = 0;
            d_val = va_arg(ap, int);
            while(d_val)
            {
                s[++tot] = d_val % 10;
                d_val /= 10;
            }
            while(tot--)
            {
                *(out + cnt) = s[tot];
                cnt++;
            }
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
                *(out + cnt) = *(str + i);
                cnt++;
            }
             fmt++;
             fmt++;
             continue;
        }
        //如果读到了其他字符
        *(out + cnt) = *fmt;
        cnt++;
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
