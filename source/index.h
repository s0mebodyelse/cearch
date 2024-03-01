#ifndef _H_INDEX
#define _H_INDEX

#include <iostream>
#include <stdexcept>
#include <mutex>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <exception>
#include <thread>

#include <nlohmann/json.hpp>

#include "document.h"

using json = nlohmann::json;

class Index {
public:
  Index(std::string directory, std::string index_path, int thread_num);
  ~Index();
  /* calculates a ranking from tfidf index and returns the documents with the highest rank, based on the input */
  std::vector<std::pair<std::string, double>> retrieve_result(const std::vector<std::string> &input_values);
  void print_documents_in_index();
  void print_tfidf_index_documents();
  void dump_tfidf_index_to_file(std::string filepath);
  int get_document_counter();
  int get_indexed_token_count();

private:
  /* vector of all Documents in the index */
  std::vector<std::unique_ptr<Document>> documents_per_path;
  /* Holds the path to a document and its tdidf score per term */
  std::unordered_map<std::string, std::unordered_map<std::string, double>> tfidf_index;
  /* holds the path to the index on the filesystem */
  std::string index_path;

  int thread_num;
  std::mutex mtx;
  std::vector<std::thread> threads;
  
  void build_document_index(std::string directory);
  void build_tfidf_index();
  void rebuild_index();

  /* calculates the inverse_doc_frequency of a term on the whole corpus */
  double inverse_doc_frequency(std::string term, const std::vector<std::unique_ptr<Document>> &corpus);
  void calculate_tfidf_index(int start_index, int end_index);
  void save_index_to_filesystem();
  void retrieve_index_from_filesystem();
  bool index_exists_on_filesystem();
};

#endif
