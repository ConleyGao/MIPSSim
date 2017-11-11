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
    int mtime=0;
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
    memOp.mtime++;
    if(memOp.op==0b100011){//if LW, 0b100011
        int accessAdd=memOp.s1+memOp.im;//accessing address
        memOp.dest=LW(accessAdd);//reading data to dest

    }else if(memOp.op==0b101011){//if SW, 0b101011
        int accessAdd=memOp.s1+memOp.im;//accessing address
        SW(accessAdd,memOp.s2);

    }else{//NOT LW or SW, just next stage
       printf("I shouldn't be here, MEM stage error, MEMinst is not LW or SW");//throw error
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
    latch NMWBlatch;//for those who are not MEM op

    inst MEMinst;
    inst WBinst;
    inst realMEM[c];//true mem op array
    int MEMempty=0;//empty slot pointer
    int MEMtop=0;
///////////////////////////////////////////
    while(foo) {//test while loop

        /*
         * MEM stage
         */
        if (MEMlatch.validBit) {//if valid, download info
            MEMinst = MEMlatch.operation;//passing inst
            MEMlatch.validBit = 0;//reset valid
            MEMPC=MEMlatch.PC;
            MEMexResult=MEMlatch.EXresult;
        }
        if(!((MEMinst.op==0b101011)||(MEMinst.op==0b100011))){//NOT LW or SW, just move on
            /***********seting NOMEM latch************/
            MEMflag=1;
            NMWBlatch.operation=MEMinst;
            NMWBlatch.EXresult=MEMexResult;
            NMWBlatch.PC=MEMPC;
            NMWBlatch.validBit=1;
        }else{
            realMEM[MEMempty]=MEMinst;//insert into the real mem arrya
            MEMempty++;
            if(MEMempty=c){MEMempty=0;}//reset
        }
        for(int i=0; i < c; i++){//for each
          if(realMEM[i].op!=0){//if op valid
            MEM(realMEM[i]);
              if((realMEM[i].mtime==c)&&!(WBlatch.validBit)){//if this inst is finished, then pass on
                  MEMflag=1;
                  WBlatch.operation=MEMinst;
                  WBlatch.EXresult=MEMexResult;
                  WBlatch.PC=MEMPC;
                  WBlatch.validBit=1;
                  realMEM[i].op=0;//invalid this element
                  MEMtop++;
                  if(MEMtop=c){MEMtop=0;}//reset memtop
              }
          }

        }




    }

}