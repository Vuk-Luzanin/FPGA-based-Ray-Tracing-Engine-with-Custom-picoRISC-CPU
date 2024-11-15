#include "include/Error.hpp"

Error::Error(const std::string &message, const std::string &filename, size_t line)
    : message(message), filename(filename), line(line) {}

std::string Error::getMessage() const {
    return message;
}

size_t Error::getLine() const {
    return line;
}

std::string Error::getFilename() const {
    return filename;
}
