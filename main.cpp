#include <iostream>
#include <vector>
#include <chrono>

#include "model.h"


int main(int argc, char* argv[]) {
    Model formula(argv[1]);
    formula.summary();
    formula.decimate();

    return 0;
}
