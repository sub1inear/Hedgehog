#pragma once
#include <cstdint>
#include <cstdio>
#include <cctype>
#include <string_view>
#include <unordered_map>
#include "token.h"

namespace hedgehog {
class Lexer {
public:
    FILE *file;

    Token token;
    bool peeked;

    struct OperatorData {
        const char str[3];
        Token::Type type;
        int32_t prec;
    };

    struct KeywordData {
        const char *str;
        Token::Type type;
    };

    static constexpr OperatorData operator_data[] = {
        { { '+' , '+' , '\0' }, Token::Type::INC,       13 },
        { { '-' , '-' , '\0' }, Token::Type::DEC,       13 },
                                               
        { { '*' , '\0', '\0' }, (Token::Type)'*',       12 },
        { { '/' , '\0', '\0' }, (Token::Type)'/',       12 },
        { { '%' , '\0', '\0' }, (Token::Type)'%',       12 },
                                               
        { { '+' , '\0', '\0' }, (Token::Type)'+',       11 },
        { { '-' , '\0', '\0' }, (Token::Type)'-',       11 },

        { { '<' , '<' , '\0' }, Token::Type::LSHIFT,    10 },
        { { '>' , '>' , '\0' }, Token::Type::RSHIFT,    10 },

        { { '<' , '\0', '\0' }, (Token::Type)'<',       9  },
        { { '>' , '\0', '\0' }, (Token::Type)'>',       9  },
        { { '<' , '=' , '\0' }, Token::Type::LT_EQ,     9  },
        { { '>' , '=' , '\0' }, Token::Type::GT_EQ,     9  },

        { { '=' , '=' , '\0' }, Token::Type::EQ,        8  },
        { { '!' , '=' , '\0' }, Token::Type::NOT_EQ,    8  },

        { { '&' , '\0', '\0' }, (Token::Type)'&',       7  },
        { { '^' , '\0', '\0' }, (Token::Type)'^',       6  },
        { { '|' , '\0', '\0' }, (Token::Type)'|',       5  },

        { { '+' , '=' , '\0' }, Token::Type::PLUS_EQ,   1  },
        { { '-' , '=' , '\0' }, Token::Type::SUB_EQ,    1  },
        { { '*' , '=' , '\0' }, Token::Type::MUL_EQ,    1  },
        { { '/' , '=' , '\0' }, Token::Type::DIV_EQ,    1  },
        { { '%' , '=' , '\0' }, Token::Type::MOD_EQ,    1  },
        { { '&' , '=' , '\0' }, Token::Type::AND_EQ,    1  },
        { { '|' , '=' , '\0' }, Token::Type::OR_EQ,     1  },
        { { '^' , '=' , '\0' }, Token::Type::XOR_EQ,    1  },
        { { '<' , '<' , '=' },  Token::Type::LSHIFT_EQ,  1  },
        { { '>' , '>' , '=' },  Token::Type::RSHIFT_EQ,  1  },
    };

    static constexpr KeywordData keyword_data[] = {
        { "if",       Token::Type::IF       },
        { "while",    Token::Type::WHILE    },
        { "for",      Token::Type::FOR      },
      
        { "break",    Token::Type::BREAK    },
        { "continue", Token::Type::CONTINUE },
      
        { "and",      Token::Type::AND      },
        { "or",       Token::Type::OR       },
        { "not",      Token::Type::NOT      },
      
        { "true",     Token::Type::TRUE     },
        { "false",    Token::Type::FALSE    },
      
        { "in",       Token::Type::IN       },
        { "range",    Token::Type::RANGE    },
      
        { "enum",     Token::Type::ENUM     },
      
        { "def",      Token::Type::DEF      },
    
        // ------------------------------- //

        { "i8",       Token::Type::I8       },
        { "u8",       Token::Type::U8       },

        { "i16",      Token::Type::I16      },
        { "u16",      Token::Type::U16      },
                                        
        { "i32",      Token::Type::I32      },
        { "u32",      Token::Type::U32      },
                                        
        { "i64",      Token::Type::I64      },
        { "u64",      Token::Type::U64      },
                                        
        { "int",      Token::Type::INT      },
                                        
        { "f32",      Token::Type::F32      },
        { "f64",      Token::Type::F64      },
      
        { "float",    Token::Type::FLOAT    },
      
        { "bool",     Token::Type::BOOL     },
        { "char",     Token::Type::CHAR     },
      
        { "isize",    Token::Type::ISIZE    },
        { "usize",    Token::Type::USIZE    },
      
        { "time_t",   Token::Type::TIME_T   },
    };

    static constexpr size_t keyword_type_start = 13;

    Lexer(const char *filename);
    ~Lexer();

    void next();
    void peek();
    
    void match(Token::Type expected);
    void match(Token::Type expected_1, Token::Type expected_2);
    void match_type();
private:
    void lex();
    void lex_core();
};

}