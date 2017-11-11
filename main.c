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
    int s1;// source 1, piority, $s, $s1
    int s2;// source 2, Or $t!
    int im;//immediate value

}inst;  // instruction set

typedef struct {
    int validBit;//0 if latch not ready, 1 if ready, reset after use
    int PC;//where is PC now
    inst operation;//op that needs to be passed
    int EXresult;//ex stage result in to store or do stuff
}latch;//latch between stages

/////////////////////Flags/////////////////////////

int c;//how many cycle MEM need, give by I/P



int IFflag;//IF ready?
int IDflag;//ID ready?
int EXflag;//EX ready?
int MEMflag;//MEM ready?(1 ready for next inst)
int WBflag;//WB ready?


////////////////////stage counters/////////////////////

int Mtime;//where is MEM at


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

/*****************Support Functions***************/

int LW(int address){
   //TODO
    return -1;
}

void SW(int saveAddress,int data){
    //TODO
    return;
}



////********************Todd**********************////
/* if not LW or SW, then just skip MEM and move on to
 * next stage, if LW or SW then wait c cycles;
 */
void MEM(inst memOp){
    Mtime++;
    if(memOp.op==0b100011){//if LW, 0b100011
        int accessAdd=memOp.s1+memOp.im;//accessing address
        memOp.dest=LW(accessAdd);//reading data to dest
        if(Mtime==c){//time's UP
            Mtime=0;
            MEMflag=1;
        }
    }else if(memOp.op==0b101011){//if SW, 0b101011
        int accessAdd=memOp.s1+memOp.im;//accessing address
        SW(accessAdd,memOp.s2);
        if(Mtime==c){//time's UP
            Mtime=0;
            MEMflag=1;
        }
    }else{//NOT LW or SW, just next stage
        Mtime=0;
        MEMflag=1;//ready!
    }


    return;//dummy return
}







/********************Main************************/
int main() {

    int foo=1;//loop check
    int MEMPC;//MEM PC store
    int MEMexResult;//MEM EX result

    Mtime=0;

    latch MEMlatch;//latch to MEM
    latch WBlatch;//latch to WB


    inst MEMinst;
    inst WBinst;
///////////////////////////////////////////
    while(foo) {

        /*
         * MEM stage
         */
        if (MEMlatch.validBit) {//if valid, download info
            MEMinst = MEMlatch.operation;//passing inst
            MEMlatch.validBit = 0;//reset valid
            MEMPC=MEMlatch.PC;
            MEMexResult=MEMlatch.EXresult;
        }
        MEM(MEMinst);
        if(MEMflag && !(WBlatch.validBit)){//if MEM finished & WBlatch ready
            MEMflag=0;
            WBlatch.operation=MEMinst;
            WBlatch.EXresult=MEMexResult;
            WBlatch.PC=MEMPC;
            WBlatch.validBit=1;
        }



    }

}