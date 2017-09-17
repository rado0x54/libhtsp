//
// Created by Martin Riedel on 17.09.17.
//

#ifndef LIBHTSP_HTSPCONNECTION_H
#define LIBHTSP_HTSPCONNECTION_H

#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class HTSPConnection {
private:
    boost::asio::io_service& io_service_;
    tcp::socket socket_;


public:
    HTSPConnection(boost::asio::io_service& io_service,
                   tcp::resolver::iterator endpoint_iterator);
    ~HTSPConnection();
};


#endif //LIBHTSP_HTSPCONNECTION_H


