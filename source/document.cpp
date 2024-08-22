#include "document.h"

/* Base Document Class */
Document::Document(
  std::string filepath, std::string file_extension):
  filepath(filepath), file_extension(file_extension)
{
  	index_document();
  	indexed_at = std::chrono::system_clock::now();
}

/* constructor the be used for reading from json file */
Document::Document(
  std::string filepath, 
  std::string file_extension, 
  std::chrono::system_clock::time_point indexed_at,
  std::unordered_map<std::string, int> concordance,
  std::unordered_map<std::string, double> tfidf_scores
):
  filepath(std::move(filepath)), file_extension(std::move(file_extension)),
  indexed_at(indexed_at), concordance(std::move(concordance)),
  tfidf_scores(std::move(tfidf_scores)) 
{
  
}

Document::~Document() {}

std::string Document::read_content() {
  std::ifstream file(filepath);
  if (!file.is_open()) {
      throw std::runtime_error("Failed to open file: " + filepath);
  }

  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return content;
}

std::unordered_map<std::string, int> Document::get_concordance() {
  return concordance;
}

void Document::insert_tfidf_score(std::pair<std::string, double> tfidf_score) {
  tfidf_scores.insert(tfidf_score);
}

/* 
* 	returns the tfidf score of the term 
* 	returns 0 if the term is not in the Document
*/
double Document::get_tfidf_score(const std::string &term) {
  if (tfidf_scores.contains(term)) {
    return tfidf_scores.at(term);
  } 

  return 0.0;
}


std::string Document::get_filepath() {
  return filepath;
}

std::string Document::get_extension() {
  return file_extension;
}

/* number of times, a word occurs in a given document */
int Document::get_term_frequency(const std::string &term) {
  if (concordance.find(term) != concordance.end()) {
    return concordance.at(term);
  } 

  return 0;
}

std::string Document::get_file_content_as_string() {
  std::string file_content = read_content();
  return file_content;
}

bool Document::contains_term(const std::string &term) {
  if (concordance.find(term) != concordance.end()) {
    return true;
  }
  return false;
}

void Document::index_document() {
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
}

std::vector<std::string> Document::clean_word(std::string &word) {
    std::vector<std::string> clean_words;
    /* transform tolower */
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
        clean_words.push_back(split_word);
    }

    return clean_words;
}

/* 
* checks indexed_at time against the last modification of the file 
* if the file was modified after it was indexed, it needs to be indexed again, 
* and the function returns true, otherwise the function returns false
*/
bool Document::needs_reindexing() {
  try {
    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(this->get_filepath());
    std::chrono::time_point tp = std::chrono::file_clock::to_sys(ftime);
    return tp > this->indexed_at;
  } catch (std::exception &e) {
    std::cerr << "Error ocurred: " << e.what() << std::endl;
    return false;
  }
}

/* 
*	Converts a Document into a json structure
*/
const json Document::serialize_to_json() {
  /* convert std::chrono indexed at to milliseconds since epoch */
  auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(indexed_at.time_since_epoch()).count();

  return {
    {"filepath", this->get_filepath()},
    {"extension", this->get_extension()},
    {"indexedat", ms_since_epoch},
    {"index", concordance},
    {"tfidf_scores", tfidf_scores}
  };
}

Document Document::from_json(const json &j) {
  return Document(
    j.at("filepath").get<std::string>(),
    j.at("extension").get<std::string>(),
    std::chrono::system_clock::from_time_t(j.at("indexedat")),
    j.at("index").get<std::unordered_map<std::string, int>>(),
    j.at("tfidf_scores").get<std::unordered_map<std::string, double>>()
  );
}

/* Document Factory implementation */
std::unique_ptr<Document> Document_factory::create_document(const std::string &filepath, const std::string &extension) {
  /* simple factory implementation */
  if (extension == ".xml" || extension == ".xhtml") {
    return std::make_unique<XML_Document>(filepath);
    std::cout << "xml doc created" << std::endl;
  }

  if (extension == ".txt") {
    return std::make_unique<Text_Document>(filepath);
    std::cout << "txt doc created" << std::endl;
  }
  
  throw std::runtime_error(std::string("Document with extension ") + extension + " not supported");
};

/* XML Specific Documents */
XML_Document::XML_Document(const std::string &filepath):
  Document(filepath, "xml") 
{
}

XML_Document::~XML_Document() {

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
	Document(filepath, "txt")
{
}

Text_Document::~Text_Document() {

}

std::string Text_Document::read_content() {
  std::ifstream file(filepath);
  if (!file.is_open()) {
      throw std::runtime_error("Failed to open file: " + filepath);
  }

  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return content;
}


/* PDF Document Stuff */
PDF_Document::PDF_Document(const std::string &filepath):
  Document(filepath, "pdf") 
{
  ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
  if (!ctx) {
    throw std::runtime_error("Could not create MuPDF context");
  }

  /* open Document */
  doc = fz_open_document(ctx, filepath.c_str());
  if (!doc) {
    fz_drop_context(ctx);
    throw std::runtime_error("Could not open PDF Document");
  }
}

PDF_Document::~PDF_Document() {
  if (doc) {
    fz_drop_document(ctx, doc);
  }

  if (ctx) {
    fz_drop_context(ctx);
  }
}

std::string PDF_Document::read_content() {
  std::string content;

  return content;
}