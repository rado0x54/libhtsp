//
// Created by Martin Riedel on 18.09.17.
//

#include <iostream>
#include <boost/asio.hpp>
#include "../src/HTSPConnection.h"

boost::asio::io_service io_service;
boost::asio::ip::tcp::resolver resolv{io_service};

int main()
{

    std::cout << "Hello World!" << std::endl;

    tcp::resolver::query query("192.168.184.20", "9982");

    auto conn = new HTSPConnection(io_service, resolv.resolve(query));

    io_service.run();
}

