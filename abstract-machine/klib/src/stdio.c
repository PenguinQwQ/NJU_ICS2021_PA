#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
    /*
  char output_buf[20000];
  int len = 0;
  va_list ap;
  va_start(ap, fmt);
  len = vsprintf(output_buf, fmt, ap);
  va_end(ap);
  for (int i = 0 ; i < len ; i++)
    putch(output_buf[i]);
  return len;
  */
  panic("Not implemented!");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
     panic("Not implemented");
     /*
   char *s = NULL;//%s str pointer
   int d_val = 0;//%d integer
   int len = 0;//string length
    int num_buf[200];
    int top = 0;
   	int cnt = 0;
    bool sign = false;
    for (; fmt != NULL && *fmt != '\0' ; fmt++)
    {
        if(*fmt != '%')
        {
            *out = *fmt;
            out++;
            cnt++;
            continue;
        }
        fmt++;//此时,*fmt是%，我们需要跳过！
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
                    cnt++;
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
                    cnt++;
                } 
                else
                {
                    if(sign)
                    {
                      *out = '-';
                      out++;
                      cnt++;
                    }
                    while(top)
                    {
                        *out = num_buf[top] + '0';
                        top--;
                        out++;
                        cnt++;
                    }
                }
                break;
              default:
                   break;
        }
        
    }
  *out = '\0';//Here is important!
   return cnt;
   */
}

int sprintf(char *out, const char *fmt, ...) {
      panic("Not implemented");
  /*
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
            cnt++;
            continue;
        }
        fmt++;//此时,*fmt是%，我们需要跳过！
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
                    cnt++;
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
                    cnt++;
                } 
                else
                {
                    if(sign)
                    {
                      *out = '-';
                      out++;
                      cnt++;
                    }
                    while(top)
                    {
                        *out = num_buf[top] + '0';
                        top--;
                        out++;
                        cnt++;
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
   */
}
int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
