#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

class DMemory {
    public:
        DMemory();
        int loadWord(int address);
        void storeWord(int address, int value);
        void storeByte(int address, int value);
        int loadByte(int address); 

        int changeMode(int newMode) {
            mode = newMode; //0 for not active, -1 for read, 1 for write.
            return mode;
        }
        int getMode() const {
            return mode;
        }

        
    private:
        int dmemory[4096];
        int mode;
};