#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>

struct sqlite3;

struct Transaction {
    int id{};
    std::string type;
    std::string category;
    double amount{};
    std::string note;
    std::string date;
};

struct CategoryTotal {
    std::string category;
    double total{};
};

class Storage {
public:
    explicit Storage(const std::string& databasePath = "expenses.db");
    ~Storage();

    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    int addTransaction(const std::string& type,
                       const std::string& category,
                       double amount,
                       const std::string& note,
                       const std::string& date);

    std::vector<Transaction> listTransactions(int limit = 50) const;
    bool deleteTransaction(int id);
    double totalForType(const std::string& type) const;
    std::vector<CategoryTotal> totalsByCategory(const std::string& type) const;

private:
    sqlite3* db{};

    void initializeSchema();
    void execute(const std::string& sql) const;
};

#endif
