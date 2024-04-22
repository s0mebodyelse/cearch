#include <iostream>
#include <exception>

/* boost headers */
#include <boost/asio.hpp>

/* cearch headers */
#include "index.h"
#include "document.h"
#include "server.h"

class Timer {
  public:
    Timer(int tick, boost::asio::io_context &io_context):
        interval(boost::posix_time::seconds(tick)),
        t(boost::asio::deadline_timer(io_context, interval))
    {
      
    };

  private:
    boost::posix_time::seconds interval;
    boost::asio::deadline_timer t;
};

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

    try {
      /* create io context */
      boost::asio::io_context io_context;

      /* init the index */
      Index idx(directory, index_path, threads);

      std::cout << "Starting cearch server on port: " << port << std::endl;
      Server server(io_context, port, idx);
      io_context.run();
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return 1;
    }

    return 0;
}