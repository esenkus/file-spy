#include <iostream>
#include "domain/Observer.hpp"

int main() {
    std::cout << "Hello, World!" << std::endl;

    Observer observer("/Users/eivydassenkus/Desktop/test");
    observer.start();
    observer.stop();
    return 0;
}
