#include "lexer.h"
#include "error.h"

namespace hedgehog {

Lexer::Lexer(const char *filename) : peeked(false) {
    file = fopen(filename, "r");
    if (file == nullptr) {
        fatal_error("error opening file: %s", strerror(errno));
    }
}

Lexer::~Lexer() {
    fclose(file);
}

void Lexer::next() {
    if (peeked) {
        peeked = false;
        return;
    }
    peeked = false;
    lex();
}

void Lexer::peek() {
    if (peeked) {
        return;
    }
    peeked = true;
    lex();
}

void Lexer::lex() {
    lex_core();
    if (token.type == Token::Type::NEWLINE) {
        long pos;
        do {
            pos = ftell(file);
            lex_core();
        } while (token.type == Token::Type::NEWLINE);
        fseek(file, pos, SEEK_SET);
        token.type = Token::Type::NEWLINE;
        token.clear_non_type();
    }
}

void Lexer::lex_core() {
    token.clear_non_type();
    while (true) {
        char c = fgetc(file);
        if (c == ' ' || c == '\t') {}
        else if (c == '\n') { // newline
            token.type = Token::Type::NEWLINE;
            return;
        } else if (isalpha(c) || c == '_') { // identifier or keyword
            do {
                token.str.push_back(c);
                c = fgetc(file);
            } while (isalnum(c) || c == '_');
            ungetc(c, file);

            token.type = Token::Type::ID;
            
            for (const KeywordData &data : keyword_data) {
                if (token.str == data.str) {
                    token.type = data.type;
                    break;
                }
            }
            return;
        } else if (isdigit(c)) { // number
            do {
                token.str.push_back(c);
                c = fgetc(file);
            } while (isdigit(c));
            ungetc(c, file);

            token.type = Token::Type::INT_LITERAL;
            if (token.str.find('.') != std::string::npos) {
                token.type = Token::Type::FLOAT_LITERAL;
            }
            return;
        } else if (c == '"') {
            while (true) {
                c = fgetc(file);
                // not using switch to break out of loop
                if (c == EOF) {
                    fatal_error("unexpected EOF in string literal");
                } else if (c == '\\') {
                    token.str.push_back(c);
                    c = fgetc(file);
                } else if (c == '"') {
                    token.str.push_back(c);
                    break;
                } else if (c == '\n') {
                    token.str.push_back('\\');
                    token.str.push_back('n');
                    continue;
                }
                token.str.push_back(c);
            }
            token.type = Token::Type::STRING_LITERAL;
            return;
        } else if (c == '\'') {
            token.str.push_back(c);
            c = fgetc(file);
            token.str.push_back(c);
            if (c == '\\') {
                c = fgetc(file);
                token.str.push_back(c);
            }
            if (c != '\\') {
                fatal_error("character constant is too long");
            }
        } else {
            long pos = ftell(file); // cannot portably ungetc 2 times in a row
            char c2 = fgetc(file);
            
            if (c == '/') {
                if (c2 == '/') { // single-line comment
                    do {
                        c = fgetc(file);
                    } while (c != '\n' && c != EOF);
                    ungetc(c, file);
                    continue;
                } else if (c2 == '*') { // multi-line comment
                    do {
                        c2 = c;
                        c = fgetc(file);
                        if (c == EOF) {
                            fatal_error("unexpected EOF in multi-line comment");
                        }
                    } while (c2 != '*' && c != '/');
                    ungetc(c, file);
                    continue;
                }
            }

            char c3 = fgetc(file);

            for (const OperatorData &data : operator_data) {
                if (data.str[0] == c && 
                   (data.str[1] == c2 || data.str[1] == '\0') &&
                   (data.str[2] == c3 || data.str[2] == '\0')) {
                    if (data.str[1] == c2 && data.str[2] != c3) {
                        ungetc(c3, file);
                    } else {
                        fseek(file, pos, SEEK_SET); // cannot portably ungetc 2 times in a row
                    }

                    token.type = data.type;
                    token.prec = data.prec;
                    return;
                }
            }
            token.type = (Token::Type)c;
            fseek(file, pos, SEEK_SET); // cannot portably ungetc 2 times in a row
            return;
        }
    }
}

void Lexer::match(Token::Type expected) {
    if (expected != token.type) {
        fatal_error("unexpected token");
    }
    next();
}

void Lexer::match(Token::Type expected_1, Token::Type expected_2) {
    if (expected_1 != token.type || expected_2 != token.type) {
        fatal_error("unexpected token");
    }
    next();
}

void Lexer::match_type() {
    // using for loop to start at specific place in keyword_data
    for (size_t i = keyword_type_start;
                i < std::size(keyword_data);
                i++) {
        const KeywordData &data = keyword_data[i];
        if (data.str == token.str) {
            next();
            return;
        }
    }
         
    fatal_error("unexpected token, expected type");
}

};