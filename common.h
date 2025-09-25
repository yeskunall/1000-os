#pragma once

#define true  1
#define false 0
#define NULL  ((void *) 0)

// Uncommenting this causes a compilation error. Why?
// typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint32_t size_t;
typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

#define align_up(value, align)   __builtin_align_up(value, align)
#define is_aligned(value, align) __builtin_is_aligned(value, align)
#define offsetof(type, member)   __builtin_offsetof(type, member)
#define va_arg   __builtin_va_arg
#define va_end   __builtin_va_end
#define va_list  __builtin_va_list
#define va_start __builtin_va_start

void *memset(void *buf, char c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
void printf(const char *fmt, ...);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dst, const char *src);
