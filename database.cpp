// #include "database.h"

// // Реализация методов вашего класса Database
// void Database::connect() {
//     conn = PQconnectdb("dbname=messenger user=postgres password=G41z2pva port=5432 host=localhost");
//     if (PQstatus(conn) == CONNECTION_BAD) {
//         std::string error = PQerrorMessage(conn);
//         PQfinish(conn); // Завершите соединение, если оно не удалось
//         throw std::runtime_error("Connection to database failed: " + error);
//     }
//     std::cout << "Connected to the database!" << std::endl;
// }

// void Database::disconnect() {
//     if (conn) {
//         PQfinish(conn);
//         conn = nullptr; // Обнулите указатель после завершения соединения
//         cout << "Disconnected from the database." << endl;
//     }
// }

// string Database::selectUserDataBySessinId(const string& sessionId)
// {
//     string query = "SELECT login FROM users WHERE session_id = '" + sessionId + "';";

//     PGresult* res = PQexec(conn,query.c_str());

//     if (PQresultStatus(res) != PGRES_TUPLES_OK) {
//         std::string error = PQerrorMessage(conn);
//         PQclear(res); // Очистите результат
//         throw std::runtime_error("Query execution failed: " + error);
//     }

//     std::string login;
//     if (PQntuples(res) > 0) {
//         login = PQgetvalue(res, 0, 0);
//     }

//     PQclear(res); 
//     return login; 
// }

// Database& Database::instance()
// {
//     static Database instance;
//     return instance;
// }
