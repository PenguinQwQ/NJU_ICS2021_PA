#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	assert(s != NULL);
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
    assert((dst != NULL) && (src != NULL));
    char* beg = dst;
    while(*src != '\0') *dst++ = *src++;
    *dst = '\0';
  return beg;
}

char *strncpy(char *dst, const char *src, size_t n) {
    panic("Not Implemented!");
}


char *strcat(char *dst, const char *src) {
  assert(dst != NULL);
  assert(src != NULL);
  char* pd = dst;
  const char* ps = src;
  while(*pd != '\0') pd++;
  while(*ps != '\0') *pd++ = *ps++;
  *pd='\0';
  return dst;
//  panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
    assert((s1 != NULL) && (s2 != NULL));
    while((*s1 != '\0') && (*s2 != '\0') && (*s1 == *s2))
        s1++,s2++;
    int flag = 0;
    if((*s1) < (*s2))
       {
        flag = -8;
       }
    if((*s1) > (*s2))
        {
        flag = 8;
        }
   	return flag;
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
  unsigned char* p = (unsigned char* )s;
  unsigned char ch = (unsigned char)c;
  for(int i = 1;i <= n && (p != NULL);i++){ *p = ch; p++;}
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
    panic("Not Implemented!");
}

void *memcpy(void *out, const void *in, size_t n) {
  assert(out != NULL);
  unsigned char* p1 = (unsigned char*)out;
  const char* p2 = (const char*)in;
  for(int i = 1;i <= n && (p1 != NULL) && (p2 != NULL);i++) *p1++ = *p2++;
  return out;
}
int memcmp(const void *s1, const void *s2, size_t n) {
 	assert((s1 != NULL) && (s2 != NULL));
  int flag = 0;
  const char* p1 = (const char*)s1; 
  const char* p2 = (const char*)s2;

  for(int i = 1;i <= n;i++){
    if(*p1 < *p2){ flag = -8; break; }
    if(*p1 > *p2){ flag = 8;  break; }
    p1++;p2++;
  }
  return flag;
}
#endif
