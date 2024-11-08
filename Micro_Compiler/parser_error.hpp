#pragma once
#include <string>

class ParserError {
public:
    ParserError(const std::string& message, const std::string& fileName, int lineNumber)
        : message(message), fileName(fileName), lineNumber(lineNumber) {}

    std::string getMessage() const {
        return "Error in file '" + fileName + "' at line " + std::to_string(lineNumber) + ": " + message;
    }

private:
    std::string message;
    std::string fileName;
    int lineNumber;
};
