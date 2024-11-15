#include "include/Reader.hpp"
#include <cctype>

bool NumberReader::readToken(Context &context) const {
    const std::string &content = context.getContent();
    size_t index = context.getIndex();
    if (!std::isdigit(static_cast<unsigned char>(content[index]))) {
        return false;
    }

    size_t start = index;
    while (index < content.size() && std::isdigit(static_cast<unsigned char>(content[index]))) {
        ++index;
    }

    if (index < content.size() && std::isalpha(static_cast<unsigned char>(content[index]))) {
        context.errors.emplace_back("Invalid token: mixed digits and letters", context.getFilename(), context.getCurrentLine());
        while (index < content.size() && std::isalnum(static_cast<unsigned char>(content[index]))) {
            ++index;
        }
        context.setIndex(index);
        return true;
    }

    std::string number = content.substr(start, index - start);
    context.tokens.emplace_back(Token::Type::Number, number, context.getCurrentLine());
    context.setIndex(index);
    return true;
}
