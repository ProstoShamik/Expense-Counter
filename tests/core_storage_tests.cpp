#include "core.h"

#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
bool closeEnough(double left, double right) {
    return std::fabs(left - right) < 0.0001;
}

std::filesystem::path testDatabasePath() {
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    return std::filesystem::temp_directory_path() /
        ("expense_counter_test_" + std::to_string(now) + ".db");
}
}

int main() {
    std::filesystem::path databasePath = testDatabasePath();

    {
        Storage storage(databasePath.string());
        Core core(storage);

        int salaryId = core.addIncome("Salary", 1000.0, "June", "2026-06-01");
        int foodId = core.addExpense("Food", 120.5, "Groceries", "2026-06-02");
        core.addExpense("Food", 30.0, "Lunch", "2026-06-03");

        assert(salaryId > 0);
        assert(foodId > 0);

        Summary firstSummary = core.summary();
        assert(closeEnough(firstSummary.income, 1000.0));
        assert(closeEnough(firstSummary.expense, 150.5));
        assert(closeEnough(firstSummary.balance, 849.5));

        std::vector<Transaction> transactions = core.recentTransactions(10);
        assert(transactions.size() == 3);
        assert(transactions.front().date == "2026-06-03");

        std::vector<CategoryTotal> expenseReport = core.expenseReportByCategory();
        assert(expenseReport.size() == 1);
        assert(expenseReport.front().category == "Food");
        assert(closeEnough(expenseReport.front().total, 150.5));

        assert(core.deleteTransaction(foodId));
        assert(!core.deleteTransaction(999999));

        Summary secondSummary = core.summary();
        assert(closeEnough(secondSummary.income, 1000.0));
        assert(closeEnough(secondSummary.expense, 30.0));
        assert(closeEnough(secondSummary.balance, 970.0));

        bool invalidAmountRejected = false;
        try {
            core.addExpense("Food", 0.0, "invalid", "2026-06-04");
        } catch (const std::invalid_argument&) {
            invalidAmountRejected = true;
        }
        assert(invalidAmountRejected);

        bool invalidDateRejected = false;
        try {
            core.addIncome("Gift", 10.0, "invalid", "06-04-2026");
        } catch (const std::invalid_argument&) {
            invalidDateRejected = true;
        }
        assert(invalidDateRejected);
    }

    std::filesystem::remove(databasePath);
    std::cout << "All tests passed\n";
    return 0;
}
