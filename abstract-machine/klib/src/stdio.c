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
   char *s = NULL;//%s str pointer
   int d_val = 0;//%d integer
   int len = 0;//string length
    int num_buf[200];
    int top = 0;
   	int cnt = 0;
    bool sign = false;
    va_list ap;
    va_start(ap, fmt);
    for (; fmt != NULL && *fmt != '\0' ; fmt++)
    {
        if(*fmt != '%')
        {
            *out = *fmt;
            out++;
            continue;
        }
        fmt++;//此时,*fmt是%，我们需要跳过！
        cnt++;
        switch(*fmt)
        {
            case 's':
                s = va_arg(ap, char *);
                len = strlen(s);
                for (int i = 0 ; i < len ; i++)
                {
                    *out = *s;
                    out++;
                    s++;
                }
                break;
            case 'd':
                d_val = 0;
                d_val += va_arg(ap, int);
                sign = false;
                top = 0;
                if(d_val < 0)
                {
                    sign = true;
                    d_val = -d_val;
                }
                while(d_val)
                {
                    num_buf[++top] = d_val % 10;
                    d_val = d_val / 10;
                }
                if(top == 0)//也就是,d_val = 0
                {
                    *out = '0';
                    out++;
                } 
                else
                {
                    if(sign)
                    {
                      *out = '-';
                      out++;
                    }
                    while(top)
                    {
                        *out = num_buf[top] + '0';
                        top--;
                        out++;
                    }
                }
                break;
              default:
                   break;
        }
        
    }
  *out = '\0';//Here is important!
   va_end(ap);
   return cnt;
}
int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
