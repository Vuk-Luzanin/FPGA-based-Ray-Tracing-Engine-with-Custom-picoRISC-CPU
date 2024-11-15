#include "include/Reader.hpp"

SymbolReader::SymbolReader(const std::vector<std::string> &allowedSymbols)
    : allowedSymbols(allowedSymbols) {}

bool SymbolReader::readToken(Context &context) const {
    const std::string &content = context.getContent();
    size_t index = context.getIndex();

    for (const auto &symbol : allowedSymbols) {
        if (content.compare(index, symbol.size(), symbol) == 0) {
            context.tokens.emplace_back(Token::Type::Symbol, symbol, context.getCurrentLine());
            context.incrementIndex(symbol.size());
            return true;
        }
    }
    return false;
}
