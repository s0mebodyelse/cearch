#include <iostream>
#include <exception>
#include <fstream>
#include <exception>
#include <sstream>
#include <filesystem>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <regex>
#include <cmath>

/* HTTP Stuff*/
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>

/* custom index headers */
#include "index.h"
#include "document.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = net::ip::tcp;               // from <boost/asio/ip/tcp.hpp>


std::string read_html_file(const std::string& file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open HTML file: " + file_path);
    }

    std::string html_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return html_content;
}


int main(int argc, const char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: ./cearch <Port> <Directory to index> <directory to save index in>";
        return 1;
    }
    
    int port = atoi(argv[1]);
    std::string directory = argv[2];
    std::string index_path = argv[3];

    /* init the index */
    Index idx(directory, index_path, 4);

    /* run a http server */    
    try {
      std::cout << "Starting http server" << std::endl;
      net::io_context io_context;
      tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

      while(true) {
          tcp::socket socket(io_context);
          acceptor.accept(socket);

          /* Read the HTTP request */
          beast::flat_buffer buffer;
          http::request<http::dynamic_body> request;
          http::response<http::dynamic_body> response;
          http::read(socket, buffer, request);
          
          /* prepare respnse */
          response.version(request.version());
          response.result(http::status::ok);
          response.set(http::field::server, "Boost Beast");
          response.set(http::field::content_type, "text/html");
          std::string html_body = read_html_file("web/index.html");

          /* handle post request, aka. calculate the tfidf and display a result */
          std::vector<std::string> input_values;
          if (request.method() == http::verb::post) {
              std::string input_value;
              if (request.body().size() > 0) {
                  /* parse the input field, assumong its name is "input-text" */
                  std::string request_body = beast::buffers_to_string(request.body().data());
                  std::cout << "Body: " << request_body << std::endl;
                  size_t start_pos = request_body.find("input-text=");
                  if (start_pos != std::string::npos) {
                      /* move past the name to get the value */
                      start_pos += std::strlen("input-text=");
                      size_t end_pos = request_body.find("&", start_pos);
                      input_value = request_body.substr(start_pos, end_pos - start_pos);
                  }
              }

              /* extract every single word from input value */
              input_values = Document::clean_word(input_value);
              
              /* retrieve the result from the index */
              std::vector<std::pair<std::string, double>> result;
              if (!input_values.empty()) {
                result = idx.retrieve_result(input_values);
              }

              /* insert result into index.html */      
              size_t pos = html_body.find("<table>") + strlen("<table>");
              if (pos != std::string::npos) {
                std::ostringstream oss;
                for (auto &i: result) {
                  oss << "<tr><td>" << i.first << " => " << i.second << "</td></tr>";
                }
                std::string result_table = oss.str();
                html_body.insert(pos, result_table);
              } else {
                std::cerr << "Couldnt find table in html body, no results shown";
              }
            }              

          /* send the response with the result */
          beast::ostream(response.body()) << html_body;
          response.prepare_payload();
          http::write(socket, response);

          /* Gracefully close the socket */
          socket.shutdown(tcp::socket::shutdown_send);
      }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
