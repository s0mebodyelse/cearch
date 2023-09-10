#include "index.h"
#include <fstream>

Index::Index(std::string directory, std::string index_path, int threads):
  index_path(index_path), threads(threads)
{
  /* TODO Check if there is an index in the index directory, then retrive the index */
  const auto start{std::chrono::steady_clock::now()};
  build_index(directory);
  calculate_tfidf_index();
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << "Building index took: " << elapsed_seconds.count() << "s" << std::endl;
  std::cout << "Indexed Documents: " << get_document_counter() << std::endl;
  std::cout << "Indexed Tokens: " << get_indexed_token_count() << std::endl;
  
  std::cout << "saving index to filesystem" << std::endl;
  save_index_to_filesystem();
}

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
    std::cout << i.first << " : " << i.second->get_filepath() << std::endl;
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
int Index::build_index(std::string directory) {
  /* if the param is a directory */ 
  if (std::filesystem::status(directory).type() == std::filesystem::file_type::directory) {
    std::cout << "Building index of directory: " << directory << std::endl;
    if (std::filesystem::exists(directory)){
      /* go over every file in the directory and create a Document Object for the file */
      for (auto const &entry: std::filesystem::recursive_directory_iterator(directory)) {
        std::string filepath = entry.path();
        std::string file_extension = std::filesystem::path(entry.path()).extension();
        /* on creation the document get indexed (clean words in the doc and there occurance counter) */
        std::unique_ptr<Document> new_doc = Document_factory::create_document(filepath, file_extension);

        /* documents are then moved to the Index Object and can be accessed by their path */
        documents_per_path[new_doc->get_filepath()] = std::move(new_doc);
      }
    }
    return 0;
  } else {
    /* Use Exceptions instead of int return value? */
    std::cerr << "No directoy given to index" << std::endl;
    return -1;
  }
}

/* calculates the tfidf for every document in the index */
void Index::calculate_tfidf_index(){
  std::cout << "Calculating tfidf index" << std::endl;
  /* use threads? */
  for (auto &doc: documents_per_path) {
    std::unordered_map<std::string, double> tfidf_entries;
    for(auto &term: doc.second->get_index()) {
      double tfidf = doc.second->get_term_frequency(term.first) * inverse_doc_frequency(term.first, documents_per_path);
      if (tfidf > 0.0) {
        tfidf_entries.insert({term.first, tfidf});
      }
    }
    tfidf_index[doc.second->get_filepath()] = tfidf_entries; 
  }
}

double Index::inverse_doc_frequency(std::string term, const std::unordered_map<std::string, std::unique_ptr<Document>> &corpus) {
  /* number of documents where the term appears */
  double term_count = 0.0;
  /* total number of documents in the corpus */
  int n = corpus.size();
  
  /* count the occurance of a term in every document */
  for (auto &doc: corpus) {
    if (doc.second->contains_term(term)) {
      term_count++;
    }
  }
  
  if (term_count == 0.0) {
    return 0.0;
  }

  return std::log10((double)n / (double)term_count);
}

/* saves the index as json to index_path */
void Index::save_index_to_filesystem() {
  json j =  tfidf_index;
  std::ofstream file(index_path + "/index_data.json");
  file << j.dump();
  file.close();
}

/* retrieves the index back from the filesystem */
void Index::retrieve_index_from_filesystem() {
  std::ifstream input(index_path + "/index_data.json");
  json load;
  input >> load;
  input.close();
  
  tfidf_index = load.get<std::unordered_map<std::string, std::unordered_map<std::string, double>>>();
}



