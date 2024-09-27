#ifndef _H_PDFCONTENTSTRATEGY
#define _H_PDFCONTENTSTRATEGY

#include "ContentStrategy.h"
#include "PDFDocument.h"

#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

class PDFContentStrategy: public ContentStrategy<PDFDocument> {
   public:
    explicit PDFContentStrategy() {};

    /*
    *   Actual implementation of the read content function for PDF's
    *   const &document -> function cant change the document passed in
    *   const -> after a function, const means the function cant change any Data members,
    *   of the class it belongs to (PDFContentStrategy)
    */
    std::string read_content(PDFDocument const &document) const {
        std::string fulltext;

        std::unique_ptr<poppler::document> doc{poppler::document::load_from_file(document.get_filepath())};
        //doc = std::make_unique<poppler::document>(poppler::document::load_from_file(document.get_filepath()));

        if (!doc) {
            throw std::runtime_error("Error: Could not open the PDF file!");
        }

        for (int i = 0; i < doc->pages(); ++i) {
            std::unique_ptr<poppler::page> page(doc->create_page(i));
            if (page) {
                fulltext.append(page->text().to_latin1());
                fulltext.append("\n");
            }
        }

        return fulltext;
    }
};

#endif