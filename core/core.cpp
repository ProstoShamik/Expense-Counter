#include "core.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

Core::Core(Storage& storage) : storage(storage) {
}

int Core::addIncome(const std::string& category,
                    double amount,
                    const std::string& note,
                    const std::string& date) {
    return addTransaction("income", category, amount, note, date);
}

int Core::addExpense(const std::string& category,
                     double amount,
                     const std::string& note,
                     const std::string& date) {
    return addTransaction("expense", category, amount, note, date);
}

std::vector<Transaction> Core::recentTransactions(int limit) const {
    return storage.listTransactions(limit);
}

bool Core::deleteTransaction(int id) {
    if (id <= 0) {
        throw std::invalid_argument("Transaction id must be positive");
    }

    return storage.deleteTransaction(id);
}

Summary Core::summary() const {
    Summary result;
    result.income = storage.totalForType("income");
    result.expense = storage.totalForType("expense");
    result.balance = result.income - result.expense;
    return result;
}

std::vector<CategoryTotal> Core::expenseReportByCategory() const {
    return storage.totalsByCategory("expense");
}

std::vector<CategoryTotal> Core::incomeReportByCategory() const {
    return storage.totalsByCategory("income");
}

int Core::addTransaction(const std::string& type,
                         const std::string& category,
                         double amount,
                         const std::string& note,
                         const std::string& date) {
    std::string normalizedDate = normalizeDate(date);
    validateTransaction(category, amount, normalizedDate);

    return storage.addTransaction(type, category, amount, note, normalizedDate);
}

void Core::validateTransaction(const std::string& category,
                               double amount,
                               const std::string& date) {
    if (category.empty()) {
        throw std::invalid_argument("Category cannot be empty");
    }

    if (amount <= 0) {
        throw std::invalid_argument("Amount must be greater than zero");
    }

    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        throw std::invalid_argument("Date must use YYYY-MM-DD format");
    }
}

std::string Core::normalizeDate(const std::string& date) {
    if (!date.empty()) {
        return date;
    }

    return today();
}

std::string Core::today() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
#if defined(_WIN32)
    localtime_s(&localTime, &currentTime);
#else
    localtime_r(&currentTime, &localTime);
#endif

    std::ostringstream output;
    output << std::put_time(&localTime, "%Y-%m-%d");
    return output.str();
}
