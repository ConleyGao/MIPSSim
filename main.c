#include <stdio.h>

/**********Global variables **********************/
typedef struct {
    int op;//op code
    int dest;//destination register
    int s1;// source 1
    int s2;// source 2
    int im;//immediate value

}inst;  // instruction set

typedef struct {
    int validBit;//0 if latch not ready, 1 if ready, reset after use
    inst operation;//op that needs to be passed
    int EXresult;//ex stage result in to store or do stuff
}latch;//latch between stages
/**********************InputProcess************************/

char *progScanner(...);
char *regNumberConverter(...);
struct inst parser(...);


/********************Stages************************/


void IF(...);
void ID(...);
void EX(...);
void MEM(...);
void WB(...);

/********************Main************************/
int main() {
//TODO

}