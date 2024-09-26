#ifndef _XML_H
#define _XML_H

#include "document.h"

/* XML Parsing */
#include <pugixml.hpp>

class XML_Document : public Document {
   public:
    XML_Document(const std::string &filepath);
    std::string read_content() const override;

   private:
    /* helper function to traverse every node in a xml file */
    void traverse_nodes(const pugi::xml_node &root_node, std::string &content) const;
};

#endif