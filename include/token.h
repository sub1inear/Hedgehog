#pragma once
#include <cinttypes>
#include <string>
#include <climits>

namespace hedgehog {
class Token {
public:
    static const char *const type_to_string[];
    static constexpr int32_t type_start = UCHAR_MAX;
    enum Type : int32_t {
        NONE = type_start,
        ID,
        // literals
        INT_LITERAL,
        FLOAT_LITERAL,

        CHAR_LITERAL,
        STRING_LITERAL,

        // keywords
        IF,
        WHILE,
        FOR,

        BREAK,
        CONTINUE,

        AND,
        OR,
        NOT,

        TRUE,
        FALSE,

        IN,
        RANGE,

        ENUM,

        DEF,

        // types
        I8,
        U8,

        I16,
        U16,
    
        I32,
        U32,

        I64,
        U64,

        INT,

        F32,
        F64,

        FLOAT,

        BOOL,

        CHAR,

        ISIZE,
        USIZE,

        TIME_T,

        // composite operators
        LSHIFT,
        RSHIFT,

        EQ,
        NOT_EQ,
        LT_EQ,
        GT_EQ,

        PLUS_EQ,
        SUB_EQ,
        MUL_EQ,
        DIV_EQ,
        MOD_EQ,
    
        AND_EQ,
        OR_EQ,
        XOR_EQ,

        LSHIFT_EQ,
        RSHIFT_EQ,    

        INC,
        DEC,

        NEWLINE,

        // end (not used as token)
        END,
    };
    
    Type type;
    std::string str;
    
    static constexpr int32_t prec_none = -1;
    int32_t prec;


    Token();
    ~Token();

    void clear_non_type();
    void print();
};

}