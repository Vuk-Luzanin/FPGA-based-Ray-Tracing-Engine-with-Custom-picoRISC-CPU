#include "tokenizer_error.hpp"

TokenizerError::TokenizerError(const std::string& message, int line, const std::string& filename)
    : message(message), line(line), filename(filename) {}

std::string TokenizerError::getMessage() const {
    std::string result = "Error on line " + std::to_string(line);
    if (!filename.empty()) {
        result += " in file " + filename;
    }
    result += ": " + message;
    return result;
}
