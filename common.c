#include "common.h"

void sbi_console_putchar(char c);

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *) dst;
    const uint8_t *s = (const uint8_t *) src;

    while (n--)
        *d++ = *s++;

    return dst;
}

void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;

    while(n--)
        *p++ = c;

    return buf;
}

void printf(const char *fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);

    while(*fmt) {
        if (*fmt == '%') {
            fmt++;

            switch (*fmt) {
                case '\0':
                    sbi_console_putchar('%');

                    goto end;
                case '%':
                    sbi_console_putchar('%');

                    break;
                // Print a NULL-terminated (\0) string.
                case 's': {
                    const char *s = va_arg(vargs, const char *);

                    while(*s) {
                        sbi_console_putchar(*s);
                        s++;
                    }

                    break;
                }
                // Print an integer in decimal.
                case 'd': {
                    int value = va_arg(vargs, int);
                    unsigned magnitude = value;

                    if (value < 0) {
                        sbi_console_putchar('-');
                        magnitude = -magnitude;
                    }

                    unsigned divisor = 1;
                    while (magnitude / divisor > 9)
                        divisor *= 10;

                    while (divisor > 0) {
                        sbi_console_putchar('0' + magnitude / divisor);
                        magnitude %= divisor;
                        divisor /= 10;
                    }

                    break;
                }

                // Print an integer in hexadecimal.
                case 'x': {
                    unsigned value = va_arg(vargs, unsigned);

                    for (int i = 7; i >= 0; i--) {
                        unsigned nibble = (value >> (i * 4)) & 0xf;

                        sbi_console_putchar("0123456789abcdef"[nibble]);
                    }
                }
            }
        } else {
            sbi_console_putchar(*fmt);
        }

        fmt++;
    }

    end:
        va_end(vargs);
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2)
            break;

        s1++; s2++;
    }

    // The casting to unsigned char * when comparing is done to conform to the POSIX specification.
    // https://www.man7.org/linux/man-pages/man3/strcmp.3.html#:~:text=both%20interpreted%20as%20type%20unsigned%20char
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strcpy(char *dst, const char *src) {
    char *d = dst;

    while (*src)
        *d++ = *src++;
    *d = '\0';

    return dst;
}
