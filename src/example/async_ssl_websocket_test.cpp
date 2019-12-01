#include <iostream>
#include <string>
#include "ws_session.h"
#include "timestamp.h"


void on_connected()
{
    std::cout << mqt::timer::Timestamp::now().fmt_string() << " -----------------> on_connected " << std::endl;
}

void on_disconnected()
{
    std::cout << mqt::timer::Timestamp::now().fmt_string() << " -----------------> on_disconnected " << std::endl;
}

void on_message(const std::string& message)
{
    std::cout << mqt::timer::Timestamp::now().fmt_string() << " -----------------> on_message: " << message << std::endl;
}

void on_error(const int& error_id, const std::string& error_msg)
{
    std::cout << mqt::timer::Timestamp::now().fmt_string() << " -----------------> on_error  error_id: " << error_id << " error_msg: " << error_msg << std::endl;
}

int main(int argc, char** argv)
{
    std::string host = "www.bitmex.com";
    std::string port = "443";
    std::string endpoint = "/realtime";

    if (argc >= 2) host = argv[1];
    if (argc >= 3) port = argv[2];
    if (argc >= 4) port = argv[3];
    
    auto session = std::make_shared<WSSession>();
    session->reg_on_connected_cb(std::bind(on_connected));
    session->reg_on_disconnected_cb(std::bind(on_disconnected));
    session->reg_on_message_cb(std::bind(on_message, std::placeholders::_1));
    session->reg_on_error_cb(std::bind(on_error, std::placeholders::_1, std::placeholders::_2));
    session->connect(host.c_str(), port.c_str(), endpoint.c_str());

    while (true)
    {
        sleep(5);
    }
    

    return 0;
}