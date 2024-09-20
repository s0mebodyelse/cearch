#ifndef _TEXT_H
#define _TEXT_H

#include "bdocument.h"

class Text_Document : public BDocument {
   public:
    Text_Document(const std::string &filepath);
    std::string read_content() override;

   private:
};

#endif 