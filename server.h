#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sw/redis++/redis++.h>
#include <set>
#include <memory>

using boost::asio::ip::tcp;
using json = nlohmann::json;
using namespace sw::redis;
using namespace std;

class Session;

class Server {
public:
    Server(boost::asio::io_context& io_context, short port);
    void save_session_to_redis(const string& login, const string& uniqueSessionId);
    vector<std::string> getActiveLogins(const string& currentLogin);
    set<shared_ptr<Session>> getAllSessions(){return sessions_;}
    void removeSession(const shared_ptr<Session>& session);
    void sendMessageToUser(const string& receiverLogin, const string& senderLogin, const string& textMessage);
    void setSessionsByLogin(const string& login, shared_ptr<tcp::socket> socket){sessionsByLogin_[login] = socket;}
    shared_ptr<tcp::socket> getSocket(const string& login){return sessionsByLogin_[login];}

private:
    void handleMessageFromTo();
    void do_accept();

    tcp::acceptor acceptor_;
    shared_ptr<Redis> redis_;
    set<shared_ptr<Session>> sessions_;
    unordered_map<string,shared_ptr<tcp::socket>> sessionsByLogin_;
};

#endif // SERVER_H
