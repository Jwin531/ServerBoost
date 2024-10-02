#include "server.h"
#include "session.h"
#include <iostream>
#include <memory>

using boost::asio::ip::tcp;
using namespace std;
using namespace sw::redis;

Server::Server(boost::asio::io_context& io_context, short port): acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), redis_(make_shared<Redis>("tcp://127.0.0.1:6379")) 
    {
    do_accept();
}

void Server::do_accept() {

    
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                cout << "Клиент подключен." << endl;
                auto session = make_shared<Session>(make_shared<tcp::socket>(move(socket)), *this);
                session->do_read_login();
                sessions_.insert(session);
            } else {
                cerr << "Ошибка подключения: " << ec.message() << endl;
            }
            do_accept();
        });
}

void Server::save_session_to_redis(const string& login, const string& uniqueSessionId) 
{
    redis_->hset("sessions", login, uniqueSessionId);
}

vector<std::string> Server::getActiveLogins(const string& currentLogin)
{
    vector<string> logins;
    try
    {
        redis_->hkeys("sessions", back_inserter(logins));

        logins.erase(
            remove_if(logins.begin(),logins.end(),
            [&currentLogin](const string& login)
            {
                return login == currentLogin;
            }),
            logins.end());

        json response;
        response["type"] = "activeUsers";
        response["logins"] = logins;

        string message = response.dump();

    }
    catch(const exception& e)
    {
        cerr << e.what() << '\n';
    }
    return logins;
}
