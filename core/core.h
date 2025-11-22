#ifndef CORE_H
#define CORE_H

#include "storage.h"

class Core {
public:
    Core(Storage& storage);
    ~Core();
private:
    Storage& storage;
};

#endif
