#include "compiler.hpp"
#include <fstream>
#include <iostream>
#include <bitset>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>

Compiler::Compiler(const std::string &ordFileName)
    : ordFileName(ordFileName),
    camFileName(removeOrdExtension(ordFileName) + ".cam"),
    uputstvoFileName(removeOrdExtension(ordFileName) + "_uputstvo.txt"),
    mifFileName(removeOrdExtension(ordFileName) + ".mif"),
    operatingFileName(removeOrdExtension(ordFileName) + "_TranslateOperating.v"),
    controlFileName(removeOrdExtension(ordFileName) + "_TranslateControl.v"),
    kmadrFileName(removeOrdExtension(ordFileName) + "_KMADR.v"),
    kmoprFileName(removeOrdExtension(ordFileName) + "_KMOPR.v"),
    kmbrFileName(removeOrdExtension(ordFileName) + "_KMBR.v"),
    linker(ordFileName),
    numberOfInstructions(0) {
}

std::string Compiler::removeOrdExtension(const std::string &filename) {
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".ord") {
        return filename.substr(0, filename.size() - 4);
    }
    return filename;
}

bool Compiler::compile() {
    if (!linker.link()) {
        std::cerr << "Linking failed. Errors:\n";
        for (const auto &error : linker.getErrors()) {
            std::cerr << error << std::endl;
        }
        return false;
    }

    commands = linker.getCommands();
    if (!processCommands()) {
        return false;
    }

    encodeMicroinstructions();
    return generateOutput();
}

bool Compiler::processCommands() {
    controlSignals["none"] = 0;
    controlSignals["bruncnd"] = 1;

    for (size_t i = 0; i < commands.size(); ++i) {
        const auto &command = commands[i];
        for (const auto &signal : command.signals) {
            if (operationalSignals.find(signal) == operationalSignals.end()) {
                operationalSignals[signal] = operationalSignals.size();
            }
        }

        if (command.jumpType == Command::JumpType::IF) {
            const auto &condition = command.condition.value();
            std::string controlSignal = condition.negated ? "!" + condition.name : condition.name;
            if (controlSignals.find(controlSignal) == controlSignals.end()) {
                controlSignals[controlSignal] = controlSignals.size();
            }
        }
        else if (command.jumpType == Command::JumpType::CASE) {
            std::string caseSignal = "case" + std::to_string(i);
            controlSignals[caseSignal] = controlSignals.size();
        }
    }

    numberOfInstructions = commands.size();
    return true;
}


void Compiler::encodeMicroinstructions() {
    int No = operationalSignals.size();
    int Nc = controlSignals.size();
    int branchAddressBits = std::ceil(std::log2(numberOfInstructions + 1));

    int controlBits = std::ceil(std::log2(Nc));
    int totalBits = No + controlBits + branchAddressBits;

    for (size_t i = 0; i < commands.size(); ++i) {
        const auto &command = commands[i];
        std::vector<bool> microinstruction(totalBits, false);
        for (const auto &signal : command.signals) {
            microinstruction[operationalSignals[signal]] = true;
        }
        int controlCode = 0;
        if (command.jumpType == Command::JumpType::UNCONDITIONAL) {
            controlCode = controlSignals["bruncnd"];
        }
        else if (command.jumpType == Command::JumpType::IF && command.condition.has_value()) {
            const auto &condition = command.condition.value();
            std::string controlSignal = condition.negated ? "!" + condition.name : condition.name;
            controlCode = controlSignals[controlSignal];
        }
        else if (command.jumpType == Command::JumpType::CASE) {
            controlCode = controlSignals["case" + std::to_string(i)];
        }
        auto controlBitsVector = std::bitset<32>(controlCode).to_string();
        for (int j = 0; j < controlBits; ++j) {
            microinstruction[No + j] = controlBitsVector[32 - controlBits + j] == '1';
        }
        if (command.jumpType != Command::JumpType::CASE) {
            size_t branchAddress = command.jumpDestination.has_value() ? std::stoi(command.jumpDestination.value()) : 0;
            auto branchAddressBitsVector = std::bitset<32>(branchAddress).to_string();
            for (int j = 0; j < branchAddressBits; ++j) {
                microinstruction[No + controlBits + j] = branchAddressBitsVector[32 - branchAddressBits + j] == '1';
            }
        }
        microinstructions.push_back(microinstruction);
    }
}


bool Compiler::generateOutput() {
    writeMIFFile();
    writeOperatingFile();
    writeControlFile();
    writeKMADR();
    writeKMOPR();
    writeKMBR();
    return true;
}

void Compiler::writeCAMFile() {
    std::ofstream camFile(camFileName, std::ios::binary);
    if (!camFile.is_open()) {
        std::cerr << "Error: Unable to open file " << camFileName << std::endl;
        return;
    }

    for (const auto &microinstruction : microinstructions) {
        std::stringstream hexStream;

        // Convert the binary instruction to a hexadecimal string
        std::string binaryString;
        for (bool bit : microinstruction) {
            binaryString += bit ? '1' : '0';
        }

        std::bitset<64> bits(binaryString);
        hexStream << std::uppercase << std::hex << bits.to_ullong();

        camFile << hexStream.str() << "\n";
    }

    camFile.close();
}

void Compiler::writeUputstvoFile() {
    std::ofstream uputstvoFile(uputstvoFileName);
    if (!uputstvoFile.is_open()) {
        std::cerr << "Error: Unable to open file " << uputstvoFileName << std::endl;
        return;
    }

    std::vector<std::pair<std::string, int>> sortedOperationalSignals(operationalSignals.begin(), operationalSignals.end());
    std::sort(sortedOperationalSignals.begin(), sortedOperationalSignals.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
        });

    std::vector<std::pair<std::string, int>> sortedControlSignals(controlSignals.begin(), controlSignals.end());
    std::sort(sortedControlSignals.begin(), sortedControlSignals.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
        });

    uputstvoFile << "Operational Signals (" << operationalSignals.size() << "): \n";
    for (size_t i = 0; i < sortedOperationalSignals.size(); ++i) {
        uputstvoFile << sortedOperationalSignals[i].first;
        if (i < sortedOperationalSignals.size() - 1) {
            uputstvoFile << ", ";
        }
    }
    for (size_t i = sortedOperationalSignals.size(); i < 64; ++i) {
        uputstvoFile << ", 0";
    }
    uputstvoFile << "\n\n";

    uputstvoFile << "Control Signals (" << controlSignals.size() << "): \n";
    if (sortedControlSignals.size() > 2) {
        uputstvoFile << "bruncnd, ";
        for (size_t i = 2; i < sortedControlSignals.size(); ++i) {
            std::string signal = sortedControlSignals[i].first;
            std::string modifiedSignal = signal;
            if (modifiedSignal[0] == '!') {
                modifiedSignal.erase(0, 1);
                modifiedSignal[0] = toupper(modifiedSignal[0]);
                modifiedSignal = "notBr" + modifiedSignal;
            }
            else {
                modifiedSignal[0] = toupper(modifiedSignal[0]);
                modifiedSignal = "br" + modifiedSignal;
            }
            uputstvoFile << modifiedSignal;
            if (i < sortedControlSignals.size() - 1) {
                uputstvoFile << ", ";
            }
        }
    }

    for (size_t i = sortedControlSignals.size() - 1; i < 16; ++i) {
        uputstvoFile << ", 0";
    }
    uputstvoFile << "\n\n";

    uputstvoFile << "Formulas for Signals:\n";
    for (const auto& [signal, index] : sortedOperationalSignals) {
        uputstvoFile << signal << " = ";
        bool first = true;
        bool found = false;
        for (size_t i = 0; i < commands.size(); ++i) {
            if (std::find(commands[i].signals.begin(), commands[i].signals.end(), signal) != commands[i].signals.end()) {
                if (!first) uputstvoFile << " + ";
                uputstvoFile << "T" << i;
                first = false;
                found = true;
            }
        }
        if (!found) {
            uputstvoFile << "0";
        }
        uputstvoFile << "\n";
    }

    uputstvoFile << "\nFormulas for Control Signals:\n";
    uputstvoFile << "bruncnd = ";
    bool first = true;
    bool found = false;
    for (size_t i = 0; i < commands.size(); ++i) {
        if (commands[i].jumpType == Command::JumpType::UNCONDITIONAL) {
            if (!first) uputstvoFile << " + ";
            uputstvoFile << "T" << i;
            first = false;
            found = true;
        }
    }
    if (!found) {
        uputstvoFile << "0";
    }
    uputstvoFile << "\n";

    for (size_t i = 2; i < sortedControlSignals.size(); ++i) {
        const auto& [controlSignal, index] = sortedControlSignals[i];
        if (controlSignal == "none") {
            continue;
        }
        uputstvoFile << controlSignal << " = ";
        first = true;
        found = false;

        if (controlSignal.rfind("case", 0) == 0) {
            size_t caseIndex = std::stoi(controlSignal.substr(4));
            uputstvoFile << "T" << caseIndex;
            found = true;
        }
        else {
            for (size_t j = 0; j < commands.size(); ++j) {
                const auto& command = commands[j];
                if (command.jumpType == Command::JumpType::IF) {
                    const auto& condition = command.condition.value();
                    std::string currentSignal = condition.negated ? "!" + condition.name : condition.name;
                    if (controlSignal == currentSignal) {
                        if (!first) uputstvoFile << " + ";
                        uputstvoFile << "T" << j;
                        first = false;
                        found = true;
                    }
                }
            }
        }

        if (!found) {
            uputstvoFile << "0";
        }
        uputstvoFile << "\n";
    }

    uputstvoFile.close();
}

void Compiler::writeMIFFile() {
    std::ofstream mifFile(mifFileName);
    if (!mifFile.is_open()) {
        std::cerr << "Error: Unable to open file " << mifFileName << std::endl;
        return;
    }

    mifFile << "DEPTH = " << microinstructions.size() << ";\n";
    mifFile << "WIDTH = " << microinstructions.front().size() << ";\n";
    mifFile << "ADDRESS_RADIX = HEX;\n";
    mifFile << "DATA_RADIX = BIN;\n";
    mifFile << "CONTENT BEGIN\n";

    for (size_t address = 0; address < microinstructions.size(); ++address) {
        std::string binaryString;
        for (bool bit : microinstructions[address]) {
            binaryString += bit ? '1' : '0';
        }
        mifFile << std::uppercase << std::hex << address << " : " << binaryString << ";\n";
    }

    mifFile << "END;\n";
    mifFile.close();
}
