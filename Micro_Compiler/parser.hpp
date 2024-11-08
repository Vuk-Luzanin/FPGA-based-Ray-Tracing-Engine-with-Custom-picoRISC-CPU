#pragma once
#include "tokenizer.hpp"
#include "parser_error.hpp"
#include <vector>
#include <string>
#include <optional>

struct Condition {
    bool negated = false;
    std::string name;
};

struct Command {
    enum JumpType {
        NONE,
        UNCONDITIONAL,
        IF,
        CASE
    };
    std::optional<std::string> label;
    std::vector<std::string> signals;
    JumpType jumpType = NONE;
    std::optional<Condition> condition;
    std::vector<std::pair<Condition, std::string>> caseActions;
    std::optional<std::string> jumpDestination;

    std::string toStringWithoutLabelDeclarations() const;

    friend std::ostream& operator<<(std::ostream& os, const Command& command);
};

class Parser {
public:
    Parser(const std::string& fileName) : fileName(fileName) {}

    bool parse(const std::vector<Token>& tokens);
    std::vector<Command> getCommands() const { return commands; }
    std::vector<ParserError> getErrors() const { return errors; }

private:
    std::vector<Command> commands;
    std::vector<ParserError> errors;
    std::string fileName;

    bool parseCommand(const std::vector<Token>& tokens, size_t& index, Command& command);
    bool parseSignals(const std::vector<Token>& tokens, size_t& index, Command& command);
    bool parseJump(const std::vector<Token>& tokens, size_t& index, Command& command);
    bool parseCase(const std::vector<Token>& tokens, size_t& index, Command& command);
    bool parseIf(const std::vector<Token>& tokens, size_t& index, Command& command);

    void addError(const std::string& message, int lineNumber);
};
