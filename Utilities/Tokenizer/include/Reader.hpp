#pragma once
#include "Context.hpp"

class Reader {
public:
    virtual ~Reader() = default;
    virtual bool readToken(Context &context) const = 0;
};

class SpaceReader : public Reader {
public:
    bool readToken(Context &context) const override;
};

class NumberReader : public Reader {
public:
    bool readToken(Context &context) const override;
};

class SymbolReader : public Reader {
public:
    SymbolReader(const std::vector<std::string> &allowedSymbols);
    bool readToken(Context &context) const override;

private:
    std::vector<std::string> allowedSymbols;
};
