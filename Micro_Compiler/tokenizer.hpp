#pragma once

#include "token.hpp"
#include "tokenizer_error.hpp"
#include <string>
#include <vector>

class Tokenizer {
public:
    Tokenizer();
    bool tokenize(const std::string& input, const std::string& filename = "");
    const std::vector<Token>& getTokens() const;
    const std::vector<TokenizerError>& getErrors() const;
    bool hasErrors() const;

private:
    std::vector<Token> tokens;
    std::vector<TokenizerError> errors;
    std::string currentFile;
    int currentLine;

    void addToken(TokenType type, const std::string& value);
    void addError(const std::string& message);
    bool isNameStart(char c);
    bool isNameChar(char c);
    bool isOperator(char c);
    bool isKeyword(const std::string& str);
};
