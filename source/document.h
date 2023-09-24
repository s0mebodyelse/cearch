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


class Document {
  public:
    Document(const std::string &filepath, const std::string &file_extension);
    /* Document has virtual functions, so it need a virtual destructor */
    virtual ~Document() = 0;

    std::string get_file_content_as_string();
    std::unordered_map<std::string, int> get_index();
    std::string get_filepath();
    std::string get_extension();

    double get_term_frequency(std::string term);
    bool contains_term(std::string term);
    static std::vector<std::string> clean_word(std::string &word);
    void print_index();
    void index_document();
    bool needs_reindexing();
    const nlohmann::json serialize_to_json();

    /* reads only the actual content, is implemented in the derived classes */
    virtual std::string read_content();

  protected:
    std::string filepath; 
    std::string file_extension;
    std::chrono::system_clock::time_point indexed_at;
    std::unordered_map<std::string, int> index;
};

class XML_Document: public Document {
  public:
    XML_Document(const std::string &filepath);
    std::string read_content() override;

  private:
    /* helper function to traverse every node in a xml file */
    void traverse_nodes(const pugi::xml_node& root_node, std::string &content);
};

class Document_factory {
public:
  static std::unique_ptr<Document> create_document(const std::string &filepath, const std::string &extension);
};

#endif // !_H_DOCUMENT