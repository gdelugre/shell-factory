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

    static constexpr bool isdigit(char c)
    {
        return (c >= '0' && c <= '9');
    }

    static constexpr bool isspace(char c)
    {
        return (c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v');
    }

    static constexpr bool islower(char c)
    {
        return ( c >= 'a' && c <= 'z' );
    }

    static constexpr bool isupper(char c)
    {
        return ( c >= 'A' && c <= 'Z' );
    }

    static constexpr bool isalpha(char c)
    {
        return islower(c) || isupper(c);
    }

    static constexpr bool isalnum(char c)
    {
        return isalpha(c) || isdigit(c);
    }

    static constexpr char toupper(char c)
    {
        return islower(c) ? (c - ('a' - 'A')) : c;
    }

    static constexpr char tolower(char c)
    {
        return isupper(c) ? (c + ('a' - 'A')) : c;
    }

    FUNCTION
    long long int strtoll(const char *nptr, char **endptr, int radix = 10)
    {
        static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
        long long int result = 0;
        int sign = 1;
        unsigned pow = 1;
        unsigned nr_digits = 0;
        auto digit_to_dec = [&digits] (char c) -> unsigned {
            for ( size_t i = 0; i < sizeof(digits) - 1; i++ )
                if ( digits[i] == c )
                    return i;
            return 0;
        };

        while ( isspace(*nptr) )
            nptr++;

        if ( *nptr == '+' )
        {
            nptr++;
            sign = 1;
        }
        else if ( *nptr == '-' )
        {
            nptr++;
            sign = -1;
        }

        if ( radix == 16 && nptr[0] == '0' && nptr[1] == 'x' )
            nptr += 2;

        char *dptr = const_cast<char *>(nptr);
        while ( tolower(*dptr) >= digits[0] && tolower(*dptr) <= digits[radix - 1] )
        {
            nr_digits++;
            dptr++;
        }

        for ( size_t i = 0; i < nr_digits; i++ )
        {
            result += digit_to_dec(tolower(nptr[nr_digits - i - 1])) * pow;
            pow *= radix;
        }

        if ( endptr )
            *endptr = dptr;

        return result * sign;
    }

    FUNCTION
    int atoi(const char *nptr)
    {
        return strtoll(nptr, nullptr);
    }

    template <typename T>
    FUNCTION_NOINLINE
    size_t format_ltoa(T& dest,
                       unsigned long value, int radix,
                       bool upcase,
                       size_t (*output)(T&, const void *, size_t))
    {
        size_t count = 0, nr_digits = 0;
        char str[sizeof(value) * 8]; // Worst case, radix = 2.
        do {
            static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

            long r = value % radix;
            char digit = digits[r];
            if ( upcase )
                digit = toupper(digit);

            str[nr_digits++] = digit;
            value = (value - r) / radix;

        } while ( value != 0 );

        for ( size_t i = 0; i < nr_digits; i++ )
            count += output(dest, &str[nr_digits - i - 1], 1);

        return count;
    }

    template <typename T>
    FUNCTION
    int vformat(T& dest, const char *format, va_list ap, size_t (*output)(T&, const void *, size_t))
    {
        int escape = 0;
        char c;
        char *param_str, param_chr;
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

                    case 'c':
                        param_chr = va_arg(ap, int);
                        result += output(dest, &param_chr, sizeof(param_chr));
                        break;

                    case 'd':
                        param_word = va_arg(ap, unsigned long);
                        result += format_ltoa(dest, param_word, 10, false, output);
                        break;

                    case 's':
                        param_str = va_arg(ap, char *);
                        param_str_sz = strlen(param_str);
                        result += output(dest, param_str, param_str_sz);
                        escape = 0;
                        break;

                    case 'p':
                        static char hex_prefix[] = { '0', 'x' };
                        result += output(dest, hex_prefix, sizeof(hex_prefix));

                    case 'x':
                    case 'X':
                        param_word = va_arg(ap, unsigned long);
                        result += format_ltoa(dest, param_word, 16, (c == 'X'), output);
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

        size_t (*output_func)(char *&, const void *, size_t) =
            [](char *& ptr, const void *buffer, size_t n) -> size_t {
                memcpy(ptr, buffer, n);
                ptr += n;

                return n;
            };
        char *output = str;
        int count = vformat(output, format, ap, output_func);

        str[count] = '\0';
        return count;
    }

    FUNCTION_NOINLINE __attribute__((format (printf, 2, 3)))
    int sprintf(char *str, const char *format, ...)
    {
        int cnt;
        va_list ap;

        va_start(ap, format);
        cnt = vsprintf(str, format, ap);
        va_end(ap);

        return cnt;
    }

    FUNCTION_NOINLINE __attribute__((format (printf, 1, 2)))
    int printf(const char *format, ...)
    {
        int cnt;
        va_list ap;
        BasicStream io = Stdio::output();

        va_start(ap, format);
        cnt = io.vprintf(format, ap);
        va_end(ap);

        return cnt;
    }

    template <typename Io>
    METHOD_NOINLINE __attribute__((format (printf, 2, 3)))
    int Stream<Io>::printf(const char *format, ...)
    {
        int cnt;
        va_list ap;

        va_start(ap, format);
        cnt = vprintf(format, ap);
        va_end(ap);

        return cnt;
    }

    template <typename Io>
    METHOD
    int Stream<Io>::vprintf(const char *format, va_list ap)
    {
        size_t (*output_func)(Stream<Io>&, const void *, size_t) =
            [](Stream<Io>& s, const void *buffer, size_t n) -> size_t {
                return s.write(buffer, n);
            };

        int count = vformat(*this, format, ap, output_func);
        return count;
    }
}

#endif
