#pragma once
#include "Token.hpp"
#include "Error.hpp"
#include "Reader.hpp"
#include "Context.hpp"
#include <vector>
#include <memory>

class Tokenizer {
public:
    Tokenizer(const std::vector<std::string> &allowedSymbols);
    bool tokenize(const std::string &input, const std::string &filename);
    const std::vector<Token> &getTokens() const;
    const std::vector<Error> &getErrors() const;
    bool hasErrors() const;

private:
    std::vector<Token> tokens;
    std::vector<Error> errors;
    std::vector<std::unique_ptr<Reader>> readers;

    void initReaders(const std::vector<std::string> &allowedSymbols);
};
