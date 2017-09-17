//
// Created by Martin Riedel on 17.09.17.
//

#include "HTSPConnection.h"

HTSPConnection::HTSPConnection(boost::asio::io_service &io_service, tcp::resolver::iterator endpoint_iterator)
        : io_service_(io_service),
          socket_(io_service) {

}

