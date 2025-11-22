#include "core.h"

#include <iostream>

Core::Core(Storage& storage) : storage(storage) {
    std::cout << "Core created" << std::endl;
}

Core::~Core() {
    std::cout << "Core destroed" << std::endl;
}