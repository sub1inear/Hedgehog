#include "hstdlib.h"
#include <cstdio>
#include <cctype>

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

h_i32 print(h_char c) {
    return putchar(c) == EOF ? 0 : 1;
}

h_i32 print(const char *str) {
    h_i32 result = fputs((char *)str, stdout);
    return result == EOF ? 0 : result;
}

template <typename ...A>
h_i32 print(const char *fmt, A... args) {
    h_i32 result = printf(fmt, args...);
    return result < 0 ? 0 : result;
}

h_i32 print(h_uref<h_char> str) {
    return print((char *)str._data);
}

template <typename ...A>
h_i32 print(h_uref<h_char> fmt, A... args) {
    return print((char *)fmt._data, args...);
}

h_i32 println(h_char c) {
    h_i32 result_c = putchar(c);
    h_i32 result_ln = putchar('\n');
    return result_c == EOF || result_ln == EOF ? 0 : 2;
}

h_i32 println(const char *str) {
    h_i32 result = puts(str);
    return result == EOF ? 0 : result;
}

template <typename ...A>
h_i32 println(const char *fmt, A... args) {
    h_i32 result_f = printf(fmt, args...);
    h_i32 result_l = putchar('\n');

    h_i32 result = 0;
    if (result_f >= 0) {
        result += result_f;
    }
    if (result_l != EOF) {
        result++;
    }
    return result;
}

h_i32 println(h_uref<h_char> str) {
    return println((char *)str._data);
}

template <typename ...A>
h_i32 println(h_uref<h_char> fmt, A... args) {
    println((char *)fmt._data, args...);
}