#include "text_document.h"

Text_Document::Text_Document(const std::string &filepath)
    : Document(filepath, "txt") {}

std::string Text_Document::read_content() const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}