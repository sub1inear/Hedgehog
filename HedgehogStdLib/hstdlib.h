#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <new>
#include <initializer_list>

#define H_TO_STR_HELPER(x) #x
#define H_TO_STR(x) H_TO_STR_HELPER(x)
#define H_RUNTIME_ERROR(error, desc) do { puts(error ": " desc "\nFile: " __FILE__ "\nLine: " H_TO_STR(__LINE__)); exit(1); } while (0)
#define H_RUNTIME_ERROR_F(error, fmt, ...) do { printf(error ": " fmt "\nFile: " __FILE__ "\nLine: " H_TO_STR(__LINE__) "\n", __VA_ARGS__ ); exit(1); } while (0)

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

// TODO: replace with big num
using h_int = h_i64;
using h_uint = h_u64;
using h_float = double;

using h_bool = bool;

namespace h_math {
h_f32 sin(h_f32 x);
h_f64 sin(h_f64 x);
h_float sin(h_float x);

h_f32 cos(h_f32 x);
h_f64 cos(h_f64 x);
h_float cos(h_float x);

h_f32 tan(h_f32 x);
h_f64 tan(h_f64 x);
h_float tan(h_float x);

h_f32 asin(h_f32 x);
h_f64 asin(h_f64 x);
h_float asin(h_float x);

h_f32 acos(h_f32 x);
h_f64 acos(h_f64 x);
h_float acos(h_float x);

h_f32 atan(h_f32 x);
h_f64 atan(h_f64 x);
h_float atan(h_float x);

h_f32 atan2(h_f32 y, h_f32 x);
h_f64 atan2(h_f64 y, h_f64 x);
h_float atan2(h_float y, h_float x);

h_f32 sqrt(h_f32 x);
h_f64 sqrt(h_f64 x);
h_float sqrt(h_float x);

h_f32 hypot(h_f32 x, h_f32 y);
h_f64 hypot(h_f64 x, h_f64 y);
h_float hypot(h_float x, h_float y);

h_f32 sinh(h_f32 x);
h_f64 sinh(h_f64 x);
h_float sinh(h_float x);

h_f32 cosh(h_f32 x);
h_f64 cosh(h_f64 x);
h_float cosh(h_float x);

h_f32 tanh(h_f32 x);
h_f64 tanh(h_f64 x);
h_float tanh(h_float x);

h_f32 log(h_f32 x);
h_f64 log(h_f64 x);
h_float log(h_float x);

h_f32 log10(h_f32 x);
h_f64 log10(h_f64 x);
h_float log10(h_float x);

h_f32 log2(h_f32 x);
h_f64 log2(h_f64 x);
h_float log2(h_float x);

h_f32 floor(h_f32 x);
h_f64 floor(h_f64 x);
h_float floor(h_float x);

h_f32 ceil(h_f32 x);
h_f64 ceil(h_f64 x);
h_float ceil(h_float x);

template <typename T>
T abs(T x);

template <typename T>
T min(T a, T b);

template <typename T>
T max(T a, T b);

}

class h_char {
public:
    unsigned char c;

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

template <typename T, h_i64 S>
class h_sref {
public:
    T *_data;

    h_sref(T (&data)[S]);
    h_sref(T *data);
    T operator [](h_i64 i);
    T *ptr();
    constexpr h_i64 size();
};

template <typename T>
class h_uref {
public:
    T *_data;
    h_i64 _size;

    template <h_i64 S>
    h_uref(T (&data)[S]);
    h_uref(T *data, h_i64 size);
    T operator [](h_i64 i);
    T *ptr();
    h_i64 size();
};

template <typename T>
class h_list;

template <typename T, h_i64 S>
class h_array {
public:
    T _data[S];
    
    template <h_i64 S2>
    h_array(const h_array<T, S2> &array);
    template <h_i64 S2>
    h_array(h_array<T, S2> &&array);

    template <h_i64 S2>
    h_array &operator=(const h_array<T, S2> &list);
    template <h_i64 S2>
    h_array &operator=(h_array<T, S2> &&list);

    operator h_sref<T, S>();
    operator h_uref<T>();

    T operator[](h_i64 i);

    h_i64 find(T item);
    h_i64 find_reverse(T item);
    h_list<h_i64> find_all(T item);

    constexpr h_i64 size();
};

template <typename T>
class h_list {
public:
    T *_data;
    h_i64 _size;
    h_i64 _capacity;

    h_list();
    h_list(std::initializer_list<T> list);
    h_list(const h_list &list);
    h_list(h_list &&list);
    ~h_list();

    h_list &operator=(const h_list &list);
    h_list &operator=(const std::initializer_list<T> &list);
    h_list &operator=(h_list &&list);

    operator h_uref<T>();
    T operator[](h_i64 i);

    void append(T item);
    void append(h_list<T> list);

    h_i64 find(T item);
    h_i64 find_reverse(T item);
    h_list<h_i64> find_all(T item);

    void remove(T item);
    void remove_reverse(T item);
    void remove_all(T item);

    void reserve(h_i64 capacity);

    h_uref<T> data();
    h_i64 size();
    h_i64 capacity();
protected:
    void malloc_data();
    void reserve_internal(h_i64 capacity);
};

class h_str : public h_list<h_char>  {
public:
    using base = h_list<h_char>;
    using base::base;

    template <h_i64 S>
    h_str(const char (&str)[S]);

    h_i64 find(h_str str);
    h_i64 find_reverse(h_str str);
    h_list<h_i64> find_all(h_str str);

    void replace(h_str str1, h_str str2);
    void replace_reverse(h_str str1, h_str str2);
    void replace_all(h_str str1, h_str str2);

    void to_upper();
    void to_lower();
};

class h_file {
    FILE *file;
public:
    h_file(const char *filename, const char *mode);
    h_file(h_uref<h_char> filename, const char *mode);
    h_file(const char *filename, h_uref<h_char> mode);
    h_file(h_uref<h_char> filename, h_uref<h_char> mode);

    ~h_file();

    h_i32 seek_set(h_i32 offset);
    h_i32 seek_cur(h_i32 offset);
    h_i32 seek_end(h_i32 offset);
    h_i32 tell();

    template <typename T>
    h_u64 read(h_uref<T> arr);

    template <typename ...A>
    void scan(h_uref<h_char> fmt, A... args);

    template <typename T>
    h_u64 write(h_uref<T> arr);

    void print(h_char c);
    void print(h_uref<h_char> str);
    template <typename ...A>
    h_i32 print(h_uref<h_char> fmt, A... args);

    void println(h_char c);
    void println(h_uref<h_char> str);
    template <typename ...A>
    h_i32 println(h_uref<h_char> fmt, A... args);

    bool flush();
    bool error();
    bool eof();
};

class h_random {
    h_i32 seed;
    h_random(h_i32 seed);
    h_random();
    h_i32 random();
};

inline h_i32 print(h_char c) {
    return putchar(c) == EOF ? EOF : 1;
}

inline h_i32 print(const char *str) {
    return fputs((char *)str, stdout);
}

template <typename ...A>
inline h_i32 print(const char *fmt, A... args) {
    h_i32 result = printf(fmt, args...);
    return result < 0 ? EOF : result;
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
    return result_c == EOF || result_ln == EOF ? EOF : 2;
}

inline h_i32 println(const char *str) {
    h_i32 result = puts(str);
    return result == EOF ? EOF : result;
}

template <typename ...A>
inline h_i32 println(const char *fmt, A... args) {
    h_i32 result_f = printf(fmt, args...);
    h_i32 result_l = putchar('\n');

    h_i32 result = EOF;
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
    println((char *)fmt.ptr(), args...);
}

void read(h_uref<h_char> ref);
template <typename ...A>
void scan(h_uref<h_char> fmt, A... args);

#include "hstdlib.ipp"