#include "token.h"

namespace hedgehog {

const char *const Token::type_to_string[] = {
    "none",
    "id",

    // literals
    "int literal",
    "float literal",

    "char literal",
    "string literal",

    // keywords
    "if",
    "while",
    "for",

    "break",
    "continue",

    "and",
    "or",
    "not",

    "true",
    "false",

    "in",
    "range",

    "enum",

    "def",
    
    // types
    "i8",
    "u8",

    "i16",
    "u16",

    "i32",
    "u32",

    "i64",
    "u64",

    "int",

    "f32",
    "f64",

    "float",

    "bool",

    "char",

    "isize",
    "usize",

    "time_t",

    // composite operators
    "<<",
    ">>",

    "==",
    "!=",
    "<=",
    ">=",

    "+=",
    "-=",
    "*=",
    "/=",
    "%=",

    "&=",
    "|=",
    "^=",

    "<<=",
    ">>=",

    "++",
    "--",

    "newline",

    // end (not used as token)
    "end",
};

Token::Token() : type(Type::NONE), prec(prec_none) {}
Token::~Token() {}

void Token::clear_non_type() {
    str.clear();
    prec = prec_none;
}

void Token::print() {
    fputs("{ type = ", stdout);
    if (type < type_start) {
        putchar(type);
    } else {
        fputs(type_to_string[type - type_start], stdout);
    }
    fputs(", value = ", stdout);
    switch (type) {
    case Type::INT_LITERAL:
    case Type::FLOAT_LITERAL:
    case Type::CHAR_LITERAL:
    case Type::STRING_LITERAL:
        fputs(str.c_str(), stdout); 
        break;
    default:
        fputs("null", stdout);
        break;
    }
    fputs(" }", stdout);
}

}