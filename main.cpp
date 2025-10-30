#include <iostream>

#include "optional.h"

int main() {
    gx::Optional<int> opt = 42;
    if (!opt.HasValue()) {
        std::cout << "Optional is empty" << std::endl;
    } else {
        std::cout << "Optional has value: " << *opt << std::endl;
    }

    return 0;
}