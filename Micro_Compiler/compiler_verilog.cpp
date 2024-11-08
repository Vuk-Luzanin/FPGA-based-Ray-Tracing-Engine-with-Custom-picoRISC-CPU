#include "compiler.hpp"
#include <fstream>
#include <iostream>
#include <bitset>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>

void Compiler::writeOperatingFile() {
    std::ofstream operatingFile(operatingFileName);
    if (!operatingFile.is_open()) {
        std::cerr << "Error: Unable to open file " << operatingFileName << std::endl;
        return;
    }

    operatingFile << "module TranslateOperating (\n";
    operatingFile << "    input  [255:0] T,\n";
    operatingFile << "    output [63:0] signals\n";
    operatingFile << ");\n\n";

    std::vector<std::pair<std::string, int>> sortedOperationalSignals(operationalSignals.begin(), operationalSignals.end());
    std::sort(sortedOperationalSignals.begin(), sortedOperationalSignals.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
        });

    if (sortedOperationalSignals.size() > 64) {
        std::cerr << "Error: More than 64 operational signals detected." << std::endl;
        return;
    }

    for (const auto& [signal, _] : sortedOperationalSignals) {
        operatingFile << "    wire " << signal << ";\n";
    }
    operatingFile << "\n";

    operatingFile << "    // Assignments for operational signals\n";
    for (const auto& [signal, _] : sortedOperationalSignals) {
        operatingFile << "    assign " << signal << " = ";
        bool first = true;
        bool found = false;

        for (size_t i = 0; i < commands.size(); ++i) {
            const auto& command = commands[i];
            if (std::find(command.signals.begin(), command.signals.end(), signal) != command.signals.end()) {
                if (!first) operatingFile << " | ";
                operatingFile << "T[" << i << "]";
                first = false;
                found = true;
            }
        }

        if (!found) {
            operatingFile << "1'b0";
        }
        operatingFile << ";\n";
    }
    operatingFile << "\n";

    operatingFile << "    assign signals = { ";
    for (size_t i = 0; i < sortedOperationalSignals.size(); ++i) {
        operatingFile << sortedOperationalSignals[i].first;
        if (i < sortedOperationalSignals.size() - 1) {
            operatingFile << ", ";
        }
    }

    size_t remainingBits = 64 - sortedOperationalSignals.size();
    for (size_t i = 0; i < remainingBits; ++i) {
        operatingFile << ", 1'b0";
    }

    operatingFile << " };\n\n";

    operatingFile << "    // ";
    for (size_t i = 0; i < sortedOperationalSignals.size(); ++i) {
        operatingFile << sortedOperationalSignals[i].first;
        if (i < sortedOperationalSignals.size() - 1) {
            operatingFile << ", ";
        }
    }
    remainingBits = 64 - sortedOperationalSignals.size();
    if (remainingBits > 1) {
        operatingFile << ", operating_symbols_temp[" << remainingBits - 1 << "..0]\n\n";
    }
    else if (remainingBits == 1) {
        operatingFile << ", operating_symbols_temp\n\n";
    }

    operatingFile << "endmodule\n";
    operatingFile.close();
}

void Compiler::writeControlFile() {
    std::ofstream controlFile(controlFileName);
    if (!controlFile.is_open()) {
        std::cerr << "Error: Unable to open file " << controlFileName << std::endl;
        return;
    }

    controlFile << "module TranslateControl (\n";
    controlFile << "    input  [255:0] T,\n";    // Input vector for microinstruction steps
    controlFile << "    input  [15:0] cond,\n";  // Additional condition signals
    controlFile << "    output bropr,\n";        // Output for the first case branch
    controlFile << "    output bradr,\n";        // Output for the second case branch
    controlFile << "    output bruncnd,\n";      // Unconditional branch signal
    controlFile << "    output brcnd,\n";        // Conditional branch signal
    controlFile << "    output [15:0] signals\n"; // Flags for when to load microprogram counter
    controlFile << ");\n\n";

    // Declarations for the internal control signals
    std::vector<std::pair<std::string, int>> sortedControlSignals(controlSignals.begin(), controlSignals.end());
    std::sort(sortedControlSignals.begin(), sortedControlSignals.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
        });
    std::vector<std::string> conditions = {}; // signals starting from element [2] onward that don't start with "case"
    std::vector<std::string> conditionsHelper = {}; // regular condition with "br" as prefix and original first letter to upper, so that camelCase workd (ex. start -> brStart)

    // Extract relevant signals into conditions and conditionsHelper vectors
    for (const auto& [signal, index] : sortedControlSignals) {
        if (index >= 2 && signal.find("case") == std::string::npos) {
            conditions.push_back(signal);
            std::string helperSignal = "br" + signal;
            helperSignal[2] = std::toupper(helperSignal[2]);  // Capitalize first letter after 'br'
            conditionsHelper.push_back(helperSignal);
        }
    }

    controlFile << "    // ";
    for (size_t i = 0; i < conditions.size(); ++i) {
        controlFile << conditions[i];
        if (i < conditions.size() - 1) {
            controlFile << ", ";
        }
    }
    for (size_t i = conditions.size(); i < 16; ++i) {
        controlFile << ", 0";
    }
    controlFile << "\n\n";

    // Declare wires for conditions and conditionsHelper
    for (const auto& signal : conditions) {
        controlFile << "    wire " << signal << ";\n";
    }
    controlFile << "\n";
    for (const auto& signal : conditionsHelper) {
        controlFile << "    wire " << signal << ";\n";
    }
    controlFile << "\n";

    for (size_t i = 0; i < conditions.size(); ++i) {
        const std::string& condition = conditions[i];
        controlFile << "    assign " << conditionsHelper[i] << " = ";
        bool firstOccurrence = true;
        for (size_t j = 0; j < commands.size(); ++j) {
            const auto& command = commands[j];
            if (command.jumpType == Command::IF && command.condition->name == condition) {
                if (!firstOccurrence) controlFile << " | ";
                controlFile << "T[" << j << "]";
                firstOccurrence = false;
            }
        }
        if (firstOccurrence) {
            controlFile << "1'b0";
        }
        controlFile << ";\n";
    }
    controlFile << "\n";

    // Compute bruncnd: OR of all unconditional branches
    controlFile << "    assign bruncnd = ";
    bool firstBrUncond = true;
    for (size_t i = 0; i < commands.size(); ++i) {
        if (commands[i].jumpType == Command::UNCONDITIONAL) {
            if (!firstBrUncond) controlFile << " | ";
            controlFile << "T[" << i << "]";
            firstBrUncond = false;
        }
    }
    controlFile << ";\n\n";

    // Compute brcnd: OR of all conditional branches (AND with the corresponding cond input signal)
    controlFile << "    assign brcnd = ";
    bool firstBrCond = true;
    for (size_t i = 0; i < conditions.size(); ++i) {
        if (!firstBrCond) controlFile << " | ";
        controlFile << "(" << conditions[i] << " & " << conditionsHelper[i] << ")"; // AND with helper signal
        firstBrCond = false;
    }
    controlFile << ";\n\n";

    // Compute bradr: First case branch address (assume bradr is the first case)
    controlFile << "    assign bradr = ";
    bool foundCase1 = false;
    for (size_t i = 0; i < commands.size(); ++i) {
        if (commands[i].jumpType == Command::CASE) {
            controlFile << "T[" << i << "]";
            foundCase1 = true;
            break;
        }
    }
    if (!foundCase1) {
        controlFile << "1'b0"; // Default to 0 if no case branch is found
    }
    controlFile << ";\n\n";

    // Compute bropr: Second case branch address (assume bropr is the second case)
    controlFile << "    assign bropr = ";
    bool foundCase2 = false;
    int caseCount = 0;
    for (size_t i = 0; i < commands.size(); ++i) {
        if (commands[i].jumpType == Command::CASE) {
            caseCount++;
            if (caseCount == 2) {
                controlFile << "T[" << i << "]";
                foundCase2 = true;
                break;
            }
        }
    }
    if (!foundCase2) {
        controlFile << "1'b0"; // Default to 0 if no second case branch is found
    }
    controlFile << ";\n\n";

    // Compute signals[15:0]: First declare each valX separately, then assign them to the output
    std::map<int, std::string> valSignals;

    // Declare each valX separately
    for (size_t i = 0; i < commands.size(); ++i) {
        if (commands[i].jumpDestination) {
            int destination = std::stoi(*commands[i].jumpDestination);
            if (valSignals.find(destination) == valSignals.end()) {
                valSignals[destination] = "T[" + std::to_string(i) + "]";
            }
            else {
                valSignals[destination] += " | T[" + std::to_string(i) + "]";
            }
        }
    }

    // Declare individual valX signals
    std::vector<std::string> signalList;
    size_t signalIndex = 0;
    for (const auto& [valIndex, valExpr] : valSignals) {
        controlFile << "    wire val" << valIndex << " = " << valExpr << ";\n";
        signalList.push_back("val" + std::to_string(valIndex));
        signalIndex++;
    }
    controlFile << "\n";

    // Assign signals[15:0] based on valX signals, padded with zeros
    controlFile << "    assign signals = { ";
    for (size_t i = 0; i < signalList.size(); ++i) {
        controlFile << signalList[i];
        if (i < signalList.size() - 1) {
            controlFile << ", ";
        }
    }

    // Pad with zeros if needed
    for (size_t i = signalList.size(); signalIndex < 16; ++signalIndex) {
        if (i > 0) {
            controlFile << ", ";
        }
        controlFile << "1'b0";
    }
    controlFile << " };\n\n";

    controlFile << "    // ";
    for (size_t i = 0; i < signalList.size(); ++i) {
        controlFile << signalList[i];
        if (i < signalList.size() - 1) {
            controlFile << ", ";
        }
    }

    size_t remainingBits = 16 - signalList.size();
    if (remainingBits > 1) {
        controlFile << ", control_symbols_temp[" << remainingBits - 1 << "..0]\n\n";
    }
    else if (remainingBits == 1) {
        controlFile << ", control_symbols_temp\n\n";
    }

    // End the module
    controlFile << "endmodule\n";
    controlFile.close();
}

void Compiler::writeKMADR() {
    std::ofstream kmadrFile(kmadrFileName);

    // Writing KMADR module
    if (kmadrFile.is_open()) {
        kmadrFile << "module KMADR (\n";
        kmadrFile << "    input regdir,\n";
        kmadrFile << "    input imm,\n";
        kmadrFile << "    input memdir,\n";
        kmadrFile << "    input regind,\n";
        kmadrFile << "    input regindpom,\n";
        kmadrFile << "    output [7:0] address\n";
        kmadrFile << ");\n\n";

        // Find the first CASE statement and assign addresses for imm, memdir, regind, regindpom
        bool foundFirstCase = false;
        for (size_t i = 0; i < commands.size(); ++i) {
            const auto& command = commands[i];
            if (command.jumpType == Command::CASE) {
                foundFirstCase = true;
                kmadrFile << "    assign address = \n";
                for (const auto& [cond, address] : command.caseActions) {
                    kmadrFile << "                    " << cond.name << " ? 8'd" << address << " : \n";
                }
                kmadrFile << "                    8'b00000000;\n\n";
                break; // Stop after processing the first CASE statement
            }
        }

        if (!foundFirstCase) {
            std::cerr << "Error: No CASE statement found for KMADR.\n";
        }

        // End KMADR module
        kmadrFile << "endmodule\n";
        kmadrFile.close();
    }
    else {
        std::cerr << "Error: Unable to open KMADR file for writing.\n";
    }
}

void Compiler::writeKMOPR() {
    std::ofstream kmoprFile(kmoprFileName);

    if (!kmoprFile.is_open()) {
        std::cerr << "Error: Unable to open KMOPR file for writing.\n";
        return;
    }

    kmoprFile << "module KMOPR (\n";
    kmoprFile << "    input [31:0] signals,\n";
    kmoprFile << "    output [7:0] address\n";
    kmoprFile << ");\n\n";

    size_t caseIndex = 0;
    int caseCount = 0;
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& command = commands[i];
        if (command.jumpType != Command::CASE) {
            continue;
        }
        caseCount++;
        if (caseCount == 2) {
            caseIndex = i;
            break;
        }
    }

    if (caseCount < 2) {
        kmoprFile << "    // No second case found\n";
    }
    else {
        const auto& command = commands[caseIndex];

        for (size_t signalIndex = 0; signalIndex < command.caseActions.size(); ++signalIndex) {
            const auto& [condition, address] = command.caseActions[signalIndex];

            kmoprFile << "    wire " << condition.name << " = signals[" << signalIndex << "];\n";
        }

        kmoprFile << "    // ";
        for (size_t i = 0; i < command.caseActions.size(); ++i) {
            kmoprFile << command.caseActions[i].first.name;
            if (i < command.caseActions.size() - 1) {
                kmoprFile << ", ";
            }
        }
        for (size_t i = command.caseActions.size(); i < 32; ++i) {
            kmoprFile << ", 0";
        }
        kmoprFile << "\n\n";

        kmoprFile << "    assign address = \n";
        for (size_t signalIndex = 0; signalIndex < command.caseActions.size(); ++signalIndex) {
            const auto& [condition, address] = command.caseActions[signalIndex];

            kmoprFile << "        " << condition.name << " ? 8'd" << address;
            if (signalIndex < command.caseActions.size() - 1) {
                kmoprFile << " :\n";
            }
            else {
                kmoprFile << " : 8'b00000000;\n";
            }
        }
    }

    // End KMOPR module
    kmoprFile << "endmodule\n";
    kmoprFile.close();
}

void Compiler::writeKMBR() {
    std::ofstream kmbrFile(kmbrFileName);

    // Writing KMBR module
    if (kmbrFile.is_open()) {
        kmbrFile << "module KMBR (\n";
        kmbrFile << "    input [15:0] signals,\n"; // 16-bit input for signals
        kmbrFile << "    output [7:0] address\n";  // 8-bit output address
        kmbrFile << ");\n\n";

        // Gather unique valX addresses from commands
        std::set<int> valSet;  // Use set to store unique addresses

        for (size_t i = 0; i < commands.size(); ++i) {
            const auto& command = commands[i];

            // Check if the command has a jump destination
            if (command.jumpDestination) {
                int destination = std::stoi(*command.jumpDestination);
                valSet.insert(destination);  // Insert the destination into the set
            }
        }

        std::vector<int> valX(valSet.begin(), valSet.end());

        kmbrFile << "    wire [7:0] addressTemp;\n";
        kmbrFile << "    assign addressTemp = 8'b00000000;\n";

        for (size_t i = 0; i < valX.size(); i++) {
            kmbrFile << "    wire val" << valX[i] << " = signals[" << i << "];\n";
        }
        kmbrFile << "\n";

        kmbrFile << "    assign address = \n";
        bool first = true;

        for (const int& valX : valSet) {
            if (!first) kmbrFile << " | \n";
            kmbrFile << "        (val" << valX << " ? 8'd" << valX << " : 8'b00000000)";
            first = false;
        }

        kmbrFile << ";\n\n";

        kmbrFile << "endmodule\n";
        kmbrFile.close();
    }
    else {
        std::cerr << "Error: Unable to open KMBR file for writing.\n";
    }
}
