cmake_minimum_required(VERSION 3.14)

# Название проекта и языки (C для SQLite, CXX для остального)
project(ExpenseCounter VERSION 1.0 LANGUAGES C CXX)

# Стандарт C++ (рекомендуется C++17 или новее)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. Сборка SQLite (Thirdparty) ---
# Собираем sqlite3.c как статическую библиотеку. 
# shell.c не нужен, так как это консольная утилита sqlite, а не библиотека.
add_library(sqlite3_lib STATIC thirdparty/sqlite3/sqlite3.c)

# Указываем, где лежат .h файлы для sqlite (чтобы другие могли делать #include "sqlite3.h")
target_include_directories(sqlite3_lib PUBLIC thirdparty/sqlite3)

# Для Linux/Unix иногда нужны системные библиотеки для потоков и dl
if(UNIX AND NOT APPLE)
    target_link_libraries(sqlite3_lib PRIVATE pthread dl)
endif()


# --- 2. Модуль Storage ---
add_library(storage_lib STATIC 
    storage/storage.cpp
    storage/storage.h
)

# Указываем путь к заголовкам (PUBLIC означает, что тот, кто использует storage_lib,
# тоже увидит эту папку и сможет написать #include "storage.h")
target_include_directories(storage_lib PUBLIC storage)

# Если Storage использует SQLite, линкуем его
target_link_libraries(storage_lib PRIVATE sqlite3_lib)


# --- 3. Модуль Core ---
add_library(core_lib STATIC 
    core/core.cpp
    core/core.h
)

target_include_directories(core_lib PUBLIC core)

# Core зависит от Storage (так как в core.h есть #include "storage.h")
# Используем PUBLIC, так как типы из Storage видны в хедере Core
target_link_libraries(core_lib PUBLIC storage_lib)


# --- 4. Модуль Terminal UI ---
add_library(ui_lib STATIC 
    terminal_ui/ui.cpp
    terminal_ui/ui.h
)

target_include_directories(ui_lib PUBLIC terminal_ui)

# UI скорее всего будет вызывать методы Core
target_link_libraries(ui_lib PRIVATE core_lib)


# --- 5. Главный исполняемый файл ---
add_executable(ExpenseCounter main.cpp)

# Линкуем с основными модулями
target_link_libraries(ExpenseCounter 
    PRIVATE 
    core_lib 
    ui_lib
    sqlite3_lib
)