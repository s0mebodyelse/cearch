#include <iostream>

#include "pdf_document.h"
#include <poppler/cpp/poppler-page.h>


PDF_Document::PDF_Document(const std::string& filepath)
    : Document(filepath, "pdf"),
      doc(poppler::document::load_from_file(filepath)) {
    if (!doc) {
        throw std::runtime_error("Error: Could not open the PDF file!");
    }
}

std::string PDF_Document::read_content() {
    std::string fullText;

    for (int i = 0; i < doc->pages(); ++i) {
        std::unique_ptr<poppler::page> page(doc->create_page(i));
        if (page) {
            fullText.append(page->text().to_latin1());
            fullText.append("\n");
        }
    }

    return fullText;
}