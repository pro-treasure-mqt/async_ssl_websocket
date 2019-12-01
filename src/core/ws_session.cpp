#include "ws_session.h"

WSSession::WSSession()
    : running_(false)
    , connected_(false)
    , resolver_(nullptr)
    , websocket_(nullptr)
{
}


int WSSession::connect(char const* host, char const* port, char const* endpoint)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (running_)
    {
        return -1;
    }
    running_ = true;
    lock.unlock();
    std::thread(&WSSession::run, shared_from_this(), host, port, endpoint).detach();
    return 0;
}

int WSSession::async_send(const std::string& message)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (websocket_) 
    {
        websocket_->async_write(boost::asio::buffer(message), boost::beast::bind_front_handler(&WSSession::on_write, shared_from_this()));
        lock.unlock();
        return 0;
    }
    lock.unlock();
    return -1;
}

void WSSession::reg_on_connected_cb(const on_connected_cb_t& on_connected_cb)
{
    on_connected_cb_ = on_connected_cb;
}

void WSSession::reg_on_disconnected_cb(const on_disconnected_cb_t& on_disconnected_cb)
{
    on_disconnected_cb_ = on_disconnected_cb;
}

void WSSession::reg_on_message_cb(const on_message_cb_t& on_message_cb)
{
    on_message_cb_ = on_message_cb;
}

void WSSession::reg_on_error_cb(const on_error_cb_t& on_error_cb)
{
    on_error_cb_ = on_error_cb;
}


void WSSession::run(char const* host, char const* port, char const* endpoint)
{
    host_ = host;
    endpoint_ = endpoint;
    while (true)
    {
        try 
        {
            boost::asio::io_context io_ctx;
            boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
            load_root_certificates(ctx);
            resolver_ = new boost::asio::ip::tcp::resolver(boost::asio::make_strand(io_ctx));
            websocket_ = new boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>>(boost::asio::make_strand(io_ctx), ctx);
            resolver_->async_resolve(host, port, boost::beast::bind_front_handler(&WSSession::on_resolve, shared_from_this()));
            io_ctx.run();
            if (connected_ &&on_disconnected_cb_)
            {
                connected_ = false;
                on_disconnected_cb_();
            }
            std::unique_lock<std::mutex> lock(mtx_);
            if (websocket_)
            {
                delete websocket_;
                websocket_ = nullptr;
            }
            if (websocket_)
            {
                delete resolver_;
                resolver_ = nullptr;
            }
            lock.unlock();
            sleep(5);
        } 
        catch (...) 
        {
            std::unique_lock<std::mutex> lock(mtx_);
            if (websocket_)
            {
                delete websocket_;
                websocket_ = nullptr;
            }
            if (websocket_)
            {
                delete resolver_;
                resolver_ = nullptr;
            }
            lock.unlock();
            std::cout << mqt::timer::Timestamp::now().fmt_string() << " ----> run occurs an exception !" << std::endl;
        }
    }

}

void WSSession::on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
{
    if (ec)
    {
        if (on_error_cb_)
        {
            on_error_cb_(ec.value(), ec.message());
        }
        return;
    }
    boost::beast::get_lowest_layer(*websocket_).expires_after(std::chrono::seconds(10));
    boost::beast::get_lowest_layer(*websocket_).async_connect(results, boost::beast::bind_front_handler(&WSSession::on_connect, shared_from_this()));
}

void WSSession::on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type)
{
    if (ec)
    {
        if (on_error_cb_)
        {
            on_error_cb_(ec.value(), ec.message());
        }
        return;
    }
    boost::beast::get_lowest_layer(*websocket_).expires_after(std::chrono::seconds(10));
    websocket_->next_layer().async_handshake(boost::asio::ssl::stream_base::client, boost::beast::bind_front_handler(&WSSession::on_ssl_handshake, shared_from_this()));
}

void WSSession::on_ssl_handshake(boost::beast::error_code ec)
{
    if (ec)
    {
        if (on_error_cb_)
        {
            on_error_cb_(ec.value(), ec.message());
        }
        return;
    }
    boost::beast::get_lowest_layer(*websocket_).expires_never();
    websocket_->set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));
    websocket_->set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::request_type& req)
        {
            req.set(boost::beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl");
        }
    ));
    websocket_->async_handshake(host_, endpoint_, boost::beast::bind_front_handler(&WSSession::on_handshake, shared_from_this()));
}

void WSSession::on_handshake(boost::beast::error_code ec)
{
    if (ec)
    {
        if (on_error_cb_)
        {
            on_error_cb_(ec.value(), ec.message());
        }
        return;
    }
    connected_ = true;
    if (on_connected_cb_)
    {
        on_connected_cb_();
    }
    websocket_->async_read(buffer_, boost::beast::bind_front_handler(&WSSession::on_read, shared_from_this()));
}

void WSSession::on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if (ec)
    {
        if (on_error_cb_)
        {
            on_error_cb_(ec.value(), ec.message());
        }
        return;
    }
}

void WSSession::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if (ec)
    {
        if (on_error_cb_)
        {
            on_error_cb_(ec.value(), ec.message());
        }
        return;
    }
    if (on_message_cb_)
    {
        on_message_cb_(boost::beast::buffers_to_string(buffer_.data()));
    }
    buffer_.consume(buffer_.size());
    websocket_->async_read(buffer_, boost::beast::bind_front_handler(&WSSession::on_read, shared_from_this()));

}

