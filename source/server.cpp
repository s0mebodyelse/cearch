#include "server.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context &io_context, short port, Index &idx)
        :   endpoint(tcp::v4(), port),
            acceptor(io_context), 
            context(io_context),
            idx(idx)
{
    std::cout << port << std::endl;
    open_acceptor();            
}

void Server::open_acceptor(){
    boost::system::error_code errorCode;
    if (!acceptor.is_open()) {
        acceptor.open(endpoint.protocol());
        acceptor.set_option(boost::asio::socket_base::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen(boost::asio::socket_base::max_listen_connections, errorCode);
        if (!errorCode) {
            do_accept();
        }
    }
}

void Server::do_accept() { 
    acceptor.async_accept(
        [this](boost::system::error_code errorCode, tcp::socket socket) {
            if (!errorCode) {
                std::make_shared<Session>(std::move(socket), context, idx) -> start();
                do_accept();
            }
        }
    );
}