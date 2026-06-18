#ifndef CORE_H
#define CORE_H

#include "storage.h"

#include <string>
#include <vector>

struct Summary {
    double income{};
    double expense{};
    double balance{};
};

class Core {
public:
    explicit Core(Storage& storage);

    int addIncome(const std::string& category,
                  double amount,
                  const std::string& note,
                  const std::string& date = "");

    int addExpense(const std::string& category,
                   double amount,
                   const std::string& note,
                   const std::string& date = "");

    std::vector<Transaction> recentTransactions(int limit = 50) const;
    bool deleteTransaction(int id);
    Summary summary() const;
    std::vector<CategoryTotal> expenseReportByCategory() const;
    std::vector<CategoryTotal> incomeReportByCategory() const;

private:
    Storage& storage;

    int addTransaction(const std::string& type,
                       const std::string& category,
                       double amount,
                       const std::string& note,
                       const std::string& date);

    static void validateTransaction(const std::string& category,
                                    double amount,
                                    const std::string& date);
    static std::string normalizeDate(const std::string& date);
    static std::string today();
};

#endif
