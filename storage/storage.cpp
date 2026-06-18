#include "storage.h"

#include "sqlite3.h"

#include <stdexcept>
#include <utility>

namespace {
void throwSqliteError(sqlite3* db, const std::string& context) {
    const char* message = db ? sqlite3_errmsg(db) : "unknown sqlite error";
    throw std::runtime_error(context + ": " + message);
}
}

Storage::Storage(const std::string& databasePath) {
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK) {
        std::string error = db ? sqlite3_errmsg(db) : "unknown sqlite error";
        sqlite3_close(db);
        db = nullptr;
        throw std::runtime_error("Failed to open database: " + error);
    }

    initializeSchema();
}

Storage::~Storage() {
    if (db) {
        sqlite3_close(db);
    }
}

int Storage::addTransaction(const std::string& type,
                            const std::string& category,
                            double amount,
                            const std::string& note,
                            const std::string& date) {
    const char* sql =
        "INSERT INTO transactions (type, category, amount, note, date) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throwSqliteError(db, "Failed to prepare insert");
    }

    sqlite3_bind_text(statement, 1, type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(statement, 3, amount);
    sqlite3_bind_text(statement, 4, note.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, date.c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    if (result != SQLITE_DONE) {
        throwSqliteError(db, "Failed to insert transaction");
    }

    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

std::vector<Transaction> Storage::listTransactions(int limit) const {
    const char* sql =
        "SELECT id, type, category, amount, note, date "
        "FROM transactions "
        "ORDER BY date DESC, id DESC "
        "LIMIT ?;";

    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throwSqliteError(db, "Failed to prepare transaction list");
    }

    sqlite3_bind_int(statement, 1, limit);

    std::vector<Transaction> transactions;
    int stepResult = SQLITE_ROW;
    while ((stepResult = sqlite3_step(statement)) == SQLITE_ROW) {
        Transaction transaction;
        transaction.id = sqlite3_column_int(statement, 0);
        transaction.type = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
        transaction.category = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
        transaction.amount = sqlite3_column_double(statement, 3);

        const unsigned char* note = sqlite3_column_text(statement, 4);
        transaction.note = note ? reinterpret_cast<const char*>(note) : "";

        transaction.date = reinterpret_cast<const char*>(sqlite3_column_text(statement, 5));
        transactions.push_back(std::move(transaction));
    }

    sqlite3_finalize(statement);

    if (stepResult != SQLITE_DONE) {
        throwSqliteError(db, "Failed to read transactions");
    }

    return transactions;
}

bool Storage::deleteTransaction(int id) {
    const char* sql = "DELETE FROM transactions WHERE id = ?;";

    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throwSqliteError(db, "Failed to prepare delete");
    }

    sqlite3_bind_int(statement, 1, id);

    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    if (result != SQLITE_DONE) {
        throwSqliteError(db, "Failed to delete transaction");
    }

    return sqlite3_changes(db) > 0;
}

double Storage::totalForType(const std::string& type) const {
    const char* sql = "SELECT COALESCE(SUM(amount), 0) FROM transactions WHERE type = ?;";

    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throwSqliteError(db, "Failed to prepare total query");
    }

    sqlite3_bind_text(statement, 1, type.c_str(), -1, SQLITE_TRANSIENT);

    double total = 0;
    int result = sqlite3_step(statement);
    if (result == SQLITE_ROW) {
        total = sqlite3_column_double(statement, 0);
    }

    sqlite3_finalize(statement);

    if (result != SQLITE_ROW && result != SQLITE_DONE) {
        throwSqliteError(db, "Failed to read total");
    }

    return total;
}

std::vector<CategoryTotal> Storage::totalsByCategory(const std::string& type) const {
    const char* sql =
        "SELECT category, SUM(amount) "
        "FROM transactions "
        "WHERE type = ? "
        "GROUP BY category "
        "ORDER BY SUM(amount) DESC, category ASC;";

    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throwSqliteError(db, "Failed to prepare category totals");
    }

    sqlite3_bind_text(statement, 1, type.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<CategoryTotal> totals;
    int stepResult = SQLITE_ROW;
    while ((stepResult = sqlite3_step(statement)) == SQLITE_ROW) {
        CategoryTotal total;
        total.category = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
        total.total = sqlite3_column_double(statement, 1);
        totals.push_back(std::move(total));
    }

    sqlite3_finalize(statement);

    if (stepResult != SQLITE_DONE) {
        throwSqliteError(db, "Failed to read category totals");
    }

    return totals;
}

void Storage::initializeSchema() {
    execute(
        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "type TEXT NOT NULL CHECK (type IN ('income', 'expense')),"
        "category TEXT NOT NULL,"
        "amount REAL NOT NULL CHECK (amount > 0),"
        "note TEXT NOT NULL DEFAULT '',"
        "date TEXT NOT NULL"
        ");");

    execute(
        "CREATE INDEX IF NOT EXISTS idx_transactions_type_date "
        "ON transactions(type, date);");
}

void Storage::execute(const std::string& sql) const {
    char* error = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error) != SQLITE_OK) {
        std::string message = error ? error : "unknown sqlite error";
        sqlite3_free(error);
        throw std::runtime_error("Failed to execute SQL: " + message);
    }
}
