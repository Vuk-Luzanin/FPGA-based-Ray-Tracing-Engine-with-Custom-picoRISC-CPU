#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "linker.hpp"

class Compiler {
public:
    Compiler(const std::string &ordFileName);
    bool compile();
private:
    bool processCommands();
    void encodeMicroinstructions();
    bool generateOutput();

    void writeCAMFile();
    void writeUputstvoFile();
    void writeMIFFile();
    void writeOperatingFile();
    void writeControlFile();
    void writeKMADR();
    void writeKMOPR();
    void writeKMBR();

    std::string ordFileName;
    std::string camFileName;
    std::string mifFileName;
    std::string uputstvoFileName;
    std::string operatingFileName;
    std::string controlFileName;
    std::string kmadrFileName;
    std::string kmoprFileName;
    std::string kmbrFileName;
    Linker linker;
    std::vector<Command> commands;
    std::vector<std::vector<bool>> microinstructions;

    std::unordered_map<std::string, size_t> operationalSignals;
    std::unordered_map<std::string, size_t> controlSignals;
    size_t numberOfInstructions;

    static std::string removeOrdExtension(const std::string &filename);
};
