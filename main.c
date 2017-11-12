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
enum opcode{add, sub, mul, addi, beq, lw, sw};
int lwresult;//temp that store lw result


///////////////////Structs/////////////////////////
typedef struct {
    enum opcode op;//op code
    int dest;//destination register
    int s1;// source 1, piority, $s, $s1
    int s2;// source 2, Or $t!
    int im;//immediate value
    int mtime;
}inst;  // instruction set

typedef struct {
    int validBit;//0 if latch not ready, 1 if ready, reset after use
    int PC;//where is PC now
    inst operation;//op that needs to be passed
    int EXresult;//ex stage result in to store or do stuff
}latch;//latch between stages

/////////////////////Flags/////////////////////////

int c;//how many cycle MEM need, give by I/P
inst* iMem;
int* reg;
int* dMem;



int IFflag;//IF ready?
int IDflag;//ID ready?
int EXflag;//EX ready?
int MEMflag;//MEM ready?(1 ready for next inst)
int WBflag;//WB ready?
int Branchflag;//beq flag


////////////////////stage counters/////////////////////

int Mtime;//where is MEM at

////////////////////Registers///////////////////////////


/**********************InputProcess************************/
/*
char *progScanner(FILE *ipf, char * istbuff ){
    while (fgets(istbuff, 75, ipf))
        printf("String input is %s \n", istbuff);
}

char *regNumberConverter(...);
struct inst parser(...);
char *progScanner(...);
char *regNumberConverter(...);
struct inst parser(...);
*/

/********************Stages************************/


//void IF(...){}
//void ID(...){}
//void EX(...){}
//void MEM(...){} <-----------Todd
//void WB(...){}

/*****************Support Functions***************/

int LW(int address){
    return dMem[address];
}

void SW(int saveAddress,int data){
    dMem[saveAddress]=data;
    return;
}



////********************Todd**********************////

void IF(inst ifOp){
    if(ifOp.op==beq){//if brach, then no op before this pass EX
        Branchflag=1;//branch is here
    }
    return;
}






/* if not LW or SW, then just skip MEM and move on to
 * next stage, if LW or SW then wait c cycles;
 */
void MEM(inst memOp){
    memOp.mtime++;
    if(memOp.op==lw){//if LW, 0b100011
        int accessAdd=reg[memOp.s1]+memOp.im;//accessing address
        lwresult=LW(accessAdd);//reading data to dest

    }else if(memOp.op==sw){//if SW, 0b101011
        int accessAdd=reg[memOp.s1]+memOp.im;//accessing address
        SW(accessAdd,reg[memOp.s2]);

    }else{//NOT LW or SW, just next stage
        //nothing
    }


    return;//dummy return
}


void WB(inst wbOp, int data){
    if((wbOp.op==add)||(wbOp.op==sub)||(wbOp.op==mul)){//storing to $d(dest)
        reg[wbOp.dest]=data;
    }
    else if((wbOp.op==addi)||(wbOp.op==lw)){//storing to $t(s2)
        reg[wbOp.s2]=data;
    }
    return;
}




/********************Main************************/
int main() {

    dMem=(int *)malloc(sizeof(int)*512);
    iMem=(inst *)malloc(512* sizeof(inst));
    reg=(int *)malloc(32* sizeof(int));
    reg[0]=0;//cant change

    int MEMPC;//MEM PC pointer
    int WBPC;//WB PC pointer
    int MEMexResult;//MEM EX result
    int WBvalue;
    int IFPC=0;//IF PC pointer

    Mtime=0;

    latch MEMlatch;//latch to MEM
    latch WBlatch;//latch to WB
    latch NMWBlatch;//for those who are not MEM op
    latch IFIDlatch;//latch to ID

    inst MEMinst;
    inst WBinst;
    inst IFinst;
    inst realMEM[c];//true mem op array
    int MEMempty=0;//empty slot pointer
    int MEMtop=0;
/*******************Testing use variables***************************/
    int foo=1;//loop check
    c=4;


///////////////////////////////////////////
    while(foo) {//test while loop

        /*
         * WB stage
         */
        if (WBlatch.validBit) {//if valid, download info
            WBinst = MEMlatch.operation;//passing inst
            WBlatch.validBit = 0;//reset valid
            WBPC = MEMlatch.PC;
            WBvalue = MEMlatch.EXresult;
        }
        WB(WBinst, WBvalue);//store and this is the end of the inst



        /*
         * MEM stage
         */
        if (MEMlatch.validBit) {//if valid, download info
            MEMinst = MEMlatch.operation;//passing inst
            MEMlatch.validBit = 0;//reset valid
            MEMPC = MEMlatch.PC;
            MEMexResult = MEMlatch.EXresult;
            MEMinst.mtime = 0;//initialize mtime
        }
        realMEM[MEMempty] = MEMinst;//insert into the array
        MEMempty++;
        if (MEMempty = c) { MEMempty = 0; }//reset

        for (int i = 0; i < c; i++) {//for each
            if (realMEM[i].op != 999) {//if op valid
                MEM(realMEM[i]);
                if ((realMEM[i].mtime == c) && !(WBlatch.validBit)) {//if this inst is finished, then pass on
                    MEMflag = 1;
                    WBlatch.operation = MEMinst;
                    WBlatch.EXresult = MEMexResult;
                    WBlatch.PC = MEMPC;
                    WBlatch.validBit = 1;
                    realMEM[i].op = 999;//invalid this element
                    MEMtop++;
                    if (MEMtop = c) { MEMtop = 0; }//reset memtop
                }
            }

        }

        /*
         *
         *
         * All the other stages
         *
         *
         *
         */


        /*
         * IF stage
         */
        if (!Branchflag){//if branchflag is not set, then read next
            IFinst = iMem[IFPC];
            IFIDlatch.validBit=1;
            IFIDlatch.PC=IFPC;
            IFIDlatch.operation=IFinst;
            IFPC++;
        }else{//if branchflag is set, nop
            IFIDlatch.validBit=0;//just in case, disable this latch
        }

        IF(IFinst);//this will set the branchflag


    }

}