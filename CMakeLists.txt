cmake_minimum_required(VERSION 3.14)

project(ExpenseCounter VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sqlite3_lib STATIC thirdparty/sqlite3/sqlite3.c)
target_include_directories(sqlite3_lib PUBLIC thirdparty/sqlite3)

if(UNIX AND NOT APPLE)
    target_link_libraries(sqlite3_lib PRIVATE pthread dl)
endif()

add_library(storage_lib STATIC
    storage/storage.cpp
    storage/storage.h
)
target_include_directories(storage_lib PUBLIC storage)
target_link_libraries(storage_lib PRIVATE sqlite3_lib)

add_library(core_lib STATIC
    core/core.cpp
    core/core.h
)
target_include_directories(core_lib PUBLIC core)
target_link_libraries(core_lib PUBLIC storage_lib)

add_library(ui_lib STATIC
    terminal_ui/ui.cpp
    terminal_ui/ui.h
)
target_include_directories(ui_lib PUBLIC terminal_ui)
target_link_libraries(ui_lib PUBLIC core_lib)

add_executable(ExpenseCounter main.cpp)
target_link_libraries(ExpenseCounter PRIVATE ui_lib)

include(CTest)
if(BUILD_TESTING)
    add_executable(expense_counter_tests tests/core_storage_tests.cpp)
    target_link_libraries(expense_counter_tests PRIVATE core_lib)
    add_test(NAME expense_counter_tests COMMAND expense_counter_tests)
endif()
