#include "console_interface.hpp"
#include <iostream>

void ConsoleInterface::start() {
    std::string command;
    while (true) {
        std::cout << "$ ";
        command = getUserCommand();
        if (command == "exit") {
            break;
        }
        printCommand(command);
    }
}

std::string ConsoleInterface::getUserCommand() {
    std::string command;
    std::getline(std::cin, command);
    return command;
}

void ConsoleInterface::printCommand(const std::string& command) {
    std::cout << "You entered: " << command << std::endl;
}
