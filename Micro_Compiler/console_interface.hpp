#pragma once
#include <string>

class ConsoleInterface {
public:
    void start();
    std::string getUserCommand();
    void printCommand(const std::string& command);
};
