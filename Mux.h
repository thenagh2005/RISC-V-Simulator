#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

class Mux {
    public:
        int32_t select(bool sel, int32_t input0, int32_t input1) {
            return sel ? input1 : input0;
        }
};