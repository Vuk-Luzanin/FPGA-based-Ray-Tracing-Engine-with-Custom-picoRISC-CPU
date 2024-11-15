#include "include/Token.hpp"

Token::Token(Type type, const std::string &value, size_t line)
    : type(type), value(value), line(line) {}

bool Token::matchToken(Type expectedType, const std::string &expectedValue) const {
    return type == expectedType && (expectedValue.empty() || value == expectedValue);
}

Type Token::getType() const {
    return type;
}

std::string Token::getValue() const {
    return value;
}

size_t Token::getLine() const {
    return line;
}