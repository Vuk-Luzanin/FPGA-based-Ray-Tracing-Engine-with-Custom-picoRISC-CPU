#pragma once
#include <string>
#include <vector>
#include "Token.hpp"
#include "Error.hpp"

class Context {
public:
    Context(const std::string &content, const std::string &filename);

    const std::string &getContent() const;
    const std::string &getFilename() const;
    size_t getIndex() const;
    void incrementIndex(size_t value = 1);
    void setIndex(size_t newIndex);
    size_t getCurrentLine() const;
    void incrementLine();

    std::vector<Token> tokens;
    std::vector<Error> errors;

private:
    const std::string &content;
    std::string filename;
    size_t index;
    size_t currentLine;
};
