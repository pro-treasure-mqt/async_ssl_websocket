#pragma once

#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <string>
#include <thread>
#include "root_certificates.h"
#include <mutex>
#include "timestamp.h"

class WSSession : public std::enable_shared_from_this<WSSession>
{
    typedef std::function<void()> on_connected_cb_t;

    typedef std::function<void()> on_disconnected_cb_t;

    typedef std::function<void (const std::string&)> on_message_cb_t;

    typedef std::function<void (const int& , const std::string&)> on_error_cb_t;

public:

    explicit WSSession();

    int connect(char const* host, char const* port, char const* endpoint);

    int async_send(const std::string& message);

    void reg_on_connected_cb(const on_connected_cb_t& on_connected_cb);

    void reg_on_disconnected_cb(const on_disconnected_cb_t& on_disconnected_cb);

    void reg_on_message_cb(const on_message_cb_t& on_message_cb);

    void reg_on_error_cb(const on_error_cb_t& on_error_cb);

private:

    std::mutex mtx_;

    bool running_;

    bool connected_;

    boost::asio::ip::tcp::resolver *resolver_;

    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>>  *websocket_;

    std::string host_;

    std::string endpoint_;

    boost::beast::multi_buffer buffer_;

    on_connected_cb_t on_connected_cb_;

    on_disconnected_cb_t on_disconnected_cb_;

    on_message_cb_t on_message_cb_;

    on_error_cb_t on_error_cb_;

private:

    void run(char const* host, char const* port, char const* endpoint);

    void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);

    void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type);

    void on_ssl_handshake(boost::beast::error_code ec);

    void on_handshake(boost::beast::error_code ec);

    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);

    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);

    

};