#include "bdocument.h"

#include "pdf_document.h"
#include "xml_document.h"
#include "text_document.h"

/* Base Document Class */
BDocument::BDocument(std::string filepath, std::string file_extension)
    : filepath(filepath), file_extension(file_extension) {}

BDocument::~BDocument() {}

std::unordered_map<std::string, int> BDocument::get_concordance() {
    return concordance;
}

void BDocument::print_tfidf_scores() {
    std::cout << "Doc: " << filepath << std::endl;
    for (const auto &entry: tfidf_scores) {
        std::cout << entry.first << " Score: " << entry.second << std::endl;
    }    
}

void BDocument::insert_tfidf_score(std::pair<std::string, double> tfidf_score) {
    tfidf_scores.insert(tfidf_score);
}

/*
 * 	returns the tfidf score of the term
 * 	returns 0 if the term is not in the Document
 */
double BDocument::get_tfidf_score(const std::string &term) {
    if (tfidf_scores.contains(term)) {
        return tfidf_scores.at(term);
    }

    return 0.0;
}

std::string BDocument::get_filepath() { return filepath; }

std::string BDocument::get_extension() { return file_extension; }

/* number of times, a word occurs in a given document */
int BDocument::get_term_frequency(const std::string &term) {
    if (concordance.find(term) != concordance.end()) {
        return concordance.at(term);
    }

    return 0;
}

std::string BDocument::get_file_content_as_string() {
    std::string file_content = read_content();
    return file_content;
}

bool BDocument::contains_term(const std::string &term) {
    if (concordance.find(term) != concordance.end()) {
        return true;
    }
    return false;
}

void BDocument::index_document() {
    std::string word;
    std::istringstream iss(this->read_content());

    std::cout << "Indexing doc : " << this->get_filepath() << std::endl;

    while (iss >> word) {
        /* split the word if necessary */
        std::vector<std::string> clean_words = clean_word(word);
        for (auto &clean_word : clean_words) {
            if (clean_word.find_first_not_of(' ') != std::string::npos) {
                if (concordance.find(clean_word) != concordance.end()) {
                    concordance[clean_word]++;
                } else {
                    concordance[clean_word] = 1;
                }
            }
        }
    }

    indexed_at = std::chrono::system_clock::now();
}

std::vector<std::string> BDocument::clean_word(std::string &word) {
    std::vector<std::string> clean_words;
    /* transform every word to lower case letters */
    std::transform(word.begin(), word.end(), word.begin(),
                   [](auto c) { return std::tolower(c); });

    /* remove special characters */
    std::replace_if(
        word.begin(), word.end(),
        [](auto c) { return std::ispunct(c) || std::isdigit(c); }, ' ');

    /* split the word if neccessary and append to result vector */
    std::stringstream iss(word);
    std::string split_word;
    while (iss >> split_word) {
        std::cout << split_word << std::endl;
        clean_words.push_back(split_word);
    }
    return clean_words;
}

/*
 * checks indexed_at time against the last modification of the file
 * if the file was modified after it was indexed, it needs to be indexed again,
 * and the function returns true, otherwise the function returns false
 */
bool BDocument::needs_reindexing() {
    try {
        std::filesystem::file_time_type ftime =
        std::filesystem::last_write_time(this->get_filepath());
        std::chrono::time_point tp = std::chrono::file_clock::to_sys(ftime);
        return tp > this->indexed_at;
    } catch (std::exception &e) {
        std::cerr << "Error ocurred: " << e.what() << std::endl;
        return false;
    }
}

/* Document Factory implementation */
std::unique_ptr<BDocument> BDocument_factory::create_document(
    const std::string &filepath, const std::string &extension) {

    if (extension == ".xml" || extension == ".xhtml") {
        return std::make_unique<XML_Document>(filepath);
    }

    if (extension == ".txt") {
        return std::make_unique<Text_Document>(filepath);
    }

    if (extension == ".pdf") {
        try {
            return std::make_unique<PDF_Document>(filepath);
        } catch (std::exception &e) {
            std::cerr << "Exception caught creating pdf document: " << e.what()
                      << std::endl;
        }
    }

    throw std::runtime_error(std::string("Document " + filepath + " " + extension + " not supported"));
};