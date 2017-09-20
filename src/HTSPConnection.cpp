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
    tcp_socket_.close();
}

//void HTSPConnection::ResolveHandler(const boost::system::error_code &err,
//                                    tcp::resolver::iterator endpoint_iterator,
//                                    std::shared_ptr<std::promise<bool>> connectPromisePtr) {
//    if (!err) {
//        BOOST_LOG_TRIVIAL(info) << "successfully resolved service";
//
//        auto connectFuture = Connect(endpoint_iterator);
////        // blocking?
//        connectPromisePtr->set_value(connectFuture);
//
//    } else {
//        BOOST_LOG_TRIVIAL(error) << "Error while resolving endpoint: " << err.message();
//        connectPromisePtr->set_value(false);
//    }
//}

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
        BOOST_LOG_TRIVIAL(error) << "Error while connecting to service: " << err.message();
        connectPromisePtr->set_value(false);
    }

}

void HTSPConnection::WriteHandler(const boost::system::error_code &err) {
    if (!err)
    {
        // Read the response status line. The response_ streambuf will
        // automatically grow to accommodate the entire line. The growth may be
        // limited by passing a maximum size to the streambuf constructor.
        BOOST_LOG_TRIVIAL(info) << "successfully wrote to service";
        uint32_t htsmsgsize;
        size_t lenhead = read(tcp_socket_, boost::asio::buffer(&htsmsgsize, sizeof(uint32_t)));
        htsmsgsize = ntohl(htsmsgsize);
        BOOST_LOG_TRIVIAL(info) << "Read Message with Size: " << htsmsgsize;
        BOOST_LOG_TRIVIAL(info) << "Size uint32_t: " << sizeof(uint32_t) << " Size read: " << lenhead;


        char * buf = (char*)malloc(htsmsgsize);

        size_t lenbody = read(tcp_socket_, boost::asio::buffer(buf, htsmsgsize));
//        BOOST_LOG_TRIVIAL(info) << "Read Message with Size: " << htsmsgsize;
        BOOST_LOG_TRIVIAL(info) << "Read #bytes; " << lenbody;


        HtsMessage m = HtsMessage::Deserialize(htsmsgsize, buf);
        BOOST_LOG_TRIVIAL(info) << "Got Valid Message? " << m.isValid();


        uint32_t chall_len;
        void * chall;
        m.getRoot()->getBin("challenge", &chall_len, &chall);

        std::string serverName = m.getRoot()->getStr("servername");
        std::string serverVersion = m.getRoot()->getStr("serverversion");
        uint32_t protoVersion = m.getRoot()->getU32("htspversion");

        BOOST_LOG_TRIVIAL(info) << "Connected to HTSP Server " << serverName << ", version " << serverVersion << ", protocol " << protoVersion;


        free(buf);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Error while writing to socket: " << err.message();
    }
}


std::future<bool> HTSPConnection::Connect(const std::string &server, const std::string &service) {
    BOOST_LOG_TRIVIAL(trace) << "calling Connect(const std::string &server, const std::string &service)";

    tcp::resolver::query query(server, service);

    tcp::resolver::iterator endpoint_iterator = resolver_.resolve(query);

    return Connect(endpoint_iterator);
}

std::future<bool> HTSPConnection::Connect(tcp::resolver::iterator endpoint_iterator) {
    BOOST_LOG_TRIVIAL(trace) << "calling Connect(tcp::resolver::iterator endpoint_iterator))";

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

    HtsMap map;
    map.setData("method", "hello");
    map.setData("clientname", "libhtsp");
    map.setData("htspversion", HTSP_CLIENT_VERSION);

    WriteHtsMessage(map.makeMsg());

//    HtsMessage m = ReadResult(sd, sys, map.makeMsg());
//    if(!m.isValid())
//    {
//        msg_Err(sd, "No valid hello response");
//        return false;
//    }

//    uint32_t chall_len;
//    void *chall;
//    m.getRoot()->getBin("challenge", &chall_len, &chall);
//
//    std::string serverName = m.getRoot()->getStr("servername");
//    std::string serverVersion = m.getRoot()->getStr("serverversion");
//    uint32_t protoVersion = m.getRoot()->getU32("htspversion");


    return false;
}

bool HTSPConnection::WriteHtsMessage(const HtsMessage &msg) {

    // TODO: We leak intentionally here.
    void *buf;
    uint32_t len;

    if (!msg.Serialize(&len, &buf)) {

        return false;
    }



    boost::asio::async_write(tcp_socket_, boost::asio::buffer(buf, len), std::bind(&HTSPConnection::WriteHandler, this,
                                                     std::placeholders::_1));


    return false;
}

