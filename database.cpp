#include "database.h"

// Реализация методов вашего класса Database
void Database::connect() {
    conn = PQconnectdb("dbname=messenger user=postgres password=G41z2pva port=5432 host=localhost");
    if (PQstatus(conn) == CONNECTION_BAD) {
        std::string error = PQerrorMessage(conn);
        PQfinish(conn); // Завершите соединение, если оно не удалось
        throw std::runtime_error("Connection to database failed: " + error);
    }
    std::cout << "Connected to the database!" << std::endl;
}

void Database::disconnect() {
    if (conn) {
        PQfinish(conn);
        conn = nullptr; // Обнулите указатель после завершения соединения
        cout << "Disconnected from the database." << endl;
    }
}

Database& Database::instance()
{
    static Database instance;
    return instance;
}

void Database::saveMessage(const string& message, const string& receiver, const string& sender, const string& time)
{
    if (conn == nullptr) {
        throw runtime_error("Database is not connected.");
    }

    // Формируем SQL-запрос на вставку данных
    string query = "INSERT INTO messages (sender_id, receiver_id, content, deliver_status, timestamp) "
                   "VALUES ($1, $2, $3, false, $4)";

    // Подготовка параметров для запроса
    const char* paramValues[4];
    paramValues[0] = sender.c_str();
    paramValues[1] = receiver.c_str();
    paramValues[2] = message.c_str();
    paramValues[3] = time.c_str();

    // Выполнение SQL-запроса
    PGresult* res = PQexecParams(conn, query.c_str(), 4, nullptr, paramValues, nullptr, nullptr, 0);

    // Проверяем результат выполнения
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Error saving message: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        return;
    }

    // Очищаем результат запроса
    PQclear(res);
    cout << "Message saved successfully." << endl;
}