#include <iostream>

using namespace std;

class box {
    public:
        box() : value(0) {};
        box(const int value) : value(value) {};

        int double_value();

        int value;
};