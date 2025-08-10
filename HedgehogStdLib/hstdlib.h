#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cctype>
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

    constexpr h_char(unsigned char c): c(c) {};
    operator unsigned char() {
        return c;
    }
    h_bool is_alpha() {
        return isalpha(c);
    }
    h_bool is_digit() {
        return isdigit(c);
    }
    h_bool is_alnum() {
        return isalnum(c);
    }
    h_bool is_cntrl() {
        return iscntrl(c);
    }
    h_bool is_graph() {
        return isgraph(c);
    }
    h_bool is_upper() {
        return isupper(c);
    }
    h_bool is_lower() {
        return islower(c);
    }
    h_bool is_print() {
        return isprint(c);
    }
    h_bool is_punct() {
        return ispunct(c);
    }
    h_bool is_space() {
        return isspace(c);
    }
    h_bool is_xdigit() {
        return isxdigit(c);
    }

    void to_upper() {
        c = toupper(c);
    }
    void to_lower() {
        c = tolower(c);
    }
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
    T operator [](h_i64 i) {
        return _data[i];
    }
    constexpr h_i64 size() {
        return S;
    }
};

template <typename T>
class h_uref {
public:
    T *_data;
    h_i64 _size;

    template <h_i64 S>
    h_uref(T (&data)[S]) {
        _data = &data;
        _size = S;
    }
    h_uref(T *data, h_i64 size) {
        _data = data;
        _size = size;
    }
    T operator [](h_i64 i) {
        return _data[i];
    }
    h_i64 size() {
        return _size;
    }
};

template <typename T>
class h_list {
protected:
    void malloc_data() {
        _data = (T *)malloc(_capacity * sizeof(T));
        if (_data == nullptr) {
            H_RUNTIME_ERROR("AllocError", "Memory allocation failed");
        }
    }
    void reserve_internal(h_i64 capacity) {
        _capacity = capacity;
        _data = (T *)realloc(_data, _capacity);
        if (_data == nullptr) {
            H_RUNTIME_ERROR("AllocError", "Memory allocation failed");
        }
    }
public:
    T *_data;
    h_i64 _size;
    h_i64 _capacity;

    void reserve(h_i64 capacity) {
        if (capacity <= _capacity) {
            return;
        }
        reserve_internal(capacity);
    }

    h_list() {
        _size = _capacity = 8;
        malloc_data();
    }

    h_list(std::initializer_list<T> list) {
        _size = _capacity = list.size();

        malloc_data();
        for (h_i64 i = 0; i < _size; i++) {
            new (&_data[i]) T(list.begin()[i]);
        }
    }

    h_list(const h_list &list) {
        _size = list._size;
        _capacity = list._capacity;
         
        malloc_data();
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
    
    h_list &operator=(const h_list &list) {
        _size = list._size;
        reserve(list._capacity);
        memcpy(_data, list._data, _capacity);
        return *this;
    }

    h_list &operator=(const std::initializer_list<T> &list) {
        _size = _capacity = list.size();
        
        free(_data);
        malloc_data();
        return *this;
    }

    h_list &operator=(h_list &&list) {
        _size = list._size;
        _capacity = list._capacity;
        _data = list._data;
        return *this;
    }
        
    operator h_uref<T>() {
        return { _data, _size };
    }

    T operator[](h_i64 i) {
        return _data[i];
    }

    void append(T item) {
        if (_size >= _capacity) {
            reserve_internal(_capacity * 2);
        }
        _data[_size++] = item;
    }
    void append(h_list<T> list) {
        T *end = _data + _size;

        _size += list._size;
        if (_size > _capacity) {
            reserve_internal((_capacity + list._size) * 2);
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
                memmove(&_data[i + 1], &_data[i], _size - 1);
                break;
            }
        }
    }

    void remove_reverse(T item) {
        for (h_i64 i = _size - 1; i >= 0; i--) {
            if (_data[i] == item) {
                _size--;
                memmove(&_data[i + 1], &_data[i], _size - 1);
                break;
            }
        }
    }

    void remove_all(T item) {
        for (h_i64 i = 0; i < _size; i++) {
            if (_data[i] == item) {
                _size--;
                memmove(&_data[i + 1], &_data[i], _size - 1);
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
        _size = _capacity = S;
        _data = (h_char *)malloc(_capacity * sizeof(h_char));
        if (_data == nullptr) {
            H_RUNTIME_ERROR("AllocError", "Memory allocation failed");
        }
        memcpy(_data, &str, _capacity);
    }

    h_i64 find(h_str str) {
        for (h_i64 i = 0; i <= _size - str._size; i++) {
            h_i64 j;
            h_i64 k;
            h_i64 len = str._size - 1;
            for (j = 0, k = i; 
                 j < len && _data[k] == str[j];
                 j++, k++) {}
            if (j == len) {
                return i;
            }
        }
        return -1;
    }

    h_i64 find_reverse(h_str str) {
        for (h_i64 i = _size - str._size; i >= 0; i--) {
            h_i64 j;
            h_i64 k;
            h_i64 len = str._size - 1;
            for (j = 0, k = i; 
                 j < len && _data[k] == str[j];
                 j++, k++) {}
            if (j == len) {
                return i;
            }
        }
        return -1;
    }

    h_list<h_i64> find_all(h_str str) {
        h_list<h_i64> result;
        for (h_i64 i = 0; i <= _size - str._size; i++) {
            h_i64 j;
            h_i64 k;
            h_i64 len = str._size - 1;
            for (j = 0, k = i; 
                 j < len && _data[k] == str[j];
                 j++, k++) {}
            if (j == len) {
                result.append(i);
            }
        }
        return result;
    }

    void replace(h_str str1, h_str str2) {
        h_i64 str1_len = str1._size - 1;
        h_i64 str2_len = str2._size - 1;
        for (h_i64 i = 0; i <= _size - str1._size; i++) {
            h_i64 j;
            h_i64 k;
            for (j = 0, k = i; 
                 j < str1_len && _data[k] == str1[j];
                 j++, k++) {}
            if (j == str1_len) {
                if (str2_len > str1_len) {
                    if (_capacity < _size + (str2._size - str1._size)) {
                        reserve_internal((_capacity + (str2._size - str1._size)) * 2);
                    }
                    memmove(&_data[i + str2_len], &_data[i + str1_len], _size - str2_len);
                    _size += str2_len - str1_len;
                } else if (str1_len > str2_len) {
                    memmove(&_data[i + str2_len], &_data[i + str1_len], _size - str1_len);  
                    _size -= str1_len - str2_len;
                }
                memcpy(&_data[i], str2._data, str2_len);
                break;
            }
        }
    }

    void replace_reverse(h_str str1, h_str str2) {
        h_i64 str1_len = str1._size - 1;
        h_i64 str2_len = str2._size - 1;
       for (h_i64 i = _size - str1._size; i >= 0; i--) {
            h_i64 j;
            h_i64 k;
            for (j = 0, k = i; 
                 j < str1_len && _data[k] == str1[j];
                 j++, k++) {}
            if (j == str1_len) {
                if (str2_len > str1_len) {
                    if (_capacity < _size + (str2._size - str1._size)) {
                        reserve_internal((_capacity + (str2._size - str1._size)) * 2);
                    }
                    memmove(&_data[i + str2_len], &_data[i + str1_len], _size - str2_len);     
                    _size += str2_len - str1_len;
                } else if (str1_len > str2_len) {
                    memmove(&_data[i + str2_len], &_data[i + str1_len], _size - str1_len);
                    _size -= str1_len - str2_len;
                }
                memcpy(&_data[i], str2._data, str2_len);
                break;
            }
        }
    }

    void replace_all(h_str str1, h_str str2) {
        h_i64 str1_len = str1._size - 1;
        h_i64 str2_len = str2._size - 1;
        for (h_i64 i = 0; i <= _size - str1._size; i++) {
            h_i64 j;
            h_i64 k;
            for (j = 0, k = i; 
                 j < str1_len && _data[k] == str1[j];
                 j++, k++) {}
            if (j == str1_len) {
                if (str2_len > str1_len) {
                    if (_capacity < _size + (str2._size - str1._size)) {
                        reserve_internal((_capacity + (str2._size - str1._size)) * 2);
                    }
                    memmove(&_data[i + str2_len], &_data[i + str1_len], _size - str2_len);
                    _size += str2_len - str1_len;
                } else if (str1_len > str2_len) {
                    memmove(&_data[i + str2_len], &_data[i + str1_len], _size - str1_len);
                    _size -= str1_len - str2_len;
                }
                memcpy(&_data[i], str2._data, str2_len);
            }
        }
    }
    
    void to_upper() {
        for (h_i64 i = 0; i < _size; i++) {
            _data[i].to_upper();
        }
    }

    void to_lower() {
        for (h_i64 i = 0; i < _size; i++) {
            _data[i].to_lower();
        }
    }
};

class h_file {
    FILE *file;
public:
    h_file(const char *filename, const char *mode) {
        file = fopen(filename, mode);
        if (file == nullptr) {
            H_RUNTIME_ERROR_F("FileError", "%s", strerror(errno));
        }
    }

    h_file(h_uref<h_char> filename, const char *mode) : h_file((char *)filename._data, mode) {}
    
    h_file(const char *filename, h_uref<h_char> mode) : h_file(filename, (char *)mode._data) {}

    h_file(h_uref<h_char> filename, h_uref<h_char> mode) : h_file((char *)filename._data, (char *)mode._data) {}

    ~h_file() {
        fclose(file);
    }

    h_i32 seek_set(h_i32 offset) {
        return fseek(file, offset, SEEK_SET) == 0 ? false : true;
    }
    
    h_i32 seek_cur(h_i32 offset) {
        return fseek(file, offset, SEEK_CUR) == 0 ? false : true;
    }

    h_i32 seek_end(h_i32 offset) {
        return fseek(file, offset, SEEK_END) == 0 ? false : true;
    }

    h_i32 tell() {
        return ftell(file);
    }

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

    bool flush() {
        return fflush(file) == 0 ? false : true;
    }

    bool error() {
        return ferror(file);
    }

    bool eof() {
        return feof(file);
    }
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