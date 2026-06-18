# Expense Counter

Expense Counter is a small terminal application for tracking personal income and expenses. It stores data in a local SQLite database file and can show recent transactions, balance, and category totals.

## Features

- Add income and expense transactions.
- Store category, amount, note, and date.
- List recent transactions.
- Delete transactions by id.
- Show total income, total expenses, and balance.
- Show income and expenses grouped by category.
- Create the SQLite schema automatically on first run.

## Requirements

SQLite is vendored in `thirdparty/sqlite3`, so no separate SQLite package is required.

Install a C++ compiler and CMake:

```bash
sudo apt update
sudo apt install build-essential cmake
```

Optional, for Ninja builds:

```bash
sudo apt install ninja-build
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/ExpenseCounter
```

The app creates `expenses.db` in the current working directory. Database files are ignored by git because they contain local personal data.

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Project Structure

```text
core/         Business logic and validation
storage/      SQLite persistence layer
terminal_ui/  Interactive terminal interface
thirdparty/   Vendored SQLite source
tests/        Basic storage/core regression tests
```
