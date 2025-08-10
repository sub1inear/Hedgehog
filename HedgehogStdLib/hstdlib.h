#pragma once
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <initializer_list>

#define H_TO_STR_HELPER(x) #x
#define H_TO_STR(x) H_TO_STR_HELPER(x)
#define H_RUNTIME_ERROR(str) do { puts("Error: " str "\nFile: " __FILE__ "\nLine: " H_TO_STR(__LINE__) "\n"); exit(1); } while (0)

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

    h_sref(T (&data)[S]) {
        _data = &data;
    }
    h_sref(T *data) {
        _data = data;
    }
    T operator [](h_u64 i) {
        return _data[i];
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
    h_uref(T (&data)[S]) {
        _data = &data;
        _size = S;
    }
    h_uref(T *data, h_u64 size) {
        _data = data;
        _size = size;
    }
    T operator [](h_u64 i) {
        return _data[i];
    }
    h_u64 size() {
        return _size;
    }
};

template <typename T>
class h_list {
public:
    T *_data;
    h_i64 _size;
    h_i64 _capacity;

    h_list() {
        _size = _capacity = 8;
        
        T *m = (T *)malloc(_capacity * sizeof(T));
        if (m == nullptr) {
            H_RUNTIME_ERROR("Memory allocation failed");
        }
    }

    h_list(std::initializer_list<T> list) {
        _size = _capacity = list.size();

        _data = (T *)malloc(_capacity * sizeof(T));
        if (_data == nullptr) {
            H_RUNTIME_ERROR("Memory allocation failed");
        }

        for (h_i64 i = 0; i < _size; i++) {
            new (&_data[i]) T(list.begin()[i]);
        }
    }

    h_list(h_list &list) {
        _size = list._size;
        _capacity = list._capacity;
        
        _data = (T *)malloc(_capacity * sizeof(T));
        if (_data == nullptr) {
            H_RUNTIME_ERROR("Memory allocation failed");
        }
        memcpy(_data, list._data, _capacity);
    }

    h_list(h_list &&list) {
        _size = list._size;
        _capacity = list._capacity;
        _data = list._data;
    }

    ~h_list() {
        for (h_i64 i = 0; i < _size; i++) {
            _data[i].~T();
        }
        free(_data);
    }
    
    operator h_uref<T>() {
        return { _data, _size };
    }
    T operator[](h_i64 i) {
        return _data[i];
    }

    void append(T item) {
        if (_size >= _capacity) {
            _capacity *= 2;
            _data = realloc(_data, _capacity);
            if (_data == nullptr) {
                H_RUNTIME_ERROR("Memory allocation failed");
            }
        }
        _data[_size++] = item;
    }
    void append(h_list<T> list) {
        T *end = _data + _size;

        _size += list._size;
        if (_size > _capacity) {
            _capacity += list._capacity;
            _capacity *= 2;
            _data = realloc(_data, _capacity);
            if (_data == nullptr) {
                H_RUNTIME_ERROR("Memory allocation failed");
            }
        }
        
        memcpy(end, list._data, list._size);
    }

    h_i64 find(T item) {
        for (h_i64 i = 0; i < _size; i++) {
            if (_data[i] == item) {
                return i;    
            }
        }
        return -1;
    }
    h_i64 find_reverse(T item) {
        for (h_i64 i = _size - 1; i >= 0; i--) {
            if (_data[i] == item) {
                return i;
            }
        }
        return -1;
    }
    h_list<h_i64> find_all(T item) {
        h_list<h_i64> result;
        for (h_i64 i = 0; i < _size; i++) {
            if (_data[i] == item) {
                result.append(i);
            }
        }
        return result;
    }

    void remove(T item) {
        for (h_i64 i = 0; i < _size; i++) {
            if (_data[i] == item) {
                _size--;
                memcpy(&_data[i + 1], &_data[i], _size - 1);
                break;
            }
        }
    }

    void remove_reverse(T item) {
        for (h_i64 i = _size - 1; i >= 0; i--) {
            if (_data[i] == item) {
                _size--;
                memcpy(&_data[i + 1], &_data[i], _size - 1);
                break;
            }
        }
    }

    void remove_all(T item) {
        for (h_i64 i = 0; i < _size; i++) {
            if (_data[i] == item) {
                _size--;
                memcpy(&_data[i + 1], &_data[i], _size - 1);
            }
        }
    }

    h_uref<T> data() {
        return { _data, _size };
    }

    h_i64 size() {
        return _size;
    }
    
    h_i64 capacity() {
        return _capacity;
    }
};

template <typename T, h_i64 S>
class h_array {
public:
    T _data[S];

    operator h_sref<T, S>() {
        return { _data }; 
    }
    operator h_uref<T>() {
        return { _data, S };
    }

    T operator[](h_i64 i) {
        return _data[i];
    }

    h_i64 find(T item) {
        for (h_i64 i = 0; i < S; i++) {
            if (_data[i] == item) {
                return i;    
            }
        }
        return -1;
    }
    
    h_i64 find_reverse(T item) {
        for (h_i64 i = S - 1; i >= 0; i--) {
            if (_data[i] == item) {
                return i;
            }
        }
        return -1;
    }

    h_list<h_i64> find_all(T item) {
        h_list<h_i64> result;
        for (h_i64 i = 0; i < S; i++) {
            if (_data[i] == item) {
                result.append(i);
            }
        }
        return result;
    }

    constexpr h_i64 size() {
        return S;
    }
};

class h_str : public h_list<h_char>  {
public:
    
    using base = h_list<h_char>;
    using base::base;

    template <h_i64 S>
    h_str(const char (&str)[S]) {
        _data = (h_char *)str;
        _size = S;
    }

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
    return print((char *)str._data);
}

template <typename ...A>
inline h_i32 print(h_uref<h_char> fmt, A... args) {
    return print((char *)fmt._data, args...);
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
    return println((char *)str._data);
}

template <typename ...A>
inline h_i32 println(h_uref<h_char> fmt, A... args) {
    println((char *)fmt._data, args...);
}

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