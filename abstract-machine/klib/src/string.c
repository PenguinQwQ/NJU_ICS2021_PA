#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
      if(s == NULL)
    {
      panic("s is a nullptr!");
      return 0;
    }
    size_t len = 0;
    //If s is a nullptr, then we should return 0;
    while((s != NULL) && (*s != '\0'))
    {
     len++;	
     s++;   
    }
    return len;
}

char *strcpy(char *dst, const char *src) {
    if((dst == NULL) || (src == NULL))
    {
      panic("dst or src is a nullptr!");
      return dst;
    }
    char* beg = dst;
    while(*src != '\0') 
    {
    *dst = *src;
    dst++; src++;
    }
    *dst = '\0';
  return beg;
}

char *strncpy(char *dst, const char *src, size_t n) {
    panic("Not Implemented!");
}


char *strcat(char *dst, const char *src) {
    if((dst == NULL) || (src == NULL))
    {
      panic("dst or src is a nullptr!");
      return dst;
    }
  char * beg = dst + strlen(dst);
	while(*src != '\0')
  {
		*beg = *src;
    beg++;
    src++;
  }
  *beg = '\0';
	return dst;
}

int strcmp(const char *s1, const char *s2) {
    if((s1 == NULL) || (s2 == NULL))
    {
      panic("s1 or s2 is a nullptr!");
      return 0;
    }
    const char *p1 = s1;
    const char *p2 = s2;
    while((*p1 != '\0') && (*p2 != '\0') && (*p1 == *p2))
        { p1++; p2++;}
    if((*p1) < (*p2))
        return -1024;
    if((*p1) > (*p2))
        return 1024;
    return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented!");
}

void *memset(void *s, int c, size_t n) {
  unsigned char* p = (unsigned char * )s;
  unsigned char ch = (unsigned char)c;
  int i = 1;
  while(i <= n)
  {
    *p = ch;
    p++;i++;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
    panic("Not Implemented!");
}

void *memcpy(void *out, const void *in, size_t n) {
  if(out == NULL)
  {
    panic("The out ptr is a nullptr!!!");
    return NULL;
  }
  unsigned char* p1 = (unsigned char*)out;
  const char* p2 = (const char*)in;
  int i = 1;
  while(i <= n)
  {
    *p1 = *p2;
    p1++;p2++;i++;
  }
  return out;
}
int memcmp(const void *s1, const void *s2, size_t n) {
 	if((s1 == NULL) || (s2 == NULL))
  {
    panic("The s1 or s2 is a nullptr!!!");
    return 0;
  }
  const char* p1 = (const char*)s1; 
  const char* p2 = (const char*)s2;
  int i = 1;
  while(i <= n)
  {
    if(*p1 < *p2){ return -1024;}
    if(*p1 > *p2){ return 1024;}
    p1++;p2++;i++;
  }
  return 0;
}
#endif
