#ifndef STRING_HELPER_H_
#define STRING_HELPER_H_

#include <cstdarg>

namespace Pico {

    template <typename T>
    class BasicString
    {
        public:
            template <unsigned N>
            CONSTRUCTOR BasicString(const T (&src)[N])
            {
                chars = const_cast<T *>(src);
                max_size = size = N;
            }

            CONSTRUCTOR BasicString() : BasicString("") {}
            CONSTRUCTOR BasicString(size_t buffer_size)
            {
                chars = new T[buffer_size];
                chars[0] = 0;
                needs_dealloc = true;

                size = 0;
                max_size = buffer_size;
            }

            CONSTRUCTOR BasicString(T* src)
            {
                chars = src;
                max_size = size = length(chars) + 1;
            }

            CONSTRUCTOR BasicString(T* src, size_t buffer_size)
            {
                chars = src;
                chars[buffer_size - 1] = '\0';

                max_size = buffer_size;
                size = length(chars) + 1;
            }

            DESTRUCTOR ~BasicString()
            {
                if ( needs_dealloc )
                    delete chars;
            }

            METHOD T *pointer() const { return chars; }
            METHOD size_t length() const { return size - 1; }

            METHOD T& operator[](unsigned index) const {
                assert(index < size);
                return chars[index];
            }

            template <unsigned N>
            METHOD BasicString<T>& operator =(const T (&str)[N]);
            METHOD BasicString<T>& operator =(BasicString<T> const& str);
            METHOD bool operator ==(BasicString<T> const& other) const;
            METHOD BasicString<T>& operator +=(BasicString<T> const& str);
            METHOD BasicString<T> operator +(BasicString const& str);

            // Static functions.
            template <unsigned N>
            FUNCTION PURE size_t length(const T (&s)[N]) { return N-1; }
            FUNCTION PURE size_t length(T *s);
            FUNCTION PURE bool equals(const T *s1, const T *s2);
            FUNCTION T *copy(T *dest, const T *src);
            FUNCTION T *copy(T *dest, const T *src, size_t n);
            FUNCTION T *concat(T *dest, const T *src);
            FUNCTION T *concat(T *dest, const T *src, size_t n);

        private:
            METHOD size_t free_space() const { return max_size - size; }

            T *chars;
            size_t size;
            size_t max_size;
            bool needs_dealloc = false;
    };

    // Copy assignment constructors.
    template <typename T>
    template <unsigned N>
    METHOD
    BasicString<T>& BasicString<T>::operator =(const T (&str)[N])
    {
        if ( needs_dealloc )
            delete chars;

        chars = const_cast<T *>(str);
        max_size = size = N;

        return *this;
    }

    template <typename T>
    METHOD
    BasicString<T>& BasicString<T>::operator =(BasicString<T> const& str)
    {
        if ( max_size < str.length() + 1 )
        {
            if ( needs_dealloc )
                delete chars;

            chars = new T[str.length() + 1];
            max_size = str.length() + 1;
            needs_dealloc = true;
        }

        BasicString<T>::copy(chars, str.pointer());
        size = str.length() + 1;

        return *this;
    }

    template <typename T>
    METHOD
    BasicString<T>& BasicString<T>::operator +=(BasicString<T> const& str)
    {
        size_t total_length = this->length() + str.length();

        if ( max_size < total_length - 1 )
        {
            T *old_chars = chars;

            chars = new T[total_length + 1];
            max_size = total_length + 1;

            BasicString<T>::copy(chars, old_chars);

            if ( needs_dealloc )
                delete old_chars;

            needs_dealloc = true;
        }

        BasicString<T>::concat(chars, str.pointer());
        size += str.length();

        return *this;
    }

    template <typename T>
    METHOD
    BasicString<T> BasicString<T>::operator +(BasicString<T> const& str)
    {
        String result(this->length() + str.length() + 1);

        result = *this;
        result += str;

        return result;
    }

    template <typename T>
    METHOD
    bool BasicString<T>::operator ==(BasicString<T> const& other) const
    {
        return BasicString<T>::equals(chars, other.chars);
    }

    template <>
    METHOD PURE
    size_t BasicString<char>::length(char *s)
    {
        if ( Options::use_builtins )
           return BUILTIN(strlen)(s);

        return tstrlen(s);
    }

    template <typename T>
    METHOD PURE
    size_t BasicString<T>::length(T *s)
    {
        return tstrlen(s);
    }

    template <>
    METHOD
    bool BasicString<char>::equals(const char *s1, const char *s2)
    {
        if ( Options::use_builtins )
            return BUILTIN(strcmp)(s1, s2) == 0;

        return tstrcmp(s1, s2) == 0;
    }

    template <typename T>
    METHOD
    bool BasicString<T>::equals(const T *s1, const T *s2)
    {
        return tstrcmp(s1, s2) == 0;
    }

    template <>
    METHOD
    char *BasicString<char>::copy(char *dest, const char *src)
    {
        if ( Options::use_builtins )
            return BUILTIN(strcpy)(dest, src);

        return tstrcpy(dest, src);
    }

    template <typename T>
    METHOD
    T *BasicString<T>::copy(T *dest, const T *src)
    {
        return tstrcpy(dest, src);
    }

    template <>
    METHOD
    char *BasicString<char>::copy(char *dest, const char *src, size_t n)
    {
        if ( Options::use_builtins )
           return BUILTIN(strncpy)(dest, src, n);

        return tstrncpy(dest, src, n);
    }

    template <typename T>
    METHOD
    T *BasicString<T>::copy(T *dest, const T *src, size_t n)
    {
        return tstrncpy(dest, src, n);
    }

    template <>
    METHOD
    char *BasicString<char>::concat(char *dest, const char *src)
    {
        if ( Options::use_builtins )
            return BUILTIN(strcat)(dest, src);

        return tstrcat(dest, src);
    }

    template <typename T>
    METHOD
    T *BasicString<T>::concat(T *dest, const T *src)
    {
        return tstrcat(dest, src);
    }

    template <>
    METHOD
    char *BasicString<char>::concat(char *dest, const char *src, size_t n)
    {
        if ( Options::use_builtins )
            return BUILTIN(strncat)(dest, src, n);

        return tstrncat(dest, src, n);
    }

    template <typename T>
    METHOD
    T *BasicString<T>::concat(T *dest, const T *src, size_t n)
    {
        return tstrncat(dest, src, n);
    }

    using String = BasicString<char>;
    using WideString = BasicString<wchar_t>;

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

    template <typename Io>
    METHOD
    ssize_t Stream<Io>::write(String const& str)
    {
        return write(str.pointer(), str.length());
    }
}

#endif
