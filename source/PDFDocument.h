#ifndef _PDFDOCUMENT_H
#define _PDFDOCUMENT_H

#include <memory>
#include <utility>

#include "Document.h"
#include "ContentStrategy.h"

class PDFDocument : public Document {
   public:
    using ContentStrategyPDF = ContentStrategy<PDFDocument>;

    explicit PDFDocument(const std::string& filepath, std::unique_ptr<ContentStrategyPDF> content)
        : Document(filepath, "pdf"), content_(std::move(content))
    {
        /* check for nullptr? */
    }

    std::string read_content() const override {
        std::string fulltext = content_->read_content(*this);
        return fulltext;
    }

   private:
    std::unique_ptr<ContentStrategyPDF> content_;
};

#endif