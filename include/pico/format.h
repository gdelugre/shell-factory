#ifndef PICOLIB_FORMAT_H_
#define PICOLIB_FORMAT_H_

namespace Pico {

    template <typename T>
    FUNCTION_NOINLINE
    size_t format_ltoa(T& dest,
                       unsigned long value, int radix,
                       bool upcase,
                       char pad, int pad_sz,
                       size_t (*output)(T&, const void *, size_t))
    {
        size_t count = 0;
        int nr_digits = 0;
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

        for ( int i = 0; i < (pad_sz - nr_digits); i++ )
            count += output(dest, &pad, sizeof(pad));

        for ( int i = 0; i < nr_digits; i++ )
            count += output(dest, &str[nr_digits - i - 1], 1);

        return count;
    }

    template <typename T>
    FUNCTION
    int vformat(T& dest, const char *format, va_list ap, size_t (*output)(T&, const void *, size_t))
    {
        char *fmt = const_cast<char *>(format);
        int escape = 0;
        char c;
        char pad = ' ';
        long pad_sz = 0;
        int result = 0;

        while ( *fmt )
        {
            c = *fmt++;
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
                if ( isdigit(c) )
                {
                    if ( c == '0' )
                        pad = '0';
                    else
                        pad_sz = strtoll(fmt-1, &fmt, 10);
                    continue;
                }

                switch ( c )
                {
                    case '%':
                        result += output(dest, &c, 1); break;

                    case 'c':
                    {
                        char param_chr = va_arg(ap, int);
                        result += output(dest, &param_chr, sizeof(param_chr));
                        break;
                    }

                    case 'i':
                    case 'd':
                    case 'u':
                    {
                        int param_word = va_arg(ap, int);
                        if ( c != 'u' && param_word < 0 ) {
                            result += output(dest, "-", 1);
                            param_word = -param_word;
                        }

                        result += format_ltoa(dest, param_word, 10, false, pad, pad_sz, output);
                        break;
                    }

                    case 's':
                    {
                        String param_str = va_arg(ap, char *);
                        size_t param_str_sz = param_str.length();

                        for ( int i = 0; i < pad_sz - static_cast<int>(param_str_sz); i++ )
                            result += output(dest, &pad, sizeof(pad));
                        result += output(dest, param_str.pointer(), param_str_sz);
                        escape = 0;
                        break;
                    }

                    case 'p':
                        static char hex_prefix[] = { '0', 'x' };
                        result += output(dest, hex_prefix, sizeof(hex_prefix));

                    case 'x':
                    case 'X':
                    {
                        unsigned long param_word = va_arg(ap, unsigned long);
                        result += format_ltoa(dest, param_word, 16, (c == 'X'), pad, pad_sz, output);
                        break;
                    }

                    default:
                        break;
                }

                pad = ' ';
                pad_sz = 0;
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
                Memory::copy(ptr, buffer, n);
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

    template <typename Io>
    METHOD
    ssize_t Stream<Io>::write(String const& str)
    {
        return write(str.pointer(), str.length());
    }
}

#endif
