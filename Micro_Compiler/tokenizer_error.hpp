#pragma once
#include <string>

class TokenizerError {
public:
    TokenizerError(const std::string& message, int line, const std::string& filename = "");

    std::string getMessage() const;

private:
    std::string message;
    int line;
    std::string filename;
};
