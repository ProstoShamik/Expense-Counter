#include "core.h"
#include "storage.h"
#include "ui.h"

#include <exception>
#include <iostream>

int main() {
    try {
        Storage storage("expenses.db");
        Core core(storage);
        UI ui(core);
        ui.run();
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
