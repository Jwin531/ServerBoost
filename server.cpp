#include "server.h"
#include "session.h"
#include "database.h"
#include <iostream>
#include <memory>
#include <ctime>
#include <chrono>
#include <algorithm>

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

void Server::save_session_to_redis(const std::string& login, const std::string& uniqueSessionId) 
{
    // Текущее время в формате ISO 8601
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::string last_active_time = std::ctime(&now_time_t);  // Преобразуем в строку

    // Сохранение информации о сессии в виде хэша
    redis_->hset("sessions:" + login, "sessionId", uniqueSessionId);
    redis_->hset("sessions:" + login, "status", "online");
    redis_->hset("sessions:" + login, "last_active_time", last_active_time);

    // Установка TTL (например, 5 минут)
    redis_->expire("sessions:" + login, 300); // 300 секунд (5 минут)

    // Добавление в множество активных пользователей
    redis_->sadd("active_users", login);
}

vector<std::string> Server::getActiveLogins(const string& currentLogin)
{
    vector<string> logins;
    try
    {
        redis_->smembers("active_users", back_inserter(logins));

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

void Server::removeSession(const shared_ptr<Session>& session)
{
    sessions_.erase(session);
}

void Server::sendMessageToUser(const string& receiverLogin, const string& senderLogin, const string& textMessage) 
{
    for (const auto& session : sessions_) 
    {
        if (session->getSessionLogin() == receiverLogin) // Предполагается, что вы добавили метод getLogin() в класс Session
        {
            json response;
            response["type"] = "message";
            response["sender"] = senderLogin;
            response["message"] = textMessage;
            
            cout << "Cообщение отправится: " << receiverLogin << endl;
            auto socket = getSocket(receiverLogin);

            if (socket) // Проверка на существование сокета
            {
                session->sendMessageToReceiver(socket, response.dump()); // Отправляем сообщение пользователю
            }
            break; // Выходим из цикла, так как сообщение отправлено
        }
    }
}
