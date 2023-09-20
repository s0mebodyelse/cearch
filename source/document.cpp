#include "document.h"
#include <memory>
#include <stdexcept>

/* Base Document Class */
Document::Document(const std::string &filepath, const std::string &file_extension):
  filepath(filepath), file_extension(file_extension)
{
  index_document();
  indexed_at = std::chrono::system_clock::now();
}

std::string Document::read_content() {
  std::ifstream file(filepath);
  if (!file.is_open()) {
      throw std::runtime_error("Failed to open file: " + filepath);
  }

  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return content;
}

std::string Document::get_file_content_as_string() {
  std::string file_content = read_content();
  return file_content;
}

std::unordered_map<std::string, int> Document::get_index() {
  return index;
}

std::string Document::get_filepath() {
  return filepath;
}

double Document::get_term_frequency(std::string term) {
  if (index.find(term) != index.end()) {
    return static_cast<double>(index.at(term)) / index.size();
  } 
  return 0.0;
}

bool Document::contains_term(std::string term) {
  if (index.find(term) != index.end()) {
    return true;
  }
  return false;
}

void Document::print_index() {
  std::cout << "Index of " << filepath << ":" << std::endl;
  for (auto &i: index) {
    std::cout << i.first << " : " << i.second << std::endl;
  }
}

void Document::index_document() {
  std::string word;
  std::istringstream iss(get_file_content_as_string());
  
  while (iss >> word) {
    /* split the word if necessary */
    std::vector<std::string> clean_words = clean_word(word);
    for (auto &clean_word: clean_words) {
      if (clean_word.length() < 4) {
        continue;
      }
      if (clean_word.find_first_not_of(' ') != std::string::npos) {
        if (index.find(clean_word) != index.end()){
          index[clean_word]++;
        } else {
          index[clean_word] = 1;
        }
      }
    }
  }
}

std::vector<std::string> Document::clean_word(std::string &word) {
    std::vector<std::string> clean_words;

    /* transform tolower */
    std::transform(word.begin(), word.end(), word.begin(),
        [](auto c){
            return std::tolower(c);
        }
    );

    /* remove special character */
    std::replace_if(word.begin(), word.end(), 
        [](auto c) {return std::ispunct(c) || std::isdigit(c);},
    ' ');

    /* split the word if neccessary and append to result vector */
    std::stringstream iss(word);
    std::string split_word;

    while (iss >> split_word) {
        clean_words.push_back(split_word);
    }

    return clean_words;
}

/* XML Specific Documents */
XML_Document::XML_Document(const std::string &filepath):
  Document(filepath, "xml") 
{
  index_document();
}

std::string XML_Document::read_content() {
  pugi::xml_document doc;
  std::string file_content;

  if (!doc.load_file(filepath.c_str())) {
      std::cerr << "failed to load xml file" << std::endl;
  }

  traverse_nodes(doc.document_element(), file_content);
  return file_content;
}

void XML_Document::traverse_nodes(const pugi::xml_node& root_node, std::string &content) {
  std::queue<pugi::xml_node> node_queue;
  node_queue.push(root_node);

  while (!node_queue.empty()) {
    pugi::xml_node current_node = node_queue.front();
    node_queue.pop();

    /* process the current node */
    content.append(current_node.value());
    content.append(" ");
    
    for (pugi::xml_node child_node = current_node.first_child(); child_node; child_node = child_node.next_sibling()) {
        node_queue.push(child_node);
    }
  }
}
/* Document Factory implementation */
std::unique_ptr<Document> Document_factory::create_document(const std::string &filepath, const std::string &extension) {
  if (extension == ".xml" || extension == ".xhtml") {
    return std::make_unique<XML_Document>(filepath);
  }
  
  throw std::runtime_error(std::string("Document with extension ") + extension + " not supported");
};
