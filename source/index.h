#ifndef _H_INDEX
#define _H_INDEX

#include <algorithm>
#include <boost/asio.hpp>
#include <chrono>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>

#include "bdocument.h"

class Index {
   public:
    Index(std::string directory, std::string index_path, int thread_num);
    ~Index();

    /* 
    *   calculates a ranking from tfidf index and returns the documents with the
    *   highest rank, based on the input 
    */
    std::vector<std::pair<std::string, double>> query_index(
        const std::vector<std::string> &input_values
    );

    int get_document_counter();
    void run_reindexing();
    void print_tfidf_index();

   private:
    /* vector of all Documents in the index */
    std::vector<std::unique_ptr<BDocument>> documents;

    /* holds the path to the index on the filesystem */
    std::string index_path;

    /* stopwords which are read from a txt file */
    std::vector<std::string> stopwords;

    /* threading */
    int thread_num;
    std::mutex mtx;
    std::vector<std::thread> threads;

    void build_document_index(std::string directory);
    void build_tfidf_index();
    void rebuild_index();
    void read_stopwords(const std::string &filepath);

    /* calculates the inverse_doc_frequency of a term over the whole corpus */
    double inverse_doc_frequency(
        std::string term,
        const std::vector<std::unique_ptr<BDocument>> &corpus
    );

    void calculate_tfidf_index(int start_index, int end_index);
};

#endif
