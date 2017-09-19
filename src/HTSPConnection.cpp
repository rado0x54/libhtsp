//
// Created by Martin Riedel on 17.09.17.
//

#include "HTSPConnection.h"
#include <boost/log/trivial.hpp>
#include <boost/bind.hpp>

using namespace boost::system;

HTSPConnection::HTSPConnection(boost::asio::io_service &io_service)
        : io_service_(io_service),
          io_service_work_(io_service),
          resolver_(io_service),
          tcp_socket_(io_service) {

}

HTSPConnection::HTSPConnection(boost::asio::io_service &io_service, const std::string &server,
                               const std::string &service)
        : HTSPConnection(io_service) {

    // Start an asynchronous resolve to translate the server and service names
    // into a list of endpoints.
    Connect(server, service);


//    tcp_socket_.async_connect(*endpoint_iterator, std::bind(&HTSPConnection::connect_handler, this));
}

HTSPConnection::~HTSPConnection() {

}

void HTSPConnection::ResolveHandler(const boost::system::error_code &err,
                                    tcp::resolver::iterator endpoint_iterator) {
    if (!err) {
        BOOST_LOG_TRIVIAL(info) << "successfully resolved service";
        Connect(endpoint_iterator);
    } else {
        BOOST_LOG_TRIVIAL(error) << "Error while resolving endpoint: " << err.message();
    }
}

void HTSPConnection::ConnectHandler(const boost::system::error_code &err,
                                    tcp::resolver::iterator endpoint_iterator,
                                    std::shared_ptr<std::promise<bool>> connectPromisePtr) {
    if (!err) {
        BOOST_LOG_TRIVIAL(info) << "successfully connected to service";
        connectPromisePtr->set_value(true);
        return;
    }

    // Error case, retry another endpoint or fail.

    if (endpoint_iterator != tcp::resolver::iterator()) {
        // The connection failed. Try the next endpoint in the list.
        tcp_socket_.close();
        tcp::endpoint endpoint = *endpoint_iterator;
        tcp_socket_.async_connect(endpoint,
                                  std::bind(&HTSPConnection::ConnectHandler, this,
                                            std::placeholders::_1, // boost::system::error_code
                                            ++endpoint_iterator,
                                            connectPromisePtr
                                  ));
    } else {
        BOOST_LOG_TRIVIAL(error) << "Error: while connecting to service: " << err.message();
        connectPromisePtr->set_value(false);
    }

}


void HTSPConnection::Connect(const std::string &server, const std::string &service) {
    tcp::resolver::query query(server, service);

    resolver_.async_resolve(query,
                            std::bind(&HTSPConnection::ResolveHandler, this,
                                      std::placeholders::_1, // boost::system::error_code
                                      std::placeholders::_2 //tcp::resolver::iterator
                            ));
}

std::future<bool> HTSPConnection::Connect(tcp::resolver::iterator endpoint_iterator) {

    // Bind does not work with rvalue, and std::promise is only moveable,
    // so allocate a shared pointer.
    auto connectPromisePtr = std::make_shared<std::promise<bool>>();
    std::future<bool> connectFuture = connectPromisePtr->get_future();

    // Call Connecthandler directory to iterate through endpoints
    auto ec = errc::make_error_code(errc::destination_address_required);
    ConnectHandler(ec, endpoint_iterator, connectPromisePtr);

    return connectFuture;
}


bool HTSPConnection::IsOpen() {
    return tcp_socket_.is_open();
}

bool HTSPConnection::SendAuth
        (const std::string &user, const std::string &pass) {

    return false;
}

bool HTSPConnection::SendHello() {

    return false;
}

bool HTSPConnection::SendHtsData(const HtsData &data) {


    return false;
}

