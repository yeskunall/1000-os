#pragma once

#define va_arg   __builtin_va_arg
#define va_end   __builtin_va_end
#define va_list  __builtin_va_list
#define va_start __builtin_va_start

void printf(const char *fmt, ...);
