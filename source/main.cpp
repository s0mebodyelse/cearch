#include <iostream>
#include <exception>

#include <boost/asio.hpp>

/* cearch headers */
#include "index.h"
#include "document.h"
#include "server.h"

int main(int argc, const char *argv[]) {
    /* read configuration from command line */
    if (argc != 5) {
      std::cerr << "Usage: ./cearch <Port> <Directory to index> <directory to save index in> <number of threads to use>";
      return 1;
    }
    
    int port = atoi(argv[1]);
    std::string directory = argv[2];
    std::string index_path = argv[3];
    int threads = atoi(argv[4]);

    /* init the index */
    Index idx(directory, index_path, threads);

    /* run a http server */    
    try {
      std::cout << "Starting cearch server" << std::endl;
      boost::asio::io_context io_context;
      Server server(io_context, port, idx);
      io_context.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}