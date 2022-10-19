#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    size_t len = 0;
    //If s is a nullptr, then we should return 0;
    if(s == NULL)
      return 0;
    while((s != NULL) && (*s != '\0'))
    {
     len++;	
     s++;   
    }
    return len;
}

char *strcpy(char *dst, const char *src) {
  size_t len_src = strlen(src);
  if(sizeof(dst) < sizeof(src))
      panic("The Destination String is Too short!! String Overflow!");
  for (int i = 0 ; i <= len_src ; i++) //Including the '\0', so it ends with len_src
      	*(dst + i) = *(src + i);
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t len_src = strlen(src);
  if(sizeof(dst) < ((n < sizeof(src)) ? n : sizeof(src))) //Check if the dst array holds the src array
      panic("The Destination String is too short!! String Overflow!");
  if(len_src + 1 <= n) //包括上'\0'，还是小于count，就整个复制。否则，只复制count个
  {
  for (int i = 0 ; i <= len_src ; i++) //Including the '\0', so it ends with len_src
      	*(dst + i) = *(src + i);
  }
  else
  {
    for (int i = 0 ; i <= n - 1; i++) //Including the '\0', so it ends with len_src
      	*(dst + i) = *(src + i);
  }
  return dst;
}


char *strcat(char *dst, const char *src) {
  	size_t len_dst = strlen(dst), len_src = strlen(src);
//    if(strlen(dst) + strlen(src) + 1 > sizeof(dst))
 //       panic("strcat overflow! src+dst > sizeof dst!");
    for (int i = len_dst ; i <= len_dst + len_src; i++)
    	*(dst + i) = *(src + i - len_dst);
    return dst;
}

/*
char *strcat(char *dst, const char *src) {
    assert((dst != NULL) && (src != NULL));
     *pos = dst + sizeof(dst) - 1, 
    char *beg = dst;
    while(*dst){dst++;}
    int len_src = strlen(src);
//    if(pos - dst + 1 < len_src)
//    	panic("strcat overflow! src+dst > sizeof dst!");
    for (int i = 0 ; i <= len_src; i++)
    	*(dst + i) = *(src + i);
    return beg;
}
*/
int strcmp(const char *s1, const char *s2) {
 	int i = 0, len_s1 = strlen(s1), len_s2 = strlen(s2);
    while((i <= len_s1 - 1) && (i <= len_s2 - 1) && (s1[i] == s2[i]))
        {
          i++;
        }
   	return s1[i] - s2[i];
}

int strncmp(const char *s1, const char *s2, size_t n) {
 	int i = 0, len_s1 = strlen(s1), len_s2 = strlen(s2);
    while((i <= n - 1) && (i <= len_s1 - 1) && (i <= len_s2 - 1) && (s1[i] == s2[i]))
        {
          i++;
        }
   	return s1[i] - s2[i];
}

void *memset(void *s, int c, size_t n) {
	assert(s);
    char* tmp = (char *)s;
    for (int i = 1 ; i <= n && i <= sizeof(tmp); i++)
    {
        *tmp = (char)c;
        tmp++;
    }
    return s;
}

void *memmove(void *dst, const void *src, size_t n) {
	assert((dst != NULL) && (src != NULL));
    char* t1 = (char *)dst;
    char* t2 = (char *)src;
    for (int i = 1 ; i <= n && i <= sizeof(t1) && i <= sizeof(t2) ; i++)
    {
        *t1 = *t2;
        t1++;
        t2++;
    }
    return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
	assert((out != NULL) && (in != NULL));
    char* o = (char *)out;
    char* i = (char *)in;
    int k = 0;
    while(n-- && k <= sizeof(o) && k <= sizeof(i))
    {
       *o = *i;
        o++;
        i++;
        k++;
    }
    return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
 	assert((s1 != NULL) && (s2 != NULL));
    char* x = (char *)s1;
    char* y = (char *)s2;
    int k = 0;
    while(n-- && k <= sizeof(x) && k <= sizeof(y))
    {
        if(*x != *y)
            break;
        k++;
    }
    return *x - *y;
}

int charcmp(char x, char y)
{
    return x - y;
}
#endif
