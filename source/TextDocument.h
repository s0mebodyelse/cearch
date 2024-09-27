#ifndef _TEXTDOCUMENT_H
#define _TEXTDOCUMENT_H

#include "Document.h"

class TextDocument : public Document {
   public:
    TextDocument(const std::string &filepath);
    std::string read_content() const override;

   private:
};

#endif 