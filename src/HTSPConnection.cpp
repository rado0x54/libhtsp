//
// Created by Martin Riedel on 17.09.17.
//

#include "HTSPConnection.h"
#include <boost/log/trivial.hpp>

HTSPConnection::HTSPConnection(boost::asio::io_service &io_service, tcp::resolver::iterator endpoint_iterator)
        : io_service_(io_service),
          tcp_socket_(io_service) {


}

HTSPConnection::~HTSPConnection() {

}

bool HTSPConnection::SendAuth
        ( const std::string &user, const std::string &pass )
{

}

bool HTSPConnection::SendHello()
{

}

bool HTSPConnection::SendHtsData(const HtsData &data) {



    return true;
}

