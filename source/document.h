#ifndef _H_DOCUMENT 
#define _H_DOCUMENT

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>
#include <queue>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <utility>
#include <memory>
#include <stdexcept>

/* XML Parsing */
#include <pugixml.hpp>
#include <nlohmann/json.hpp>
#include <mupdf/fitz.h>

using json = nlohmann::json;

class Document {
  public:
    /* standard constructor */
    Document(std::string filepath, std::string file_extension);

    /* constructor the be used for reading from json file */
    Document(
      std::string filepath, 
      std::string file_extension,
      std::chrono::system_clock::time_point indexed_at,
      std::unordered_map<std::string, int> concordance,
      std::unordered_map<std::string, double> tfidf_scores
    );

    /* needs to be virtual, so it can be implemented in the derived classes */
    virtual ~Document();
    virtual std::string read_content();

    std::unordered_map<std::string, int> get_concordance();

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

    /* json stuff */
    const json serialize_to_json();
    static Document from_json(const json &j);

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
class Document_factory {
public:
  static std::unique_ptr<Document> create_document(const std::string &filepath, const std::string &extension);
};

class XML_Document: public Document {
  public:
    XML_Document(const std::string &filepath);
    ~XML_Document();
    std::string read_content() override;

  private:
    /* helper function to traverse every node in a xml file */
    void traverse_nodes(const pugi::xml_node& root_node, std::string &content);
};

class Text_Document: public Document {
  public:
    Text_Document(const std::string &filepath);
    ~Text_Document();
    std::string read_content() override;

  private:
};

class PDF_Document: public Document {
  public:
    PDF_Document(const std::string &filepath);
    ~PDF_Document();
    std::string read_content() override;

  private:
    fz_context *ctx;
    fz_document *doc;
};

#endif 