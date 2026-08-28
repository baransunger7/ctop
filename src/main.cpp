#include "sysmon/application.hpp"
#include <iostream>

int main() {
    try {
        Application app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Kritik Hata: " << e.what() << '\n';
        return 1;
    }
    return 0;
}