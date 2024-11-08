#include "tokenizer.hpp"
#include <cctype>
#include <fstream>
#include <iostream>

Tokenizer::Tokenizer() : currentLine(1) {}

bool Tokenizer::tokenize(const std::string& input, const std::string& filename) {
    tokens.clear();
    errors.clear();
    currentLine = 1;
    currentFile = filename;

    size_t i = 0;
    while (i < input.size()) {
        // Skip whitespace and track new lines
        if (std::isspace(input[i])) {
            if (input[i] == '\n') {
                currentLine++;
            }
            ++i;
            continue;
        }

        // Tokenize names
        if (isNameStart(input[i])) {
            size_t start = i;
            while (i < input.size() && isNameChar(input[i])) {
                ++i;
            }
            std::string name = input.substr(start, i - start);

            // Check if it's a keyword
            if (isKeyword(name)) {
                addToken(TokenType::Keyword, name);
            }
            else {
                addToken(TokenType::Name, name);
            }
            continue;
        }

        // Tokenize operators
        if (isOperator(input[i])) {
            addToken(TokenType::Operator, std::string(1, input[i]));
            ++i;
            continue;
        }

        // Invalid token handling
        addError("Invalid token: '" + std::string(1, input[i]) + "'");
        ++i;
    }

    return !hasErrors();
}

const std::vector<Token>& Tokenizer::getTokens() const {
    return tokens;
}

const std::vector<TokenizerError>& Tokenizer::getErrors() const {
    return errors;
}

bool Tokenizer::hasErrors() const {
    return !errors.empty();
}

void Tokenizer::addToken(TokenType type, const std::string& value) {
    tokens.emplace_back(type, value, currentLine);
}

void Tokenizer::addError(const std::string& message) {
    errors.emplace_back(message, currentLine, currentFile);
}

bool Tokenizer::isNameStart(char c) {
    return std::isalpha(c) || c == '_';
}

bool Tokenizer::isNameChar(char c) {
    return std::isalnum(c) || c == '_';
}

bool Tokenizer::isOperator(char c) {
    return c == ':' || c == '(' || c == ')' || c == ',' || c == ';' || c == '!';
}

bool Tokenizer::isKeyword(const std::string& str) {
    return str == "if" || str == "then" || str == "case" || str == "br";
}
