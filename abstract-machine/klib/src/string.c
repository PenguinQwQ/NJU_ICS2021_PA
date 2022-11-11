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
  assert((dst != NULL) && (src != NULL));
  char* p1 = dst;
  const char* p2 = src;
  while(*p1 != '\0') p1++;
  while(*p2 != '\0') *p1++ = *p2++;
  *p1='\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
    assert((s1 != NULL) && (s2 != NULL));
    while((*s1 != '\0') && (*s2 != '\0') && (*s1 == *s2))
        { s1++; s2++;}
    if((*s1) < (*s2))
        return -1024;
    if((*s1) > (*s2))
        return 1024;
    return 0;
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
  const char* p1 = (const char*)s1; 
  const char* p2 = (const char*)s2;

  for(int i = 1;i <= n;i++){
    if(*p1 < *p2){ return -1024;}
    if(*p1 > *p2){ return 1024;}
    p1++;p2++;
  }
  return 0;
}
#endif
