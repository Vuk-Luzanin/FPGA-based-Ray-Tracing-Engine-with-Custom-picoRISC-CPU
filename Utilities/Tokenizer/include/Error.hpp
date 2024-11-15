#pragma once
#include <string>
#include <limits>

class Error {
public:
    static constexpr size_t NO_LINE = std::numeric_limits<size_t>::max();

    Error(const std::string &message, const std::string &filename = "", size_t line = NO_LINE);

    std::string getMessage() const;
    size_t getLine() const;
    std::string getFilename() const;

private:
    std::string message;
    size_t line;
    std::string filename;
};
