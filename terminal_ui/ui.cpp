#include "ui.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>

UI::UI(Core& core) : core(core) {
}

void UI::run() {
    bool running = true;

    while (running) {
        showMenu();
        int choice = readInt("Choose action: ");

        try {
            switch (choice) {
                case 1:
                    addExpense();
                    break;
                case 2:
                    addIncome();
                    break;
                case 3:
                    listTransactions();
                    break;
                case 4:
                    deleteTransaction();
                    break;
                case 5:
                    showSummary();
                    break;
                case 6:
                    showReports();
                    break;
                case 0:
                    running = false;
                    break;
                default:
                    std::cout << "Unknown action.\n";
                    break;
            }
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }
}

void UI::showMenu() const {
    std::cout << "\nExpense Counter\n";
    std::cout << "1. Add expense\n";
    std::cout << "2. Add income\n";
    std::cout << "3. Show recent transactions\n";
    std::cout << "4. Delete transaction\n";
    std::cout << "5. Show summary\n";
    std::cout << "6. Show reports by category\n";
    std::cout << "0. Exit\n";
}

void UI::addIncome() {
    addTransaction(true);
}

void UI::addExpense() {
    addTransaction(false);
}

void UI::listTransactions() const {
    int limit = readInt("Limit: ");
    if (limit <= 0) {
        limit = 50;
    }

    printTransactions(core.recentTransactions(limit));
}

void UI::deleteTransaction() {
    int id = readInt("Transaction id: ");
    if (core.deleteTransaction(id)) {
        std::cout << "Transaction deleted.\n";
    } else {
        std::cout << "Transaction not found.\n";
    }
}

void UI::showSummary() const {
    Summary result = core.summary();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Income:  " << result.income << "\n";
    std::cout << "Expense: " << result.expense << "\n";
    std::cout << "Balance: " << result.balance << "\n";
}

void UI::showReports() const {
    std::cout << "\nExpenses by category\n";
    printCategoryTotals(core.expenseReportByCategory());

    std::cout << "\nIncome by category\n";
    printCategoryTotals(core.incomeReportByCategory());
}

void UI::addTransaction(bool income) {
    std::string category = readLine("Category: ");
    double amount = readAmount("Amount: ");
    std::string note = readLine("Note: ");
    std::string date = readLine("Date (YYYY-MM-DD, empty for today): ");

    int id = income
        ? core.addIncome(category, amount, note, date)
        : core.addExpense(category, amount, note, date);

    std::cout << "Saved with id " << id << ".\n";
}

void UI::printTransactions(const std::vector<Transaction>& transactions) const {
    if (transactions.empty()) {
        std::cout << "No transactions found.\n";
        return;
    }

    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(12) << "Date"
              << std::setw(10) << "Type"
              << std::setw(18) << "Category"
              << std::right << std::setw(12) << "Amount"
              << "  Note\n";

    std::cout << std::fixed << std::setprecision(2);
    for (const Transaction& transaction : transactions) {
        std::cout << std::left
                  << std::setw(5) << transaction.id
                  << std::setw(12) << transaction.date
                  << std::setw(10) << transaction.type
                  << std::setw(18) << transaction.category
                  << std::right << std::setw(12) << transaction.amount
                  << "  " << transaction.note << "\n";
    }
}

void UI::printCategoryTotals(const std::vector<CategoryTotal>& totals) const {
    if (totals.empty()) {
        std::cout << "No data.\n";
        return;
    }

    std::cout << std::fixed << std::setprecision(2);
    for (const CategoryTotal& total : totals) {
        std::cout << std::left << std::setw(20) << total.category
                  << std::right << std::setw(12) << total.total << "\n";
    }
}

std::string UI::readLine(const std::string& prompt) {
    std::cout << prompt;

    std::string value;
    std::getline(std::cin, value);
    return value;
}

double UI::readAmount(const std::string& prompt) {
    while (true) {
        std::string value = readLine(prompt);

        try {
            size_t parsed = 0;
            double amount = std::stod(value, &parsed);
            if (parsed == value.size()) {
                return amount;
            }
        } catch (const std::exception&) {
        }

        std::cout << "Enter a valid number.\n";
    }
}

int UI::readInt(const std::string& prompt) {
    while (true) {
        std::string value = readLine(prompt);

        try {
            size_t parsed = 0;
            int number = std::stoi(value, &parsed);
            if (parsed == value.size()) {
                return number;
            }
        } catch (const std::exception&) {
        }

        std::cout << "Enter a valid integer.\n";
    }
}
