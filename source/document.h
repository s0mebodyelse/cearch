#ifndef _H_DOCUMENT
#define _H_DOCUMENT

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>
#include <queue>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <fstream>

/* XML Parsing */
#include <pugixml.hpp>
#include <utility>


class Document {
  public:
    Document(const std::string &filepath, const std::string &file_extension);
    std::string get_file_content_as_string();
    std::unordered_map<std::string, int> get_index();
    std::string get_filepath();
    double get_term_frequency(std::string term);
    bool contains_term(std::string term);
    static std::vector<std::string> clean_word(std::string &word);
    void print_index();
    void index_document();

    /* reads only the actual content, is implemented in the derived classes */
    virtual std::string read_content();

  protected:
    std::string filepath; 
    std::string file_extension;
    std::chrono::time_point<std::chrono::system_clock> indexed_at;
    
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
