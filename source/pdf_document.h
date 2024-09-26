#ifndef _PDF_H
#define _PDF_H

#include <poppler/cpp/poppler-document.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "document.h"

class PDF_Document : public Document {
   public:
    PDF_Document(const std::string &filepath);
    std::string read_content() override;

   private:
    std::unique_ptr<poppler::document> doc;
};

#endif