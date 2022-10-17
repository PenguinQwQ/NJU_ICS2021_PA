#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    size_t len = 0;
    //If s is a nullptr, then we should return 0;
    if(s == NULL)
      return 0;
    while(*s != '\0')
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
  size_t len_dst = strlen(dst), len_src = strlen(src);
  if(len_dst + 1 < ((n < len_src + 1) ? n : len_src + 1))
      panic("The Destination String is too short!! String Overflow!");
  if(len_src + 1 <= n) //包括上'\0'，还是小于count，就整个复制。否则，只复制count个
  {
    for (int i = 0 ; i <= len_src ; i++) //Including the '\0', so it ends with len_src
      	dst[i] = src[i];
  }
  else
  {
    for (int i = 0 ; i <= n - 1; i++) //Including the '\0', so it ends with len_src
      	dst[i] = src[i];   
  }
  char* ptr = dst;
  return ptr;
}

char *strcat(char *dst, const char *src) {
  	size_t len_dst = strlen(dst), len_src = strlen(src);
    if(len_dst + len_src + 1 > sizeof(dst))
        panic("strcat overflow! src+dst > sizeof dst!");
    for (int i = len_dst ; i <= len_dst + len_src ; i++)
    	dst[i] = src[i - len_dst];
    return dst;
}

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
    for (int i = 0 ; i <= n - 1 ; i++)
    	{
        *((int *)(s + i * 4)) = c;
      }
    return s;
}

void *memmove(void *dst, const void *src, size_t n) {
    for (int i = 0 ; i <= n - 1 ; i++)
    	{
      *((int *)(dst + i * 4)) = *((int *)(src + i * 4)); 	
      }
    return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
    for (int i = 0 ; i <= n - 1 ; i++)
    	{
      *((int *)(out + i * 4)) = *((int *)(in + i * 4));
      } 
    return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
 	int len_s1 = strlen(s1), len_s2 = strlen(s2), i = 0;
    while((i <= len_s1) && (i <= len_s2) && (i <= n - 1) && ((*((int *)(s1 + 4 * i))) == (*((int *)(s2 + 4 * i)))))
       { 
        i++;
       }
   	return *((int *)(s1 + 4 * i)) - *((int *)(s2 + 4 * i));
}

#endif
