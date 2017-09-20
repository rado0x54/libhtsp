//
// Created by Martin Riedel on 18.09.17.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "../src/HTSPConnection.h"

int main(int argc, char* argv[])
{

    if (argc != 3)
    {
        BOOST_LOG_TRIVIAL(error) << "Usage: client <ip or hostname> <service or port>";
        return 1;
    }

    BOOST_LOG_TRIVIAL(info) << "Starting Test HTSP Client!";

    boost::asio::io_service io_service;

    auto conn = new HTSPConnection(io_service);

    std::thread t([&io_service](){ io_service.run(); });

    auto connFuture = conn->Connect(argv[1], argv[2]);

    // Wait until done.
    connFuture.get();

    conn->SendHello();

    sleep(2);


    BOOST_LOG_TRIVIAL(info) << "Running io_service in separate thread...";

    // delete connection (should return io_service.run())
    delete conn;

    t.join();

    BOOST_LOG_TRIVIAL(info) << "Closing Application!";

    return 0;
}

