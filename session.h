#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <thread>

using boost::asio::ip::tcp;
using json = nlohmann::json;
using namespace std;

class Server;

class Session : public enable_shared_from_this<Session> {
public:
    explicit Session(shared_ptr<tcp::socket> socket, Server& server);

    void start();
    void do_read_login();
    void sendMessageToAll(const string& message, std::shared_ptr<Session> newSession);
    void onUserConnected(const std::string& newLogin, std::shared_ptr<Session> newSession);

private:
    void do_read();

    void do_write(const string& message);

    shared_ptr<tcp::socket> socket_;
    boost::asio::streambuf buffer_;
    Server& server_; 
};

#endif // SESSION_H
