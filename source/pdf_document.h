#ifndef PDF_H
#define PDF_H

#include <string>
#include <memory>
#include <stdexcept>

#include <poppler/cpp/poppler-document.h>

#include "bdocument.h"

class PDF_Document: public BDocument {
public:
    PDF_Document(const std::string &filepath);
    std::string read_content() override;

private:
    std::unique_ptr<poppler::document> doc;
};

#endif