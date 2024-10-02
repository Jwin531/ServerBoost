#include "server.h"
#include "database.h"

int main() {
    boost::asio::io_context io_context;
    Server server(io_context, 1234);
    Database& db = Database::instance();
    
    try {
        db.connect(); // Попытка подключиться к базе данных
        io_context.run();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl; // Обработка ошибки подключения
        return 1;
    }

    return 0;
}
