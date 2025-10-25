#pragma once

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

//Multiplexer class

class MUX
{
public:
    int32_t select(bool sel, int32_t input0, int32_t input1); //Function to implement MUX. Selects input based on select boolean variable.
};