#include "index.h"
#include <chrono>
#include <stdexcept>

/* 
 * The directory is the directory which is read and indexed, the index_path is used to save the index as a json
 * by using threads the speed if indexing a directory can be increased 
 */
Index::Index(std::string directory, std::string index_path, int threads_used):
  index_path(index_path), thread_num(threads_used)
{
  /* check first if there is already an existing index in the directory */
  if (index_exists_on_filesystem()) {
    std::cout << "Existing index found in " << index_path << ", retrieving index." << std::endl;
    try {
      retrieve_index_from_filesystem();
      return;
    } catch (std::exception &e) {
      std::cout << "Exception caught, retrieving index: " << e.what() << std::endl;
    }
  }

  /* start building the index, is run if retrieving the index fails */
  try {
    build_document_index(directory);
    build_tfidf_index();
    save_index_to_filesystem();
  } catch (std::exception &e) {
    std::cout << "Caught Exception building index: " << e.what() << std::endl;
  }
}

Index::~Index() {}

/* queries the index and returns the result ordered by tfidf ranking */
std::vector<std::pair<std::string, double>> Index::retrieve_result(const std::vector<std::string> &input_values) {
  std::vector<std::pair<std::string, double>> result;
  /* loop over every document in the index */
  for (auto &doc: tfidf_index) {
    double rank = 0.0;
    /* retrieve the tfidf of every term from the input values and add them up as a rank */
    for (auto &input : input_values) {
      try {
        if (doc.second.contains(input)) {
          rank += doc.second.at(input);
        }
      } catch (std::exception &e) {
        std::cerr << "Error getting tfidf rank of term: " << input << " in Document: " 
          << doc.first << " " << e.what() << std::endl;
      }
    }
    /* we take the document to the result, only if its rank is above 0.01 */
    if (rank > 0.01) {
        result.push_back(std::make_pair(doc.first, rank));
    }
  }
  
  /* Sort the result ascending by rank */
  std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
      return a.second > b.second;
  });

  return result;
}

void Index::print_documents_in_index() {
  for (auto &i: documents_per_path) {
    std::cout << i->get_filepath() << std::endl;
  }
}

void Index::print_tfidf_index_documents() {
  for (auto &i: tfidf_index) {
    std::cout << i.first << std::endl;
  }
}

void Index::dump_tfidf_index_to_file(std::string filepath) {
  std::ofstream dump_file; 
  dump_file.open(filepath);
  if (dump_file.is_open()) {
    for (auto &i: tfidf_index) {
      dump_file << i.first << "\n";
      for (auto &j : i.second) {
        dump_file << j.first << " : " << j.second << "\n";
      }
    }
  }
}

int Index::get_document_counter() {
  return documents_per_path.size();
}

int Index::get_indexed_token_count() {
  int counter = 0;
  for (auto &i: tfidf_index) {
    counter += i.second.size();  
  }

  return counter;
}

/* builds an index on a given directory */
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
          documents_per_path.push_back(std::move(new_doc));
        } catch(std::exception &e) {
          std::cerr << "Exception caught reading file: " << e.what() << std::endl;
        }
      }
    }
  } else {
    /* Use Exceptions instead of int return value? */
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
 * Used to be run in specific intervall, to rebuild the index
 * checks the Document index and reindexes changed documens,
 * if the documents change, the tfidf index also has to be rebuild
 */
void Index::rebuild_index() {
  bool reindex_tfidf = false;
  for (auto &i: documents_per_path) {
    if (i->needs_reindexing()) {
      i->index_document();
      reindex_tfidf = true;
    }
  }
  
  if (reindex_tfidf) {
    build_tfidf_index(); 
  }

  /* update the filesystem */
  save_index_to_filesystem();
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
  std::cout << "Calculating tfidf index" << std::endl;

  for (int i = start_index; i < end_index; ++i) {
    std::string file_path = documents_per_path[i]->get_filepath();
    std::unordered_map<std::string, double> tfidf_entries;

    for(auto &term: documents_per_path[i]->get_index()) {
      double tfidf = documents_per_path[i]->get_term_frequency(term.first) * inverse_doc_frequency(term.first, documents_per_path);

      if (tfidf > 0.0) {
        tfidf_entries.insert({term.first, tfidf});
      }
    }
    
    std::lock_guard<std::mutex> lock(mtx);
    tfidf_index[file_path] = tfidf_entries; 
  }
}

/*
 * Calculates the idf for a certain term over the whole index 
 */
double Index::inverse_doc_frequency(std::string term, const std::vector<std::unique_ptr<Document>> &corpus) {
  double term_count = 0.0;
  int n = corpus.size();
  
  for (auto &doc: corpus) {
    if (doc->contains_term(term)) {
      term_count++;
    }
  }
  
  if (term_count == 0.0) {
    return 0.0;
  }

  return std::log10((double)n / (double)term_count);
}

/* 
 * saves the index as json to index_path
 * the tfidf index is saved as index_tfidf.json
 * the docs index is saved a index_docs.json
 */
void Index::save_index_to_filesystem() {
  std::cout << "Index is saved in " << index_path << std::endl;
  json j_tfidf =  tfidf_index;
  json j_documents;

  /* serialize the document objects */
  for (auto &doc: documents_per_path) {
    json json_object = doc->serialize_to_json();
    j_documents.push_back(json_object);
  }

  std::ofstream tfidf_file(index_path + "/index_tfidf.json");
  std::ofstream doc_file(index_path + "/index_docs.json");

  try {
    tfidf_file << j_tfidf.dump();
    doc_file << j_documents.dump();
  } catch(std::exception &e) {
    std::cout << "Exception caught: " << e.what() << std::endl;
  }
}

/* retrieves the index back from the filesystem, throws an exception on failure */
void Index::retrieve_index_from_filesystem() {
  std::ifstream input(index_path + "/index_tfidf.json");
  json load;

  try {
    input >> load;
  } catch(std::exception &e) {
    std::cout << "Exception caught: " << e.what() << std::endl;
    throw std::runtime_error(e.what());
  }
  
  tfidf_index = load.get<std::unordered_map<std::string, std::unordered_map<std::string, double>>>();
}

bool Index::index_exists_on_filesystem() {
  const std::string file_path_docs = index_path + "/index_docs.json";
  const std::string file_path_tfidf = index_path + "/index_tfidf.json";

  return std::filesystem::exists(file_path_docs) && 
    std::filesystem::exists(file_path_tfidf) && 
    std::filesystem::is_regular_file(file_path_docs);
}
