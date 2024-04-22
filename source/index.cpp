#include "index.h"

/* 
*   The directory is the directory which is read and indexed, the index_path is used to save the index as a json,
*/
Index::Index(std::string directory, std::string index_path, int threads_used):
  index_path(index_path),
  thread_num(threads_used) 
{
  if (index_exists_on_filesystem()) {
    std::cout << "Existing index found in " << index_path << ", retrieving index." << std::endl;
    try {
      retrieve_index_from_filesystem();
      return;
    } catch (std::exception &e) {
      std::cerr << "Exception caught, retrieving index: " << e.what() << std::endl;
    }
  }

  /* start building the index, is run if retrieving the index fails */
  try {
    read_stopwords("stopwords.txt");
    build_document_index(directory);
    build_tfidf_index();
    save_index_to_filesystem();
  } catch (std::exception &e) {
    std::cerr << "Caught Exception building index: " << e.what() << std::endl;
  }
}

Index::~Index() {}

/*  
*   queries the index and returns the result ordered by tfidf ranking
*   returns a sorted vector of pairs <filepath, rank>
*/
std::vector<std::pair<std::string, double>> Index::retrieve_result(const std::vector<std::string> &input_values) {
  std::vector<std::pair<std::string, double>> result;
  /* loop over every document in the index */
  for (auto &document: documents) {
    double rank = 0.0;
    for (auto &input : input_values) {
      try {
        rank = document->get_tfidf_score(input);
      } catch (std::exception &e) {
        std::cerr << "Error getting tfidf rank of term: " << input << " in Document: " 
          << document->get_filepath() << " " << e.what() << std::endl;
      }
    }

	if (rank == 0.0) {
		continue;
	}

    result.push_back(std::make_pair(document->get_filepath(), rank));
  }
  
  /* Sort the result ascending by rank */
  std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
      return a.second > b.second;
  });

  return result;
}

/*
* returns the number of documents in the index
*/
int Index::get_document_counter() {
  return documents.size();
}

/* 
*   Moves trough a directy and try's to read every supported file in it
*   For every supported file in the dir, a Document is created and read 
*/
void Index::build_document_index(std::string directory) {
  /* if the param is a directory */ 
  if (std::filesystem::status(directory).type() == std::filesystem::file_type::directory) {
    std::cout << "Building index of directory: " << directory << std::endl;
    if (std::filesystem::exists(directory)){
      /* go over every file in the directory and create a Document Object for the file */
      for (auto const &entry: std::filesystem::recursive_directory_iterator(directory)) {
        std::string filepath = entry.path();
        std::string file_extension = std::filesystem::path(entry.path()).extension();
        /* on creation the document get indexed (clean words in the doc and there occurance counter) */
        try {
          std::unique_ptr<Document> new_doc = Document_factory::create_document(filepath, file_extension);
          documents.push_back(std::move(new_doc));
        } catch(std::exception &e) {
          std::cerr << "Exception caught reading file: " << e.what() << std::endl;
        }
      }
    }
  } else {
    std::cerr << "No directoy given to index" << std::endl;
    throw std::runtime_error("Directory to index not found: " + directory);
  }
}

/*
 * Uses threads to read every file in the document index
 * and calculate its tfidf score per every word in the doucments
 */
void Index::build_tfidf_index() {
  std::cout << "Running build tfidf index" << std::endl;
  const auto start{std::chrono::steady_clock::now()};

  // Doenst work on low Document count? Or if Threads > Document count
  int files_per_thread = get_document_counter() / thread_num;
  for (int i = 0; i < thread_num; ++i) {
    int start_index = i * files_per_thread;
    int end_index = (i == thread_num) ? get_document_counter() : (i + 1) * files_per_thread;
    std::cout << "Start index: " << start_index << " End index: " << end_index << std::endl;
    threads.emplace_back([this, start_index, end_index](){ this->calculate_tfidf_index(start_index, end_index); });
  }

  for(std::thread &thread: threads) {
    thread.join();
  }

  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << "Building tfidf index took: " << elapsed_seconds.count() << "seconds" << std::endl;
}

/* 
 * calculates the tfidf for every word of every document in the 
 * document index, saves the calculated values per word in a hashmap, 
 * the values are accessed by filepath,
 * needs the start and end index because of threads
 * the tfidf index itself is protected by a mutex
 * To be run the build document index has to be complete
 */
void Index::calculate_tfidf_index(int start_index, int end_index){
  for (int i = start_index; i < end_index; ++i) {
    std::cout << "Calculating tfidf for doc: " << documents.at(i)->get_filepath() << std::endl;
    for(auto &term: documents.at(i)->get_concordance()) {
      /* skip stop words */
      if (std::find(stopwords.begin(), stopwords.end(), term.first) != stopwords.end()) {
        continue;
      }
      /* caclulating the tfidf */
      double tfidf = documents.at(i)->get_term_frequency(term.first) * inverse_doc_frequency(term.first, documents);
      documents.at(i)->insert_tfidf_score({term.first, tfidf});
    }
  }
}

/*
 * Calculates the idf for a certain term over the whole index 
 */
double Index::inverse_doc_frequency(std::string term, const std::vector<std::unique_ptr<Document>> &corpus) {
  int term_count = 0;
  int n = corpus.size();
  
  for (auto &doc: corpus) {
    if (doc->contains_term(term)) {
      term_count++;
    }
  }
  
  if (term_count == 0) {
    return 0.0;
  }
  
  return std::log10((double)n / (double)term_count);
}

/*
 * Used to be run in specific intervall, to rebuild the index
 * checks the Document index and reindexes changed documents,
 * if a documents changed, the index also has to be rebuild
 */
void Index::rebuild_index() {
  bool reindex_tfidf = false;
  for (auto &document: documents) {
    if (document->needs_reindexing()) {
      document->index_document();
      reindex_tfidf = true;
    }
  }
  
  if (reindex_tfidf) {
    build_tfidf_index(); 
  }

  save_index_to_filesystem();
}

/* 
*   read stopwords from a txt file 
*/
void Index::read_stopwords(const std::string &filepath) {
  try {
    Document stop_words{filepath, ""};
    std::stringstream iss(stop_words.get_file_content_as_string());
    std::string word;
    while (iss >> word) {
      stopwords.push_back(word);
    }
  } catch (std::exception &e) {
    std::cerr << "Exception ocurred reading stop words: " << e.what() << std::endl;
    stopwords.clear();
    return;
  }
}

/* 
 * saves the index as json to index_path
 * the tfidf index is saved as index_tfidf.json
 * the docs index is saved a index_docs.json
 */
void Index::save_index_to_filesystem() {
  std::cout << "Index is saved in " << index_path << std::endl;
  json j_documents;

  for (auto &doc: documents) {
    json json_object = doc->serialize_to_json();
    j_documents.push_back(json_object);
  }

  std::ofstream doc_file(index_path + "/index_docs.json");

  try {
    doc_file << j_documents.dump();
  } catch(std::exception &e) {
    std::cout << "Exception caught: " << e.what() << std::endl;
  }
}

/* 
*   retrieves the index back from the filesystem, throws an exception on failure
*/
void Index::retrieve_index_from_filesystem() {
  std::ifstream input(index_path + "/index_docs.json");
  json load;

  try {
    input >> load;
    for (const auto &doc_json: load) {
      documents.push_back(std::make_unique<Document>(Document::from_json(doc_json)));
    }
  } catch(std::exception &e) {
    std::cout << "Exception caught: " << e.what() << std::endl;
    throw std::runtime_error(e.what());
  }
}

/* 
*  check wether the index exists in the filesystem
*/
bool Index::index_exists_on_filesystem() {
  const std::string file_path_docs = index_path + "/index_docs.json";

  return std::filesystem::exists(file_path_docs) && std::filesystem::is_regular_file(file_path_docs);
}