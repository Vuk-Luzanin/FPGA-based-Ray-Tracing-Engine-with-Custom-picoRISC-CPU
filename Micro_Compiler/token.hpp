#pragma once
#include <string>

enum class TokenType {
    Name,
    Operator,
    Keyword,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;

    Token(TokenType t, const std::string& val, int ln) : type(t), value(val), line(ln) {}
};
