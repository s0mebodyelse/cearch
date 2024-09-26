#ifndef _TEXT_H
#define _TEXT_H

#include "document.h"

class Text_Document : public Document {
   public:
    Text_Document(const std::string &filepath);
    std::string read_content() const override;

   private:
};

#endif 