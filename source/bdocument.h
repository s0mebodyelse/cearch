#ifndef _H_BDOCUMENT
#define _H_BDOCUMENT

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

/* Base Document Class */
class BDocument {
   public:
    BDocument(std::string filepath, std::string file_extension);

    /* needs to be virtual, so it can be implemented in the derived classes */
    virtual ~BDocument();
    virtual std::string read_content() = 0;

    std::unordered_map<std::string, int> get_concordance();
    void print_tfidf_scores();

    void insert_tfidf_score(std::pair<std::string, double> tfidf_score);
    void index_document();

    double get_tfidf_score(const std::string &term);
    int get_term_frequency(const std::string &term);

    std::string get_filepath();
    std::string get_extension();
    std::string get_file_content_as_string();

    bool contains_term(const std::string &term);
    bool needs_reindexing();
    static std::vector<std::string> clean_word(std::string &word);

   protected:
    std::string filepath;
    std::string file_extension;
    std::chrono::system_clock::time_point indexed_at;

    /* every term in the document and counter for that term */
    std::unordered_map<std::string, int> concordance;
    /* every term in the document and its tfidf score */
    std::unordered_map<std::string, double> tfidf_scores;
};

/* a Factory which returns Document objects depending on their file extensions */
class BDocument_factory {
   public:
    static std::unique_ptr<BDocument> create_document(
        const std::string &filepath, const std::string &extension);
};

#endif