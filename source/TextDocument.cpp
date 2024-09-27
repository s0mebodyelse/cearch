#include <fstream>

#include "TextDocument.h"

TextDocument::TextDocument(const std::string &filepath)
    : Document(filepath, "txt") {}

std::string TextDocument::read_content() const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}