#ifndef STRING_HELPER_H_
#define STRING_HELPER_H_

#include <cstdarg>

namespace Pico {

    template <typename T = char>
    class BasicString {
        public:
            CONSTRUCTOR BasicString(T *str) : chars(str) {}
            METHOD size_t length();
            METHOD BasicString<T>& copy(BasicString<T> const& src);
            METHOD BasicString<T>& copy(BasicString<T> const& src, size_t max_size);
            METHOD BasicString<T>& concat(BasicString<T> const& src);
            METHOD BasicString<T>& concat(BasicString<T> const& src, size_t max_size);

            METHOD T& operator[](unsigned index) const { return chars[index]; }
            METHOD bool operator ==(BasicString<T> const& other);
            METHOD T* pointer() const { return chars; }
            METHOD BasicString<T>& operator =(BasicString<T> const& str) { return copy(str); }

        private:
            T *chars;
    };

    typedef BasicString<char> String;
    typedef BasicString<wchar_t> WideString;

    template <typename T>
    METHOD
    size_t BasicString<T>::length()
    {
        #if defined(__i386__) || defined(__amd64__)
        if ( Options::use_builtins ) {
           return BUILTIN(strlen)(chars); // Optimize to repnz
        }
        #endif

        size_t len = 0;
        while ( chars[len] )
            len++;
        return len;
    }

    template <typename T>
    METHOD
    BasicString<T>& BasicString<T>::copy(BasicString<T> const& src)
    {
        T c;
        unsigned i = 0;

        while ( (c = src[i]) != 0 )
            chars[i++] = c;

        chars[i] = 0;
        return *this;
    }

    template <typename T>
    METHOD
    BasicString<T>& BasicString<T>::copy(BasicString<T> const& src, size_t max_size)
    {
        T c;
        unsigned i = 0;

        while ( max_size-- && (c = src[i]) != 0 )
            chars[i++] = c;

        chars[i] = 0;
        return *this;
    }

    template <typename T>
    METHOD
    BasicString<T>& BasicString<T>::concat(BasicString<T> const& src)
    {
        BasicString<T>(chars + length()).copy(src);

        return *this;
    }

    template <typename T>
    METHOD
    BasicString<T>& BasicString<T>::concat(BasicString<T> const& src, size_t max_size)
    {
        size_t length = this->length();

        if ( max_size > length )
            BasicString<T>(chars + length).copy(src, max_size - length);

        return *this;
    }

    template <typename T>
    METHOD
    bool BasicString<T>::operator ==(BasicString<T> const& other)
    {
        T c1, c2;
        unsigned i = 0;

        do
        {
            c1 = chars[i];
            c2 = other[i];
            i++;

            if ( c1 != c2 )
                break;
        } while ( c1 && c2 );

        return ( c1 == c2 );
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
        auto digit_to_dec = [] (char c) -> unsigned {
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

                    case 'd':
                    {
                        unsigned long param_word = va_arg(ap, unsigned long);
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
}

#endif
