//
// Created by Martin Riedel on 17.09.17.
//

#ifndef LIBHTSP_HTSPCONNECTION_H
#define LIBHTSP_HTSPCONNECTION_H

#include <thread>
#include <boost/asio.hpp>
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
    tcp::socket tcp_socket_;


public:
    HTSPConnection(boost::asio::io_service &io_service,
                   tcp::resolver::iterator endpoint_iterator);

    ~HTSPConnection();


    bool SendHtsData(const HtsData &data);

    bool SendHello();

    bool SendAuth(const std::string &user, const std::string &pass);
};


#endif //LIBHTSP_HTSPCONNECTION_H


