#include <iostream>
#include <exception>
#include <chrono>

/* boost headers */
#include <boost/asio.hpp>

/* cearch headers */
#include "index.h"
#include "document.h"
#include "server.h"

class Timer {
	public:
		Timer(int tick, boost::asio::io_context &io_context) : 
			m_interval(std::chrono::seconds(tick)),
			m_timer(io_context, m_interval) 
			{
				start_timer();
			};

	private:
		std::chrono::seconds m_interval;
		boost::asio::steady_timer m_timer;

		void start_timer() {
			m_timer.expires_from_now(m_interval);
			m_timer.async_wait([this](const boost::system::error_code &error){
				this->async_handler(error);
			});
		}

		void async_handler(const boost::system::error_code &error) {
			if (!error) {
				std::cout << "Timer expired!" << std::endl;
				/* reschedule the timer */
				start_timer();
			} else {
				std::cout << "Error occured on timer: " << error.message() << std::endl;
			}
		}
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

		Timer timer(5, io_context);

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