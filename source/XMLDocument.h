#ifndef _XMLDOCUMENT_H
#define _XMLDOCUMENT_H

#include "Document.h"

/* XML Parsing */
#include <pugixml.hpp>

class XMLDocument : public Document {
   public:
    XMLDocument(const std::string &filepath);
    std::string read_content() const override;

   private:
    /* helper function to traverse every node in a xml file */
    void traverse_nodes(const pugi::xml_node &root_node, std::string &content) const;
};

#endif