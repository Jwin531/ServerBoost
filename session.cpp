#include "session.h"
#include "server.h"
#include "database.h"
#include <iostream>
#include <utility>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using boost::asio::ip::tcp;
using json = nlohmann::json;
using namespace std;

Session::Session(shared_ptr<tcp::socket> socket, Server& server)
    : socket_(move(socket)), server_(server) {}

void Session::start() {
    cout << "Сессия началась для клиента." << endl;
    do_read();
}

void Session::do_read() 
{
    auto self(shared_from_this());

    boost::asio::async_read_until(*socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, size_t length) 
        {
            if (!ec) 
            {
                do_read();
            } 
            else
            {
                cerr << "Ошибка чтения данных: " << ec.message() << endl;
            }
        });
}

void Session::do_write(const string& message) {
    auto self(shared_from_this());

    json json_message = {{"response", message}};
    string json_data = json_message.dump() + "\n";

    boost::asio::async_write(*socket_, boost::asio::buffer(json_data),
        [this, self, json_data](const boost::system::error_code& ec, size_t /*length*/) {
            if (ec) {
                cerr << "Ошибка отправки сообщения клиенту: " << ec.message() << endl;
            } else {
                cout << "Сообщение отправлено клиенту: " << json_data << endl;
            }
        });
}

string createUniqueSessionId()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return boost::uuids::to_string(uuid);
}



void Session::do_read_login() {
    auto self(shared_from_this());
    boost::asio::async_read_until(*socket_, buffer_, "\n",
        [this, self](boost::system::error_code ec, std::size_t length) 
        {
            if (!ec) 
            {
                istream is(&buffer_);
                string message(length, '\0');
                is.read(&message[0], length);

                if (!message.empty() && message.back() == '\n') 
                {
                    message.pop_back();
                }

                if (message.empty()) 
                {
                    cerr << "Получено пустое сообщение." << endl;
                    return;
                }
                
                try 
                {
                    json login_message = json::parse(message);

                    if (login_message.contains("login")) 
                    {
                        server_.save_session_to_redis(login_message["login"],createUniqueSessionId());
                        onUserConnected(login_message["login"],self);
                        do_read();
                    }
                    else 
                    {
                        cout << "Неверный формат сообщения." << endl;
                    }
                } 
                catch (const json::parse_error& e) 
                {
                    cerr << "Ошибка разбора JSON: " << e.what() << endl;
                }
            }
        });
}

void Session::sendMessageToAll(const string& message, shared_ptr<Session> newSession)
{
    for(const auto& session: server_.getAllSessions())
    {
        if(session == newSession)
        {
            continue;
        }
        else
        {
            session->do_write(message);
        }
    }
}

void Session::onUserConnected(const std::string& newLogin, std::shared_ptr<Session> newSession)
{
    auto self(shared_from_this());
    // 1. Получить список активных логинов и отправить новому пользователю
    vector<std::string> activeLogins = server_.getActiveLogins(newLogin);

    json response;
    response["type"] = "active_users";
    response["logins"] = activeLogins;

    std::string message = response.dump();
    cout << "Отправил одному юзеру: " << message << endl;
    newSession->do_write(message); // Отправляем новому пользователю список активных пользователей

    // 2. Сообщить всем остальным о подключении нового пользователя
    json notification;
    notification["type"] = "new_user";
    notification["login"] = newLogin;

    std::string notificationMessage = notification.dump();
    cout << "Отправил остальным пользователям: " << notificationMessage << endl;
    sendMessageToAll(notificationMessage,self); // Рассылаем сообщение всем пользователям
}