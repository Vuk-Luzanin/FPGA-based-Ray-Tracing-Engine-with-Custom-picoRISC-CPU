#include "include/Context.hpp"

Context::Context(const std::string &content, const std::string &filename)
    : content(content), filename(filename), index(0), currentLine(1) {}

const std::string &Context::getContent() const {
    return content;
}

const std::string &Context::getFilename() const {
    return filename;
}

size_t Context::getIndex() const {
    return index;
}

void Context::incrementIndex(size_t value) {
    index += value;
}

void Context::setIndex(size_t newIndex) {
    index = newIndex;
}

size_t Context::getCurrentLine() const {
    return currentLine;
}

void Context::incrementLine() {
    ++currentLine;
}