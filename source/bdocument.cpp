#include "bdocument.h"
#include "pdf_document.h"

/* Base Document Class */
BDocument::BDocument(
  std::string filepath, std::string file_extension):
  filepath(filepath), file_extension(file_extension)
{
}

BDocument::~BDocument() {}

std::unordered_map<std::string, int> BDocument::get_concordance() {
  return concordance;
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


std::string BDocument::get_filepath() {
  return filepath;
}

std::string BDocument::get_extension() {
  return file_extension;
}

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
    for (auto &clean_word: clean_words) {
      if (clean_word.find_first_not_of(' ') != std::string::npos) {
        if (concordance.find(clean_word) != concordance.end()){
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
        [](auto c){
            return std::tolower(c);
        }
    );

    /* remove special characters */
    std::replace_if(word.begin(), word.end(), 
        [](auto c) {return std::ispunct(c) || std::isdigit(c);},
    ' ');

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
    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(this->get_filepath());
    std::chrono::time_point tp = std::chrono::file_clock::to_sys(ftime);
    return tp > this->indexed_at;
  } catch (std::exception &e) {
    std::cerr << "Error ocurred: " << e.what() << std::endl;
    return false;
  }
}

/* Document Factory implementation */
std::unique_ptr<BDocument> BDocument_factory::create_document(const std::string &filepath, const std::string &extension) {
  /* simple factory implementation */
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
      std::cerr << "Exception caught creating pdf document: " << e.what() << std::endl;
    }
  }
  
  throw std::runtime_error(std::string("Document " + filepath + " " + extension + " not supported"));
};

/* XML Specific Documents */
XML_Document::XML_Document(const std::string &filepath):
  BDocument(filepath, "xml") 
{
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

/* Text Document stuff */
Text_Document::Text_Document(const std::string &filepath):
	BDocument(filepath, "txt")
{
}

std::string Text_Document::read_content() {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    	throw std::runtime_error("Failed to open file: " + filepath);
  }

  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return content;
}