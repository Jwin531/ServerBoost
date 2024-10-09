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
    ~Session();

    void start();
    void do_read_login();
    void sendMessageToAll(const string& message, std::shared_ptr<Session> newSession);
    void onUserConnected(const std::string& newLogin, std::shared_ptr<Session> newSession);
    void handleMessage(const string& message);
    string getSessionLogin(){return sessionLogin;}
    void sendMessageToReceiver(shared_ptr<tcp::socket> socket, const string& message);
    void closeSession();

private:
    void do_write(const string& message);
    void do_read();


    shared_ptr<tcp::socket> socket_;
    boost::asio::streambuf buffer_;
    Server& server_; 
    string sessionLogin;
};

#endif // SESSION_H
