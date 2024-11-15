#pragma once
#include <string>

class Token {
public:
    enum class Type {
        Name,
        Number,
        Symbol,
        Unknown
    };

    Token(Type type, const std::string &value, size_t line);

    bool matchToken(Type expectedType, const std::string &expectedValue = "");

    Type getType() const;
    std::string getValue() const;
    size_t getLine() const;

private:
    Type type;
    std::string value;
    size_t line;
};
