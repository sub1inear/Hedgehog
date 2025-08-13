#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define H_VLA_RETURN

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <cmath>
#include <new>
#include <tuple>
#include <initializer_list>
#include <random>

#include <gmpxx.h>
#pragma comment(lib, "lib/libgmp-13.lib")
#pragma comment(lib, "lib/libgmpxx-9.lib")

using h_i8 = int8_t;
using h_u8 = uint8_t;
using h_i16 = int16_t;
using h_u16 = uint16_t;
using h_i32 = int32_t;
using h_u32 = uint32_t;
using h_i64 = int64_t;
using h_u64 = uint64_t;

using h_f32 = float;
using h_f64 = double;

using h_int = mpz_class;
using h_float = mpf_class;

using h_bool = bool;

using h_size_t = size_t;
using h_ssize_t = ptrdiff_t;

using h_time_t = time_t;

// TODO: write own tuple class
#define h_tuple std::tuple

#define H_EOF (-1)

#define H_TO_STR_HELPER(x) #x
#define H_TO_STR(x) H_TO_STR_HELPER(x)
#define H_RUNTIME_ERROR(error, desc) do { puts(error ": " desc "\nFile: " __FILE__ "\nLine: " H_TO_STR(__LINE__)); exit(1); } while (0)
#define H_RUNTIME_ERROR_F(error, fmt, ...) do { printf(error ": " fmt "\nFile: " __FILE__ "\nLine: " H_TO_STR(__LINE__) "\n", __VA_ARGS__ ); exit(1); } while (0)

namespace h_math {
inline h_f32 sin(h_f32 x) {
    return sinf(x);
}

inline h_f64 sin(h_f64 x) {
    return ::sin(x);
}

inline h_f32 cos(h_f32 x) {
    return cosf(x);
}

inline h_f64 cos(h_f64 x) {
    return ::cos(x);
}

inline h_f32 tan(h_f32 x) {
    return tanf(x);
}
inline h_f64 tan(h_f64 x) {
    return ::tan(x);
}

inline h_f32 asin(h_f32 x) {
    return asinf(x);
}
inline h_f64 asin(h_f64 x) {
    return ::asin(x);
}

inline h_f32 acos(h_f32 x) {
    return acosf(x);
}

inline h_f64 acos(h_f64 x) {
    return ::acos(x);
}

inline h_f32 atan(h_f32 x) {
    return atanf(x);
}

inline h_f64 atan(h_f64 x) {
    return ::atan(x);
}

inline h_f32 atan2(h_f32 y, h_f32 x) {
    return atan2f(y, x);
}

inline h_f64 atan2(h_f64 y, h_f64 x) {
    return ::atan2(y, x);
}

inline h_f32 sqrt(h_f32 x) {
    return sqrtf(x);
}

inline h_f64 sqrt(h_f64 x) {
    return ::sqrt(x);
}

inline h_float sqrt(h_float x) {
    return ::sqrt(x);
}

inline h_f32 hypot(h_f32 x, h_f32 y) {
    return hypotf(x, y);
}

inline h_f64 hypot(h_f64 x, h_f64 y) {
    return ::hypot(x, y);
}

inline h_float hypot(h_float x, h_float y) {
    return ::hypot(x, y);
}

inline h_f32 sinh(h_f32 x) {
    return sinhf(x);
}

inline h_f64 sinh(h_f64 x) {
    return ::sinh(x);
}

inline h_f32 cosh(h_f32 x) {
    return coshf(x);
}

inline h_f64 cosh(h_f64 x) {
    return ::cosh(x);
}

inline h_f32 tanh(h_f32 x) {
    return tanhf(x);
}
inline h_f64 tanh(h_f64 x) {
    return ::tanh(x);
}

inline h_f32 log(h_f32 x) {
    return logf(x);
}

inline h_f64 log(h_f64 x) {
    return ::log(x);
}

inline h_f32 log10(h_f32 x) {
    return log10f(x);
}

inline h_f64 log10(h_f64 x) {
    return ::log10(x);
}

inline h_f32 log2(h_f32 x) {
    return log2f(x);
}

inline h_f64 log2(h_f64 x) {
    return ::log2(x);
}

inline h_f32 floor(h_f32 x) {
    return floorf(x);
}

inline h_f64 floor(h_f64 x) {
    return ::floor(x);
}

h_float floor(h_float x) {
    return ::floor(x);
}

h_f32 ceil(h_f32 x) {
    return ceilf(x);
}

h_f64 ceil(h_f64 x) {
    return ::ceil(x);
}

h_float ceil(h_float x) {
    return ::ceil(x);
}

template <typename T>
T abs(T x) {
    return x < 0 ? -x : x;
}

template <>
h_f32 abs(h_f32 x) {
    return ::fabsf(x);
}

template <>
h_f64 abs(h_f64 x) {
    return ::fabs(x);
}

template <>
h_int abs(h_int x) {
    return ::abs(x);
}

template <typename T>
T min(T a, T b) {
    return a < b ? a : b;
}

template <typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

}

class h_char {
public:
    unsigned char c;
    
    constexpr h_char();
    constexpr h_char(unsigned char c);
    operator unsigned char();

    h_bool is_alpha();
    h_bool is_digit();
    h_bool is_alnum();
    h_bool is_cntrl();
    h_bool is_graph();
    h_bool is_upper();
    h_bool is_lower();
    h_bool is_print();
    h_bool is_punct();
    h_bool is_space();
    h_bool is_xdigit();

    void to_upper();
    void to_lower();
};

template <typename T, h_ssize_t S>
class h_sref {
public:
    T *_data;

    h_sref(T (&data)[S]);
    h_sref(T *data);
    T operator [](h_ssize_t i);
    T *ptr();
    constexpr h_ssize_t size();
};

template <typename T>
class h_uref {
public:
    T *_data;
    h_ssize_t _size;

    template <h_ssize_t S>
    h_uref(T (&data)[S]);
    h_uref(T *data, h_ssize_t size);
    T operator [](h_ssize_t i);
    T *ptr();
    h_ssize_t size();
};

template <typename T>
class h_list;

template <typename T, h_ssize_t S>
class h_array {
public:
    T _data[S];

    h_array();
    h_array(std::initializer_list<T> array);

    template <h_ssize_t S2>
    h_array(const h_array<T, S2> &array);

    operator h_sref<T, S>();
    operator h_uref<T>();

    T operator[](h_ssize_t i);

    h_ssize_t find(T item);
    h_ssize_t find_reverse(T item);
    h_list<h_ssize_t> find_all(T item);

    T *ptr();
    constexpr h_ssize_t size();
};

template <typename T>
class h_list {
public:
    T *_data;
    h_ssize_t _size;
    h_ssize_t _capacity;

    h_list();
    h_list(h_ssize_t size);
    h_list(std::initializer_list<T> list);
    h_list(const h_list &list);
    h_list(h_list &&list);
    ~h_list();

    h_list &operator=(const h_list &list);
    h_list &operator=(const std::initializer_list<T> &list);
    h_list &operator=(h_list &&list);

    operator h_uref<T>();
    T operator[](h_ssize_t i);

    void append(T item);
    void append(h_list<T> list);

    h_ssize_t find(T item);
    h_ssize_t find_reverse(T item);
    h_list<h_ssize_t> find_all(T item);

    void remove(T item);
    void remove_reverse(T item);
    void remove_all(T item);

    void reserve(h_ssize_t capacity);

    h_uref<T> data();
    T *ptr();
    h_ssize_t size();
    h_ssize_t capacity();
protected:
    void malloc_data();
    void reserve_internal(h_ssize_t capacity);
};

class h_str : public h_list<h_char>  {
public:
    using base = h_list<h_char>;
    using base::base;

    template <h_ssize_t S>
    h_str(const char (&str)[S]);

    h_ssize_t find(h_str str);
    h_ssize_t find_reverse(h_str str);
    h_list<h_ssize_t> find_all(h_str str);

    void replace(h_str str1, h_str str2);
    void replace_reverse(h_str str1, h_str str2);
    void replace_all(h_str str1, h_str str2);

    void to_upper();
    void to_lower();
};

class h_file {
    FILE *file;
public:
    h_file();

    h_file(const char *filename, const char *mode);
    h_file(h_uref<h_char> filename, const char *mode);
    h_file(const char *filename, h_uref<h_char> mode);
    h_file(h_uref<h_char> filename, h_uref<h_char> mode);

    constexpr h_file(FILE *file);

    ~h_file();

    h_i32 seek_set(h_i32 offset);
    h_i32 seek_cur(h_i32 offset);
    h_i32 seek_end(h_i32 offset);
    h_i32 tell();

    h_tuple<h_char, h_bool> read();

    H_VLA_RETURN
    template <typename T>
    h_u64 read(h_uref<T> out);

    H_VLA_RETURN
    h_bool read(h_uref<h_char> out);

    H_VLA_RETURN
    template <typename T>
    h_u64 read(h_uref<T> out, h_i32 count);

    H_VLA_RETURN
    h_bool read(h_uref<h_char> out, h_i32 count);

    template <typename ...A>
    h_i32 scan(const char *fmt, A... args);

    template <typename ...A>
    h_i32 scan(h_uref<h_char> fmt, A... args);

    template <typename T>
    h_u64 write(const h_uref<T> in );

    h_i32 print(h_char c);
    h_i32 print(const char *str);
    h_i32 print(h_uref<h_char> str);
    template <typename ...A>
    h_i32 print(const char *fmt, A... args);
    template <typename ...A>
    h_i32 print(h_uref<h_char> fmt, A... args);

    h_i32 println(h_char c);
    h_i32 println(const char *str);
    h_i32 println(h_uref<h_char> str);
    template <typename ...A>
    h_i32 println(const char *fmt, A... args);
    template <typename ...A>
    h_i32 println(h_uref<h_char> fmt, A... args);

    h_i32 unget(h_char c);

    h_bool flush();
    h_bool error();
    h_bool eof();

    void clear_flags();
    void rewind();
    
    static h_bool rename(const char *old_filename, const char *new_filename) {
        return ::rename(old_filename, new_filename);
    }
    
    static h_bool rename(h_uref<h_char> old_filename, const char *new_filename) {
        return rename((char *)old_filename.ptr(), new_filename);
    }
    
    static h_bool rename(const char *old_filename, h_uref<h_char> new_filename) {
        return rename(old_filename, (char *)new_filename.ptr());
    }

    static h_bool rename(h_uref<h_char> old_filename, h_uref<h_char> new_filename) {
        return rename((char *)old_filename.ptr(), (char *)new_filename.ptr());
    }

    static h_bool remove(const char *filename) {
        return ::remove(filename);
    }

    static h_bool remove(h_uref<h_char> filename) {
        return remove((char *)filename.ptr());
    }
};

#define H_STDOUT ((h_file)stdout)
#define H_STDERR ((h_file)stderr)
#define H_STDIN ((h_file)stdin)

class h_random {
public:
    h_u32 seed;
    
    h_random();
    constexpr h_random(h_u32 seed);

    h_u32 random();
    h_u32 random(h_u32 max);
    h_u32 random(h_u32 min, h_u32 max);
    h_i32 random(h_i32 min, h_i32 max);
};

class h_time {
public:
    struct tm time_data;
    h_time();
    h_time(h_time_t time);
    
    h_i32 second();
    h_i32 minute();
    h_i32 hour();
    h_i32 day();
    h_i32 month();
    h_i32 year();
    h_i32 week_day();
    h_i32 year_day();
    bool is_daylight_savings();
    
    H_VLA_RETURN
    h_i32 format(h_uref<h_char> out, const char *fmts);

    H_VLA_RETURN
    h_i32 format(h_uref<h_char> out, h_uref<h_char> fmt);

    static h_time_t time();
};

inline h_i32 print(h_char c) {
    return putchar(c) == EOF ? H_EOF : 1;
}

inline h_i32 print(const char *str) {
    h_i32 count;
    for (count = 0; *str != '\0'; count++)  {
        if (putchar(*str++) == EOF) {
            return H_EOF;
        }
    }
    return count;
}

template <typename ...A>
inline h_i32 print(const char *fmt, A... args) {
    h_i32 result = gmp_printf(fmt, args...);
    return result < 0 ? H_EOF : result;
}

inline h_i32 print(h_uref<h_char> str) {
    return print((char *)str.ptr());
}

template <typename ...A>
inline h_i32 print(h_uref<h_char> fmt, A... args) {
    return print((char *)fmt.ptr(), args...);
}

inline h_i32 println(h_char c) {
    h_i32 result_c = putchar(c);
    h_i32 result_ln = putchar('\n');
    return result_c == EOF || result_ln == EOF ? H_EOF : 2;
}

inline h_i32 println(const char *str) {
    h_i32 count;
    for (count = 0; *str != '\0'; count++)  {
        if (putchar(*str++) == EOF) {
            return H_EOF;
        }
    }
    if (putchar('\n') == EOF) {
        return H_EOF;
    }
    return count + 1;
}

template <typename ...A>
inline h_i32 println(const char *fmt, A... args) {
    h_i32 result_f = gmp_printf(fmt, args...);
    h_i32 result_l = putchar('\n');

    h_i32 result = H_EOF;
    if (result_f >= 0) {
        result = result_f;
        if (result_l != EOF) {
            result++;
        }
    }
    return result;
}

inline h_i32 println(h_uref<h_char> str) {
    return println((char *)str.ptr());
}

template <typename ...A>
inline h_i32 println(h_uref<h_char> fmt, A... args) {
    return println((char *)fmt.ptr(), args...);
}

inline h_tuple<h_char, h_bool> read() {
    h_i32 result = getchar();
    return { result, result == EOF };
}

H_VLA_RETURN
inline h_bool read(h_uref<h_char> out) {
    return fgets((char *)out.ptr(), (h_i32)out.size(), stdout) == nullptr;
}

template <typename ...A>
inline h_i32 scan(const char *fmt, A... args) {
    return scanf(fmt, args...);
}

template <typename ...A>
inline h_i32 scan(h_uref<h_char> fmt, A... args) {
    return scan((char *)fmt.ptr());
}

#include "hstdlib.ipp"