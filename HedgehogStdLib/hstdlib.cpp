#include "hstdlib.h"
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