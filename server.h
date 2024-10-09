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

    // Сохранение информации о сессии в Redis
    void save_session_to_redis(const string& login, const string& uniqueSessionId);

    // Получение списка активных логинов, кроме текущего пользователя
    vector<std::string> getActiveLogins(const string& currentLogin);

    // Получение всех активных сессий
    set<shared_ptr<Session>> getAllSessions(){return sessions_;}

    // Удаление сессии из множества активных сессий
    void removeSession(const shared_ptr<Session>& session);

    // Отправка сообщения конкретному пользователю
    void sendMessageToUser(const string& receiverLogin, const string& senderLogin, const string& textMessage);

    // Привязка логина пользователя к его сокету
    void setSessionsByLogin(const string& login, shared_ptr<tcp::socket> socket){sessionsByLogin_[login] = socket;}

    // Получение сокета, связанного с логином
    shared_ptr<tcp::socket> getSocket(const string& login){return sessionsByLogin_[login];}

    // Удаление пользователя из активных сессий и Redis
    void removeUserFromMap(const string& login){sessionsByLogin_.erase(login); redis_->srem("active_users",login); redis_->del("sessions:"+login);}

private:
    // Обработка передачи сообщений между пользователями
    void handleMessageFromTo();

    // Начало процесса принятия новых подключений
    void do_accept();

    tcp::acceptor acceptor_;
    shared_ptr<Redis> redis_;
    set<shared_ptr<Session>> sessions_;
    unordered_map<string,shared_ptr<tcp::socket>> sessionsByLogin_;
};

#endif // SERVER_H
