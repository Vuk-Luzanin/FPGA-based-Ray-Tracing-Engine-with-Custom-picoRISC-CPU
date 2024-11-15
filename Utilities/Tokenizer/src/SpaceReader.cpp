#include "include/Reader.hpp"
#include <cctype>

bool SpaceReader::readToken(Context &context) const {
    bool found = false;
    const std::string &content = context.getContent();
    size_t index = context.getIndex();
    while (index < content.size() && std::isspace(static_cast<unsigned char>(content[index]))) {
        if (content[index] == '\n') {
            context.incrementLine();
        }
        ++index;
        found = true;
    }
    if (found) {
        context.setIndex(index);
    }
    return found;
}
