#include "parser.hpp"
#include <iostream>
#include <sstream>

void Parser::addError(const std::string& message, int lineNumber) {
    errors.emplace_back(message, fileName, lineNumber);
}

bool Parser::parse(const std::vector<Token>& tokens) {
    size_t index = 0;
    while (index < tokens.size()) {
        Command command;
        if (!parseCommand(tokens, index, command)) {
            addError("Failed to parse command.", index < tokens.size() ? tokens[index].line : tokens[index - 1].line);
            return false;
        }
        commands.push_back(command);
    }
    return true;
}

bool Parser::parseCommand(const std::vector<Token>& tokens, size_t& index, Command& command) {
    if (index + 1 < tokens.size() && tokens[index].type == TokenType::Name && tokens[index + 1].value == ":") {
        command.label = tokens[index].value;
        index += 2;  // Skip over label and colon
    }

    if (!parseSignals(tokens, index, command)) {
        addError("Failed to parse signals.", tokens[index].line);
        return false;
    }

    if (index >= tokens.size()) {
        addError("Expected ';' at the end of command.", tokens[index - 1].line);
        return false;
    }

    if (tokens[index].value == "br") {
        if (!parseJump(tokens, index, command)) {
            addError("Failed to parse jump.", tokens[index].line);
            return false;
        }
    }
    else {
        command.jumpType = Command::JumpType::NONE;
    }

    if (index >= tokens.size()) {
        addError("Expected ';' at the end of command.", tokens[index - 1].line);
        return false;
    }

    if (tokens[index].value == ";") {
        index++;
    }
    else {
        addError("Expected ';' at end of command.", tokens[index].line);
        return false;
    }

    return true;
}

bool Parser::parseSignals(const std::vector<Token>& tokens, size_t& index, Command& command) {
    while (index < tokens.size() && tokens[index].type == TokenType::Name) {
        command.signals.push_back(tokens[index].value);
        index++;

        if (index < tokens.size() && tokens[index].value == ",") {
            index++;  // Continue parsing more signals
        }
        else if (index < tokens.size() && (tokens[index].value == ";" || tokens[index].value == "br")) {
            break;
        }
        else {
            addError("Unexpected token in signal list: " + (index < tokens.size() ? tokens[index].value : "EOF"), index < tokens.size() ? tokens[index].line : -1);
            return false;
        }
    }

    return true;
}

bool Parser::parseCase(const std::vector<Token>& tokens, size_t& index, Command& command) {
    command.jumpType = Command::JumpType::CASE;
    index++;

    if (index >= tokens.size() || tokens[index].value != "(") {
        addError("Expected '(' after 'case'.", tokens[index].line);
        return false;
    }
    index++;

    // Collect conditions
    std::vector<Condition> conditions;
    while (index < tokens.size() && tokens[index].value != ")" && tokens[index].type == TokenType::Name) {
        bool negated = false;
        if (tokens[index].value == "!") {
            negated = true;
            index++;
        }
        if (index >= tokens.size()) {
            addError("Expected condition after '!' operator.", tokens[index - 1].line);
            return false;
        }
        conditions.push_back(Condition{ negated, tokens[index].value });
        index++;
        if (index >= tokens.size()) {
            addError("Unexpected end.", tokens[index - 1].line);
            return false;
        }
        if (tokens[index].value == ")") {
            break;
        }
        else if (tokens[index].value != ",") {
            addError("Expected ',' after condition '" + tokens[index - 1].value + ";.", tokens[index].line);
            return false;
        }
        index++;
    }

    if (index >= tokens.size() || tokens[index].value != ")") {
        addError("Expected ')' after conditions.", tokens[index].line);
        return false;
    }
    index++;

    if (index >= tokens.size() || tokens[index].value != "then") {
        addError("Expected 'then' after conditions.", tokens[index].line);
        return false;
    }
    index++;

    if (index >= tokens.size() || tokens[index].value != "(") {
        addError("Expected '(' after 'then'.", tokens[index].line);
        return false;
    }
    index++;

    // Collect destinations
    std::vector<std::string> destinations;
    while (index < tokens.size() && tokens[index].value != ")" && tokens[index].type == TokenType::Name) {
        destinations.push_back(tokens[index].value);
        index++;
        if (index >= tokens.size()) {
            addError("Unexpected end.", tokens[index - 1].line);
            return false;
        }
        if (tokens[index].value == ")") {
            break;
        }
        else if (tokens[index].value != ",") {
            addError("Expected ',' after destination '" + tokens[index - 1].value + ";.", tokens[index].line);
            return false;
        }
        index++;
    }

    if (index >= tokens.size() || tokens[index].value != ")") {
        addError("Expected ')' after destinations.", tokens[index].line);
        return false;
    }
    index++;

    if (index >= tokens.size() || tokens[index].value != ")") {
        addError("Expected ')' after case.", tokens[index].line);
        return false;
    }
    index++;

    // Ensure the number of conditions and destinations match
    if (conditions.size() != destinations.size()) {
        addError("Mismatch between number of conditions and destinations.", tokens[index].line);
        return false;
    }

    // Create vector of pairs
    for (size_t i = 0; i < conditions.size(); ++i) {
       command.caseActions.emplace_back(conditions[i], destinations[i]);
    }

    return true;
}

bool Parser::parseIf(const std::vector<Token>& tokens, size_t& index, Command& command) {
    command.jumpType = Command::JumpType::IF;
    index++;
    if (index >= tokens.size()) {
        addError("Unexpected end.", tokens[index].line);
        return false;
    }
    bool negated = false;
    if (tokens[index].value == "!") {
        negated = true;
        index++;
    }
    if (index >= tokens.size() || tokens[index].type != TokenType::Name) {
        addError("Expected condition after 'if'.", tokens[index].line);
        return false;
    }
    
    command.condition = { negated, tokens[index].value };
    index++;

    if (index >= tokens.size() || tokens[index].value != "then") {
        addError("Expected 'then' after condition.", tokens[index].line);
        return false;
    }
    index++;
    if (index >= tokens.size() || tokens[index].type != TokenType::Name) {
        addError("Expected jump destination after 'then'.", tokens[index].line);
        return false;
    }
    command.jumpDestination = tokens[index].value;
    index++;
    if (index >= tokens.size() || tokens[index].value != ")") {
        addError("Expected ')' after if.", tokens[index].line);
        return false;
    }
    index++;
    return true;
}

bool Parser::parseJump(const std::vector<Token>& tokens, size_t& index, Command& command) {
    index++;  // Skip over "br"

    if (index >= tokens.size()) {
        addError("Unexpected end of input after 'br'.", tokens[index - 1].line);
        return false;
    }

    if (tokens[index].type == TokenType::Name) {
        command.jumpType = Command::JumpType::UNCONDITIONAL;
        command.jumpDestination = tokens[index].value;
        index++;
        return true;
    }

    if (tokens[index].value != "(") {
        addError("Unexpected token after 'br'.", tokens[index].line);
        return false;
    }

    index++;

    if (index >= tokens.size()) {
        addError("Expected 'if' or 'case' after '('.", tokens[index - 1].line);
        return false;
    }

    if (tokens[index].value == "if") {
        if (!parseIf(tokens, index, command)) {
            addError("Failed to parse if.", tokens[index].line);
            return false;
        }
    }
    else if (tokens[index].value == "case") {
        if (!parseCase(tokens, index, command)) {
            addError("Failed to parse case.", tokens[index].line);
            return false;
        }
    }
    else {
        addError("Unexpected token after 'br': " + tokens[index].value, tokens[index].line);
        return false;
    }

    return true;
}

std::ostream& operator<<(std::ostream& os, const Command& command) {
    if (command.label.has_value()) {
        os << command.label.value() << ": ";
    }
    os << command.toStringWithoutLabelDeclarations();
    return os;
}

std::string Command::toStringWithoutLabelDeclarations() const {
    std::ostringstream oss;
    if (!signals.empty()) {
        for (const auto& signal : signals) {
            oss << signal;
            if (signal != signals.back()) {
                oss << ", ";
            }
        }
    }
    if (!signals.empty() && jumpType != JumpType::NONE) {
        oss << ", ";
    }
    if (jumpType == JumpType::UNCONDITIONAL) {
        oss << "br " << jumpDestination.value();
    }
    if (jumpType == JumpType::IF) {
        oss << "br (if " << (condition.value().negated ? "!" : "") << condition.value().name << " then " << jumpDestination.value() << ")";
    }
    if (jumpType == JumpType::CASE) {
        oss << "br (case ";
        std::vector<Condition> conditions;
        std::vector<std::string> destinations;
        for (const auto& [c, d] : caseActions) {
            conditions.push_back(c);
            destinations.push_back(d);
        }
        oss << "(";
        int ccounter = 0;
        for (const auto& c : conditions) {
            ccounter++;
            oss << (c.negated ? "!" : "") << c.name;
            if (ccounter != conditions.size()) {
                oss << ", ";
            }
        }
        oss << ") then (";
        for (const auto& d : destinations) {
            oss << d;
            if (d != destinations.back()) {
                oss << ", ";
            }
        }
        oss << "))";
    }

    oss << ";";

    return oss.str();

}