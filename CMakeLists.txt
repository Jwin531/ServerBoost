cmake_minimum_required(VERSION 3.10)
project(server_app)

# Найдите необходимые библиотеки
find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)
find_package(PostgreSQL REQUIRED)

# Укажите каталоги заголовков
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${nlohmann_json_INCLUDE_DIRS})
include_directories(${PostgreSQL_INCLUDE_DIRS})

# Укажите путь к заголовкам Redis++
include_directories(/usr/local/include/sw/redis++)

# Укажите библиотеку Redis++
find_library(REDISPP_LIBRARY redis++ HINTS /usr/local/lib)

# Укажите библиотеку hiredis
find_library(HIREDIS_LIBRARY hiredis HINTS /usr/lib/x86_64-linux-gnu) # Убедитесь, что путь верный

# Укажите все исходные файлы проекта
set(SOURCES
    main.cpp          # Файл с функцией main
    server.cpp
    session.cpp
    database.cpp
)

# Создайте исполняемый файл
add_executable(server_app ${SOURCES})

# Свяжите целевой файл с библиотеками
target_link_libraries(server_app 
    ${Boost_LIBRARIES} 
    nlohmann_json::nlohmann_json 
    ${PostgreSQL_LIBRARIES} 
    ${REDISPP_LIBRARY}  
    ${HIREDIS_LIBRARY}  # Добавьте библиотеку hiredis
)
