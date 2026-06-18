#ifndef UI_H
#define UI_H

#include "core.h"

#include <string>
#include <vector>

class UI {
public:
    explicit UI(Core& core);

    void run();

private:
    Core& core;

    void showMenu() const;
    void addIncome();
    void addExpense();
    void listTransactions() const;
    void deleteTransaction();
    void showSummary() const;
    void showReports() const;

    void addTransaction(bool income);
    void printTransactions(const std::vector<Transaction>& transactions) const;
    void printCategoryTotals(const std::vector<CategoryTotal>& totals) const;

    static std::string readLine(const std::string& prompt);
    static double readAmount(const std::string& prompt);
    static int readInt(const std::string& prompt);
};

#endif
