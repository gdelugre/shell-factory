#ifndef STRING_HELPER_H_
#define STRING_HELPER_H_

#include <cstdarg>

namespace Pico {

    FUNCTION
    size_t strlen(const char *s)
    {
        size_t len = 0;
        while ( s[len] )
            len++;
        return len;
    }

    FUNCTION
    char *strcpy(char *dest, const char *src)
    {
        char c;
        char *out = dest;

        while ( (c = *src++) != '\0' )
            *out++ = c;

        *out = '\0';
        return dest;
    }

    FUNCTION
    char *strcat(char *dest, const char *src)
    {
        strcpy(&dest[strlen(dest)], src);
        return dest;
    }

    FUNCTION
    char *strncpy(char *dest, const char *src, size_t n)
    {
        char c;
        char *out = dest;

        while ( n != 0 && (c = *src++) != '\0' )
        {
            *out++ = c;
            n--;
        }

        if ( n > 0 )
            *out = '\0';

        return dest;
    }

    FUNCTION
    int strcmp(const char *s1, const char *s2)
    {
        char c1 = 0, c2 = 0;

        do
        {
            c1 = *s1++;
            c2 = *s2++;

            if ( c1 != c2 )
                break;
        } while ( c1 && c2 );

        if ( c1 == c2 )
            return 0;
        else
            return (c1 < c2 ? -1 : +1);
    }

    FUNCTION
    char *strstr(const char *haystack, const char *needle)
    {
        char *ptr = (char *) haystack;
        while ( *ptr )
        {
            if ( strcmp(ptr, needle) == 0 )
                return ptr;
            ptr++;
        }

        return NULL;
    }

    FUNCTION
    int isdigit(int c)
    {
        return (c >= '0' && c <= '9');
    }

    FUNCTION
    int toupper(int c)
    {
        if ( c >= 'a' && c <= 'z' )
            c -= ('a' - 'A');

        return c;
    }

    FUNCTION
    int tolower(int c)
    {
        if ( c >= 'A' && c <= 'Z' )
            c += ('a' - 'A');

        return c;
    }

    FUNCTION
    unsigned int atoi(const char *nptr)
    {
        char *ptr = (char *) nptr;
        unsigned int num_digits = 0;
        unsigned int res = 0;
        unsigned int pow = 1;
        unsigned int i;

        while ( isdigit(*ptr++) )
            num_digits++;

        for ( i = 0; i < num_digits; i++ )
        {
            res += (nptr[num_digits - i - 1] - '0') * pow;
            pow *= 10;
        }

        return res;
    }

    template <typename T>
    FUNCTION
    size_t format_word_to_hex(T& dest, unsigned long w, bool upcase, size_t (*output)(T&, void *, size_t))
    {
        static char hex_chars[] = "0123456789abcdef";
        const size_t bitsize = sizeof(w) * 8;
        int off = bitsize - 4;
        size_t count = 0;
        char c;

        while ( off != 0 && w >> off == 0 )
            off -= 4;

        if ( off == 0 )
        {
            output(dest, &hex_chars[0], 1);
            count++;
        }
        else
        {
            while ( off >= 0 )
            {
                c = hex_chars[(w >> off) & 0xF];
                if ( upcase )
                    c = toupper(c);

                output(dest, &c, 1);
                off -= 4;
                count++;
            }
        }

        return count;
    }

    template <typename T>
    FUNCTION
    int vformat(T& dest, const char *format, va_list ap, size_t (*output)(T&, void *, size_t))
    {
        int escape = 0;
        char c;
        char *param_str;
        size_t param_str_sz;
        unsigned long param_word;
        int result = 0;

        while ( *format )
        {
            c = *format++;
            if ( !escape && c != '%' )
            {
                result += output(dest, &c, 1);
            }
            else if ( !escape && c == '%' )
            {
                escape = 1;
            }
            else
            {
                switch ( c )
                {
                    case '%':
                        result += output(dest, &c, 1); break;

                    case 's':
                        param_str = va_arg(ap, char *);
                        param_str_sz = strlen(param_str);
                        result += output(dest, param_str, param_str_sz);
                        escape = 0;
                        break;

                    case 'p':
                    case 'x':
                    case 'X':
                        param_word = va_arg(ap, unsigned long);
                        result += format_word_to_hex(dest, param_word, (c == 'X'), output);
                        break;

                    default:
                        break;
                }

                escape = 0;
            }
        }

        return result;
    }

    FUNCTION
    int vsprintf(char *str, const char *format, va_list ap)
    {

        size_t (*output_func)(char *&, void *, size_t) =
            [](char *& ptr, void *buffer, size_t n) -> size_t {
                memcpy(ptr, buffer, n);
                ptr += n;

                return n;
            };
        char *output = str;
        int count = vformat(output, format, ap, output_func);

        str[count] = '\0';
        return count;
    }

    FUNCTION_NOINLINE
    int sprintf(char *str, const char *format, ...)
    {
        int cnt;
        va_list ap;

        va_start(ap, format);
        cnt = vsprintf(str, format, ap);
        va_end(ap);

        return cnt;
    }

    FUNCTION_NOINLINE
    int printf(const char *format, ...)
    {
        int cnt;
        va_list ap;
        Stream io = Stream::standard_output();

        va_start(ap, format);
        cnt = io.vprintf(format, ap);
        va_end(ap);

        return cnt;
    }

    METHOD_NOINLINE
    int Stream::printf(const char *format, ...)
    {
        int cnt;
        va_list ap;

        va_start(ap, format);
        cnt = vprintf(format, ap);
        va_end(ap);

        return cnt;
    }

    METHOD
    int Stream::vprintf(const char *format, va_list ap)
    {
        size_t (*output_func)(Stream&, void *, size_t) =
            [](Stream& s, void *buffer, size_t n) -> size_t {
                return s.out(buffer, n);
            };

        int count = vformat(*this, format, ap, output_func);
        flush();
        return count;
    }
}

#endif
