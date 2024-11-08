#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include "parser.hpp"

class Linker {
public:
    Linker(const std::string& ordFilename);

    bool link();
    const std::vector<Command>& getCommands() const { return commands; }
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    bool loadOrderFile();
    bool loadModule(const std::string& filename);
    bool resolveLabels();
    void addError(const std::string& message);
    bool writeRayFile();

    std::string ordFilename;
    std::string ordFilePath;
    std::vector<Command> commands;
    std::vector<std::string> errors;
    std::unordered_map<std::string, size_t> labelToLine;  // Maps label to line number
    std::unordered_map<std::string, size_t> labelDefinitionCount;  // Maps label to definition count
};
