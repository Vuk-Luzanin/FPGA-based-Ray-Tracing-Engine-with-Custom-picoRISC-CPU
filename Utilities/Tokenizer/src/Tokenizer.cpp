#include "include/Tokenizer.hpp"

Tokenizer::Tokenizer(const std::vector<std::string> &allowedSymbols) {
    initReaders(allowedSymbols);
}

void Tokenizer::initReaders(const std::vector<std::string> &allowedSymbols) {
    readers.emplace_back(std::make_unique<SpaceReader>());
    readers.emplace_back(std::make_unique<NumberReader>());
    readers.emplace_back(std::make_unique<SymbolReader>(allowedSymbols));
}

bool Tokenizer::tokenize(const std::string &input, const std::string &filename) {
    Context context(input, filename);
    while (context.getIndex() < context.getContent().size()) {
        bool matched = false;
        for (const auto &reader : readers) {
            if (reader->readToken(context)) {
                matched = true;
                break;
            }
        }
        if (!matched) {
            context.errors.emplace_back("Unknown token", filename, context.getCurrentLine());
            context.incrementIndex();
        }
    }
    tokens.insert(tokens.end(),
                 std::make_move_iterator(context.tokens.begin()),
                 std::make_move_iterator(context.tokens.end()));
    errors.insert(errors.end(),
                 std::make_move_iterator(context.errors.begin()),
                 std::make_move_iterator(context.errors.end()));
    return !hasErrors();
}

const std::vector<Token> &Tokenizer::getTokens() const {
    return tokens;
}

const std::vector<Error> &Tokenizer::getErrors() const {
    return errors;
}

bool Tokenizer::hasErrors() const {
    return !errors.empty();
}
