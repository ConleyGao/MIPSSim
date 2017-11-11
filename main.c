// List the full names of ALL group members at the top of your code.
// Group Members: Gongtao Yang,
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
//feel free to add here any additional library names you may need
#define SINGLE 1
#define BATCH 0
#define REG_NUM 32

/**********Global variables **********************/
///////////////////Structs/////////////////////////
typedef struct {
    int op;//op code
    int dest;//destination register
    int s1;// source 1
    int s2;// source 2
    int im;//immediate value

}inst;  // instruction set

typedef struct {
    int validBit;//0 if latch not ready, 1 if ready, reset after use
    int PC;//where is PC now 
    inst operation;//op that needs to be passed
    int EXresult;//ex stage result in to store or do stuff
}latch;//latch between stages

/////////////////////Flags/////////////////////////

int memTime;//how many cycle MEM need, give by I/P



int IFflag;//IF ready?
int IDflag;//ID ready?
int EXflag;//EX ready?
int MEMflag;//MEM ready?(1 ready for next inst)
int WBflag;//WB ready?






/**********************InputProcess************************/

char *progScanner(FILE *ipf, char * istbuff ){
    while (fgets(istbuff, 75, ipf))
        printf("String input is %s \n", istbuff);
}

char *regNumberConverter(...)
struct inst parser(...)
char *progScanner(...);
char *regNumberConverter(...);
struct inst parser(...);


/********************Stages************************/


void IF(...){}
void ID(...){}
void EX(...){}
//void MEM(...){} <-----------Todd
void WB(...){}






////********************Todd**********************////
void MEM(){



    return;//dummy return
}







/********************Main************************/
int main() {
//TODO

}