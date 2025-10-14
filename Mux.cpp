#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "MUX.h"
using namespace std;

int32_t MUX::select(bool sel, int32_t input0, int32_t input1)
{
    return sel ? input1 : input0;
}
