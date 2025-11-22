#include "sqlite3.h"
#include "storage.h"
#include "core.h"
#include "ui.h"

int main() {
    sqlite3* db;
    if (sqlite3_open("test.db", &db) == SQLITE_OK) {
        sqlite3_close(db);
    }
    
    Storage storage;
    Core core(storage);
    UI ui;

    return 0;
}
