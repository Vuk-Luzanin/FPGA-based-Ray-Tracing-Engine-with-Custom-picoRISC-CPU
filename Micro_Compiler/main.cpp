#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
#include "tokenizer.hpp"
#include "parser.hpp"
#include "linker.hpp"
#include "compiler.hpp"

// Define the directory path as a constant
const std::string directoryPath = "C:/microinstructions/";

// Function to list files in the directory
void listFiles() {
    for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
        std::cout << entry.path().filename().string() << std::endl;
    }
}

// Function to parse a file
void parseFile(const std::string &filename) {
    std::string filePath = directoryPath + filename + ".ord";

    // Read the file content into a string
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file '" << filename << "'." << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();

    Tokenizer tokenizer;
    if (!tokenizer.tokenize(fileContent, filename)) {
        for (const auto &error : tokenizer.getErrors()) {
            std::cerr << error.getMessage() << std::endl;
        }
        return;
    }

    Parser parser(filename);
    if (!parser.parse(tokenizer.getTokens())) {
        for (const auto &error : parser.getErrors()) {
            std::cerr << error.getMessage() << std::endl;
        }
        return;
    }

    std::cout << "Parsing successful!" << std::endl;
    const auto &commands = parser.getCommands();
    for (const auto &command : commands) {
        std::cout << command << std::endl;
    }
}

// Function to link files based on .ord file
void linkFiles(const std::string &ordFilename) {
    auto ordFilePath = directoryPath + ordFilename + ".ord";
    Linker linker(ordFilePath);
    if (!linker.link()) {
        for (const auto &error : linker.getErrors()) {
            std::cerr << error << std::endl;
        }
        return;
    }

    std::cout << "Linking successful!" << std::endl;
    const auto &linkedCommands = linker.getCommands();
    for (const auto &command : linkedCommands) {
        std::cout << command << std::endl;
    }
}

// Function to compile files based on .ord file
void compileFiles(const std::string &ordFilename) {
    auto ordFilePath = directoryPath + ordFilename + ".ord";
    Compiler compiler(ordFilePath);
    if (!compiler.compile()) {
        std::cerr << "Compilation failed." << std::endl;
        return;
    }

    std::cout << "Compilation successful!" << std::endl;
}

void cleanFiles() {
    for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
        auto filePath = entry.path();
        if (filePath.extension() != ".vec" && filePath.extension() != ".ord") {
            std::filesystem::remove(filePath);
            std::cout << "deleted " << filePath << std::endl;
        }
    }
    std::cout << "Cleaned up unnecessary files.\n";
}


int main() {
    // Ensure the directory exists
    std::filesystem::create_directory(directoryPath);

    std::string command;
    while (true) {
        std::cout << "$ ";
        std::getline(std::cin, command);

        if (command == "list") {
            listFiles();
        }
        else if (command.rfind("parse ", 0) == 0) {
            std::string filename = command.substr(6);
            parseFile(filename);
        }
        else if (command.rfind("link ", 0) == 0) {
            std::string ordFilename = command.substr(5);
            linkFiles(ordFilename);
        }
        else if (command.rfind("compile ", 0) == 0) {
            std::string ordFilename = command.substr(8);
            compileFiles(ordFilename);
        }
        else if (command == "clean") {
            cleanFiles();
        }
        else if (command == "exit") {
            break;
        }
        else {
            std::cerr << "Unknown command. Use 'list' to see available files, 'parse <filename>' to parse a file, 'link <ordfile>' to link files, or 'compile <ordfile>' to compile files." << std::endl;
        }
    }

    return 0;
}
