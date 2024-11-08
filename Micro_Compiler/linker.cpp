#include "linker.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

Linker::Linker(const std::string& ordFilename) : ordFilename(ordFilename) {}

bool Linker::writeRayFile() {
    std::string rayFilename = ordFilename;
    rayFilename = rayFilename.substr(0, rayFilename.find_last_of('.')) + ".ray";

    std::ofstream rayFile(rayFilename);
    if (!rayFile.is_open()) {
        addError("Error: Unable to open .ray file for writing '" + rayFilename + "'.");
        return false;
    }

    for (const auto& command : commands) {
        // Write command details without label declarations
        rayFile << command.toStringWithoutLabelDeclarations() << std::endl;
    }

    rayFile.close();
    return true;
}

bool Linker::link() {
    if (!loadOrderFile()) {
        return false;
    }

    if (!resolveLabels()) {
        return false;
    }

    if (!writeRayFile()) {
        return false;
    }

    return true;
}


bool Linker::loadOrderFile() {
    ordFilePath = ordFilename;
    std::ifstream ordFile(ordFilename);
    if (!ordFile.is_open()) {
        addError("Error: Unable to open .ord file '" + ordFilename + "'.");
        return false;
    }

    std::string line;
    std::string ordDir = std::filesystem::path(ordFilename).parent_path().string();

    while (std::getline(ordFile, line)) {
        if (line.empty() || line[0] == '#') {
            continue; // Skip empty lines and comments
        }

        std::string moduleFilename = ordDir + "/" + line + ".vec";
        if (!loadModule(moduleFilename)) {
            addError("Error: Failed to load module '" + moduleFilename + "'.");
            return false;
        }
    }

    return true;
}

bool Linker::loadModule(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        addError("Error: Unable to open module file '" + filename + "'.");
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();

    Tokenizer tokenizer;
    if (!tokenizer.tokenize(fileContent, filename)) {
        for (const auto& error : tokenizer.getErrors()) {
            addError(error.getMessage());
        }
        return false;
    }

    Parser parser(filename);
    if (!parser.parse(tokenizer.getTokens())) {
        for (const auto& error : parser.getErrors()) {
            addError(error.getMessage());
        }
        return false;
    }

    // Collect commands and track labels
    size_t offset = commands.size();
    for (const auto& command : parser.getCommands()) {
        commands.push_back(command);
        if (command.label.has_value()) {
            const std::string& label = command.label.value();
            if (labelDefinitionCount.find(label) == labelDefinitionCount.end()) {
                labelDefinitionCount[label] = 0;
            }
            labelDefinitionCount[label]++;
            labelToLine[label] = commands.size() - 1;
        }
    }

    return true;
}

bool Linker::resolveLabels() {
    for (const auto& [label, count] : labelDefinitionCount) {
        if (count > 1) {
            addError("Error: Label '" + label + "' defined multiple times.");
            return false;
        }
    }

    for (auto& command : commands) {
        if (command.jumpDestination.has_value()) {
            auto it = labelToLine.find(command.jumpDestination.value());
            if (it == labelToLine.end()) {
                addError("Error: Label '" + command.jumpDestination.value() + "' does not exist.");
                return false;
            }
            command.jumpDestination = std::to_string(it->second);
        }
        for (auto& caseAction : command.caseActions) {
            auto it = labelToLine.find(caseAction.second);
            if (it != labelToLine.end()) {
                caseAction.second = std::to_string(it->second);
            }
            else {
                addError("Error: Label '" + caseAction.second + "' does not exist.");
                return false;
            }
        }
    }
    return true;
}

void Linker::addError(const std::string& message) {
    errors.push_back(message);
}
