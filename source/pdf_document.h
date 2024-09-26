#ifndef _PDF_H
#define _PDF_H

#include <memory>
#include <utility>

#include "document.h"
#include "ContentStrategy.h"

class PDF_Document : public Document {
   public:
    using ContentStrategyPDF = ContentStrategy<PDF_Document>;

    explicit PDF_Document(const std::string& filepath, std::unique_ptr<ContentStrategyPDF> content)
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