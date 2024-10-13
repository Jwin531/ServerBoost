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

void Database::saveMessage(const std::string& message, const std::string& receiver, const std::string& sender, const std::string& time, bool status)
{
    if (conn == nullptr) {
        throw std::runtime_error("Database is not connected.");
    }

    // Формируем SQL-запрос на вставку данных
    std::string query = "INSERT INTO messages (sender_id, receiver_id, content, deliver_status, timestamp) "
                        "VALUES ($1, $2, $3, $4, $5)";

    // Подготовка параметров для запроса
    const char* paramValues[5];
    paramValues[0] = sender.c_str();         // Логин отправителя
    paramValues[1] = receiver.c_str();       // Логин получателя
    paramValues[2] = message.c_str();        // Текст сообщения
    paramValues[3] = status ? "true" : "false"; // Статус доставки (boolean)
    paramValues[4] = time.c_str();           // Время отправки сообщения

    // Выполнение SQL-запроса с 5 параметрами
    PGresult* res = PQexecParams(conn, query.c_str(), 5, nullptr, paramValues, nullptr, nullptr, 0);

    // Проверяем результат выполнения
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Error saving message: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        throw std::runtime_error("Failed to save message.");
    }

    // Очищаем результат запроса
    PQclear(res);
    std::cout << "Message saved successfully." << std::endl;
}


string Database::selectChatWith(const string& login)
{
    if (conn == nullptr) 
    {
        throw runtime_error("Database is not connected.");
    }

    string query = "SELECT in_chat FROM  users WHERE login= $1";

    const char* param[1] = {login.c_str()};

    PGresult* result = PQexecParams(
        conn,               // Указатель на подключение
        query.c_str(),      // SQL-запрос
        1,                  // Количество параметров
        nullptr,            // Массив типов параметров (nullptr означает автоматическое определение)
        param,              // Массив значений параметров
        nullptr,            // Длины параметров (если строки, можно оставить nullptr)
        nullptr,            // Формат параметров (0 для текстового формата)
        0                   // Формат результата (0 для текста, 1 для бинарного)
    );

    // Проверяем статус выполнения
    if (PQresultStatus(result) != PGRES_TUPLES_OK) 
    {
        std::string error_message = PQerrorMessage(conn);
        PQclear(result); // Освобождаем ресурсы
        throw std::runtime_error("Query execution failed: " + error_message);
    }

    // Извлечение результата (первой строки и первого столбца)
    std::string in_chat;
    if (PQntuples(result) > 0) 
    {
        in_chat = PQgetvalue(result, 0, 0);
    }

    PQclear(result); // Освобождаем ресурсы
    return in_chat;

}