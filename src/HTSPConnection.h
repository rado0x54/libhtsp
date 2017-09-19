//
// Created by Martin Riedel on 17.09.17.
//

#ifndef LIBHTSP_HTSPCONNECTION_H
#define LIBHTSP_HTSPCONNECTION_H

#include <thread>
#include <boost/asio.hpp>
#include <future>
#include "HTSMessage.h"


/*
 * Configuration defines
 */
#define HTSP_MIN_SERVER_VERSION       (19) // Server must support at least this htsp version
#define HTSP_CLIENT_VERSION           (29) // Client uses HTSP features up to this version. If the respective
// addon feature requires htsp features introduced after
// HTSP_MIN_SERVER_VERSION this feature will only be available if the
// actual server HTSP version matches (runtime htsp version check).


using boost::asio::ip::tcp;


class HTSPConnection {
private:
    boost::asio::io_service &io_service_;
    boost::asio::io_service::work io_service_work_;
    tcp::resolver resolver_;
    tcp::socket tcp_socket_;

    void ResolveHandler(const boost::system::error_code& err,
                        tcp::resolver::iterator endpoint_iterator);

    void ConnectHandler(const boost::system::error_code& err,
                        tcp::resolver::iterator endpoint_iterator,
                        std::shared_ptr<std::promise<bool>> connectPromise);


public:
    HTSPConnection(boost::asio::io_service &io_service);
    HTSPConnection(boost::asio::io_service &io_service,
                   const std::string &server,
                   const std::string &service);

    ~HTSPConnection();

    void Connect(const std::string& server, const std::string& service);
    std::future<bool> Connect(tcp::resolver::iterator endpoint_iterator);
    bool IsOpen();

    void connect_handler(const boost::system::error_code& error);

    bool SendHtsData(const HtsData &data);

    bool SendHello();

    bool SendAuth(const std::string &user, const std::string &pass);
};


#endif //LIBHTSP_HTSPCONNECTION_H


