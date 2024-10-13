#ifndef DATABASE_H
#define DATABASE_H

#include <libpq-fe.h>
#include <string>
#include <unordered_set>
#include <iostream>
#include <stdexcept>

using namespace std;

class Database {
public:

    static Database& instance();

    void connect();
    void disconnect();
    void saveMessage(const string& message, const string& receiver, const string& sender, const string& time, bool status);
    string selectChatWith(const string& login);

private:
    Database() : conn(nullptr) {}
    ~Database() { disconnect(); }
    PGconn* conn;
};

#endif // DATABASE_H
