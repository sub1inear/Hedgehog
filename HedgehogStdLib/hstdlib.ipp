#include "hstdlib.h"

constexpr h_char::h_char(unsigned char c) : c(c) {}

h_char::operator unsigned char() {
    return c;
}

h_bool h_char::is_alpha() {
    return isalpha(c);
}

h_bool h_char::is_digit() {
    return isdigit(c);
}

h_bool h_char::is_alnum() {
    return isalnum(c);
}

h_bool h_char::is_cntrl() {
    return iscntrl(c);
}

h_bool h_char::is_graph() {
    return isgraph(c);
}

h_bool h_char::is_upper() {
    return isupper(c);
}

h_bool h_char::is_lower() {
    return islower(c);
}

h_bool h_char::is_print() {
    return isprint(c);
}

h_bool h_char::is_punct() {
    return ispunct(c);
}

h_bool h_char::is_space() {
    return isspace(c);
}

h_bool h_char::is_xdigit() {
    return isxdigit(c);
}

void h_char::to_upper() {
    c = toupper(c);
}

void h_char::to_lower() {
    c = tolower(c);
}

template <typename T, h_i64 S>
h_sref<T, S>::h_sref(T (&data)[S]) {
    _data = &data;
}

template <typename T, h_i64 S>
h_sref<T, S>::h_sref(T *data) {
    _data = data;
}

template <typename T, h_i64 S>
T h_sref<T, S>::operator [](h_i64 i) {
    return _data[i];
}

template <typename T, h_i64 S>
T *h_sref<T, S>::ptr() {
    return _data;
}

template <typename T, h_i64 S>
constexpr h_i64 h_sref<T, S>::size() {
    return S;
}

template <typename T>
template <h_i64 S>
h_uref<T>::h_uref(T (&data)[S]) {
    _data = &data;
    _size = S;
}

template <typename T>
h_uref<T>::h_uref(T *data, h_i64 size) {
    _data = data;
    _size = size;
}

template <typename T>
T h_uref<T>::operator [](h_i64 i) {
    return _data[i];
}

template <typename T>
T *h_uref<T>::ptr() {
    return _data;
}

template <typename T>
h_i64 h_uref<T>::size() {
    return _size;
}

template <typename T, h_i64 S>
template <h_i64 S2>
h_array<T, S>::h_array(const h_array<T, S2> &array) {
    memcpy(_data, array._data, h_math::min(S, S2));
}

template <typename T, h_i64 S>
template <h_i64 S2>
h_array<T, S>::h_array(h_array<T, S2> &&array) {
    memcpy(_data, array._data, h_math::min(S, S2));
}

template <typename T, h_i64 S>
template <h_i64 S2>
h_array<T, S> &h_array<T, S>::operator=(const h_array<T, S2> &array) {
    memcpy(_data, array._data, h_math::min(S, S2));
    return *this;
}

template <typename T, h_i64 S>
template <h_i64 S2>
h_array<T, S> &h_array<T, S>::operator=(h_array<T, S2> &&array) {
    memcpy(_data, array._data, h_math::min(S, S2));
    return *this;
}

template <typename T, h_i64 S>
h_array<T, S>::operator h_sref<T, S>() {
    return { _data };
}

template <typename T, h_i64 S>
h_array<T, S>::operator h_uref<T>() {
    return { _data, S };
}

template <typename T, h_i64 S>
T h_array<T, S>::operator[](h_i64 i) {
    return _data[i];
}

template <typename T, h_i64 S>
h_i64 h_array<T, S>::find(T item) {
    for (h_i64 i = 0; i < S; i++) {
        if (_data[i] == item) {
            return i;    
        }
    }
    return -1;
}

template <typename T, h_i64 S>
h_i64 h_array<T, S>::find_reverse(T item) {
    for (h_i64 i = S - 1; i >= 0; i--) {
        if (_data[i] == item) {
            return i;
        }
    }
    return -1;
}

template <typename T, h_i64 S>
h_list<h_i64> h_array<T, S>::find_all(T item) {
    h_list<h_i64> result;
    for (h_i64 i = 0; i < S; i++) {
        if (_data[i] == item) {
            result.append(i);
        }
    }
    return result;
}

template <typename T, h_i64 S>
constexpr h_i64 h_array<T, S>::size() {
    return S;
}

template <typename T>
void h_list<T>::reserve(h_i64 capacity) {
    if (capacity <= _capacity) {
        return;
    }
    reserve_internal(capacity);
}

template <typename T>
h_list<T>::h_list() {
    _size = _capacity = 8;
    malloc_data();
}

template <typename T>
h_list<T>::h_list(h_i64 size) {
    _size = _capacity = size;
    malloc_data();
}

template <typename T>
h_list<T>::h_list(std::initializer_list<T> list) {
    _size = _capacity = list.size();

    malloc_data();
    for (h_i64 i = 0; i < _size; i++) {
        new (&_data[i]) T(list.begin()[i]);
    }
}

template <typename T>
h_list<T>::h_list(const h_list &list) {
    _size = list._size;
    _capacity = list._capacity;
     
    malloc_data();
    memcpy(_data, list._data, _capacity);
}

template <typename T>
h_list<T>::h_list(h_list &&list) {
    _size = list._size;
    _capacity = list._capacity;
    _data = list._data;
}

template <typename T>
h_list<T>::~h_list() {
    for (h_i64 i = 0; i < _size; i++) {
        _data[i].~T();
    }
    free(_data);
}

template <typename T>
h_list<T> &h_list<T>::operator=(const h_list &list) {
    _size = list._size;
    reserve(list._capacity);
    memcpy(_data, list._data, _capacity);
    return *this;
}

template <typename T>
h_list<T> &h_list<T>::operator=(const std::initializer_list<T> &list) {
    _size = _capacity = list.size();
    
    free(_data);
    malloc_data();
    return *this;
}

template <typename T>
h_list<T> &h_list<T>::operator=(h_list &&list) {
    _size = list._size;
    _capacity = list._capacity;
    _data = list._data;
    return *this;
}

template <typename T>
h_list<T>::operator h_uref<T>() {
    return { _data, _size };
}

template <typename T>
T h_list<T>::operator[](h_i64 i) {
    return _data[i];
}

template <typename T>
void h_list<T>::append(T item) {
    if (_size >= _capacity) {
        reserve_internal(_capacity * 2);
    }
    _data[_size++] = item;
}

template <typename T>
void h_list<T>::append(h_list<T> list) {
    T *end = _data + _size;

    _size += list._size;
    if (_size > _capacity) {
        reserve_internal((_capacity + list._size) * 2);
    }
    
    memcpy(end, list._data, list._size);
}

template <typename T>
h_i64 h_list<T>::find(T item) {
    for (h_i64 i = 0; i < _size; i++) {
        if (_data[i] == item) {
            return i;    
        }
    }
    return -1;
}

template <typename T>
h_i64 h_list<T>::find_reverse(T item) {
    for (h_i64 i = _size - 1; i >= 0; i--) {
        if (_data[i] == item) {
            return i;
        }
    }
    return -1;
}

template <typename T>
h_list<h_i64> h_list<T>::find_all(T item) {
    h_list<h_i64> result;
    for (h_i64 i = 0; i < _size; i++) {
        if (_data[i] == item) {
            result.append(i);
        }
    }
    return result;
}

template <typename T>
void h_list<T>::remove(T item) {
    for (h_i64 i = 0; i < _size; i++) {
        if (_data[i] == item) {
            _size--;
            memmove(&_data[i + 1], &_data[i], _size - 1);
            break;
        }
    }
}

template <typename T>
void h_list<T>::remove_reverse(T item) {
    for (h_i64 i = _size - 1; i >= 0; i--) {
        if (_data[i] == item) {
            _size--;
            memmove(&_data[i + 1], &_data[i], _size - 1);
            break;
        }
    }
}

template <typename T>
void h_list<T>::remove_all(T item) {
    for (h_i64 i = 0; i < _size; i++) {
        if (_data[i] == item) {
            _size--;
            memmove(&_data[i + 1], &_data[i], _size - 1);
        }
    }
}

template <typename T>
void h_list<T>::malloc_data() {
    _data = (T *)malloc(_capacity * sizeof(T));
    if (_data == nullptr) {
        H_RUNTIME_ERROR("AllocError", "Memory allocation failed");
    }
}

template <typename T>
void h_list<T>::reserve_internal(h_i64 capacity) {
    _capacity = capacity;
    _data = (T *)realloc(_data, _capacity);
    if (_data == nullptr) {
        H_RUNTIME_ERROR("AllocError", "Memory allocation failed");
    }
}

template <typename T>
h_uref<T> h_list<T>::data() {
    return { _data, _size };
}

template <typname T>
T *h_list<T>::ptr() {
    return _data;
}

template <typename T>
h_i64 h_list<T>::size() {
    return _size;
}

template <typename T>
h_i64 h_list<T>::capacity() {
    return _capacity;
}

template <h_i64 S>
h_str::h_str(const char (&str)[S]) {
    _size = _capacity = S;
    _data = (h_char *)malloc(_capacity * sizeof(h_char));
    if (_data == nullptr) {
        H_RUNTIME_ERROR("AllocError", "Memory allocation failed");
    }
    memcpy(_data, &str, _capacity);
}

h_i64 h_str::find(h_str str) {
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

h_i64 h_str::find_reverse(h_str str) {
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

h_list<h_i64> h_str::find_all(h_str str) {
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

void h_str::replace(h_str str1, h_str str2) {
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

void h_str::replace_reverse(h_str str1, h_str str2) {
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

void h_str::replace_all(h_str str1, h_str str2) {
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

void h_str::to_upper() {
    for (h_i64 i = 0; i < _size; i++) {
        _data[i].to_upper();
    }
}

void h_str::to_lower() {
    for (h_i64 i = 0; i < _size; i++) {
        _data[i].to_lower();
    }
}

h_file::h_file(const char *filename, const char *mode) {
    file = fopen(filename, mode);
    if (file == nullptr) {
        H_RUNTIME_ERROR_F("FileError", "%s", strerror(errno));
    }
}

h_file::h_file(h_uref<h_char> filename, const char *mode)
    : h_file((char *)filename.ptr(), mode) {}

h_file::h_file(const char *filename, h_uref<h_char> mode)
    : h_file(filename, (char *)mode.ptr()) {}

h_file::h_file(h_uref<h_char> filename, h_uref<h_char> mode)
    : h_file((char *)filename.ptr(), (char *)mode.ptr()) {}

h_file::~h_file() {
    fclose(file);
}

h_i32 h_file::seek_set(h_i32 offset) {
    return fseek(file, offset, SEEK_SET) == 0 ? false : true;
}

h_i32 h_file::seek_cur(h_i32 offset) {
    return fseek(file, offset, SEEK_CUR) == 0 ? false : true;
}

h_i32 h_file::seek_end(h_i32 offset) {
    return fseek(file, offset, SEEK_END) == 0 ? false : true;
}

h_i32 h_file::tell() {
    return ftell(file);
}

bool h_file::flush() {
    return fflush(file) == 0 ? false : true;
}

bool h_file::error() {
    return ferror(file);
}

bool h_file::eof() {
    return feof(file);
}

h_random::h_random() {
    std::random_device rd;
    seed = rd();
}

constexpr h_random::h_random(h_u32 seed) : seed(seed) { }

h_u32 h_random::random() {
    return seed = seed * 69069 + 1;
}

h_u32 h_random::random(h_u32 max) {
    return random() % max;
}

h_u32 h_random::random(h_u32 min, h_u32 max) {
    return random() % (max - min) + min;
}

h_i32 h_random::random(h_i32 min, h_i32 max) {
    return random() % (max - min) + min;
}