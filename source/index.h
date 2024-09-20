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
#include <chrono>
#include <stdexcept>
#include <boost/asio.hpp>

#include "bdocument.h"

class Index {
  public:
    Index(std::string directory, std::string index_path, int thread_num);
    ~Index();
    /* calculates a ranking from tfidf index and returns the documents with the highest rank, based on the input */
    std::vector<std::pair<std::string, double>> retrieve_result(const std::vector<std::string> &input_values);

    /* helper functions */
    int get_document_counter();

    void run_reindexing();

  private:
    /* vector of all Documents in the index */
    std::vector<std::unique_ptr<BDocument>> documents;

    /* Holds the path to a document and its tfidf score per term */
    std::unordered_map<std::string, std::unordered_map<std::string, double>> tfidf_index;

    /* holds the path to the index on the filesystem */
    std::string index_path;

    /* stopwords which are read from a txt file */
    std::vector<std::string> stopwords;

    int thread_num;
    std::mutex mtx;
    std::vector<std::thread> threads;
    
    void build_document_index(std::string directory);
    void build_tfidf_index();
    void rebuild_index();
    void read_stopwords(const std::string &filepath);

    /* calculates the inverse_doc_frequency of a term over the whole corpus */
    double inverse_doc_frequency(std::string term, const std::vector<std::unique_ptr<BDocument>> &corpus);

    void calculate_tfidf_index(int start_index, int end_index);
};

#endif
