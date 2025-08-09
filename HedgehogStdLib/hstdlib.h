#pragma once
#include <cstdint>
#include <cstring>

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

class h_char {
public:
    unsigned char c;

    constexpr h_char(unsigned char c): c(c) {};
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

    h_sref(T (&ref)[S]) {
        _data = &ref;
    }
    T operator [](h_u64 idx) {
        return _data[idx];
    }
    constexpr h_u64 size() {
        return S;
    }
};

template <typename T>
class h_uref {
public:
    T *_data;
    h_u64 _size;

    template <h_u64 S>
    h_uref(T (&ref)[S]) {
        _data = &ref;
        _size = S;
    }
    h_uref(T *data, h_u64 size) {
        _data = data;
        _size = size;
    }
    T operator [](h_u64 idx) {
        return _data[idx];
    }
    h_u64 size() {
        return _size;
    }
};

template <typename T>
class h_list {
public:
    T *_data;
    h_u64 _size;
    h_u64 _capacity;

    h_list() {}

    template <h_u64 S>
    h_list(T (&list)[S]);

    h_list(h_list &list);
    h_list(h_list &&list);

    ~h_list() {}
    
    operator h_uref<T>() {
        return { _data, _size };
    }
    T operator[](h_u64 idx);

    void append(T item);
    void append(h_list<T> list);

    h_u64 find(T item);
    h_u64 find_reverse(T item);
    h_list<h_u64> find_all(T item);

    void remove(T item);
    void remove_reverse(T item);
    void remove_all(T item);

    h_uref<T> data();
    h_u64 size();
    h_u64 capacity();
};

template <typename T, h_u64 S>
class h_array {
public:
    T data[S];

    operator h_sref<T, S>();
    operator h_uref<T>();

    h_u64 find(T item);
    h_u64 find_reverse(T item);
    h_list<h_u64> find_all(T item);

    h_u64 size();
};

class h_str : public h_list<h_char>  {
public:
    
    using base = h_list<h_char>;
    using base::base;

    template <h_u64 S>
    h_str(const char (&str)[S]) {
        _data = (h_char *)str;
        _size = S;
    }

    h_u64 find(h_str str);
    h_u64 find_reverse(h_str str);
    h_list<h_u64> find_all(h_str str);

    void replace(h_str str1, h_str str2);
    void replace_reverse(h_str str1, h_str str2);
    void replace_all(h_str str1, h_str str2);
    
    void to_upper();
    void to_lower();
};

class h_file {
    void *file;
public:
    h_file(const h_char *filename);
    ~h_file();

    h_bool seek_start(h_i64 offset);
    h_bool seek_cur(h_i64 offset);
    h_bool seek_end(h_i64 offset);

    h_i64 tell();

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

h_i32 print(h_char c);
h_i32 print(const char *str);
template <typename ...A>
h_i32 print(const char *fmt, A... args);
h_i32 print(h_uref<h_char> str);
template <typename ...A>
h_i32 print(h_uref<h_char> fmt, A... args);

h_i32 println(h_char c);
h_i32 println(const char *str);
template <typename ...A>
h_i32 println(const char *fmt, A... args);
h_i32 println(h_uref<h_char> str);
template <typename ...A>
h_i32 println(h_uref<h_char> fmt, A... args);

void read(h_uref<h_char> ref);
template <typename ...A>
void scan(h_uref<h_char> fmt, A... args);

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
}