// List the full names of ALL group members at the top of your code.
// Group Members: Gongtao Yang, Kai Qian, Conley Gao
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

int lwresult;//temp that store lw result


enum opcode{add, sub, mul, addi, beq, lw, sw, haltSimulation,nop};
typedef struct{
    enum opcode op;
    int dest;
    int s1;
    int s2;
    int im;
} inst;


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

latch ifid = { .validBit = 0 };
latch idex = { .validBit = 0 };
latch exdata = { .validBit = 0};
latch memdata = { .validBit = 0};
latch MEMlatch;//latch to MEM
latch WBlatch;//latch to WB

/////////////////////Flags/////////////////////////

int c;//how many cycle MEM need, give by I/P
inst* iMem;
int* reg;
int* dMem;

const latch tempLatch = (latch) {0, 0, {nop, 0, 0, 0, 0,}, 0};
int EXcycles;
int EXdelay;
int IDcycles = 0;

int IFflag;//IF ready?
int IDflag;//ID ready?
int EXflag;//EX ready?
int MEMflag;//MEM ready?(1 ready for next inst)
int WBflag;//WB ready?
int Branchflag;//beq flag
int ifUtil;
int idUtil;
int exUtil;
int memUtil;
int wbUtil;
int EOIflag;
int c, m, n;





////////////////////Registers///////////////////////////


/**********************InputProcess************************/
/*
char *progScanner(FILE *ipf, char * istbuff ){
    while (fgets(istbuff, 75, ipf))
        printf("String input is %s \n", istbuff);
/**********************InputProcess************************/
//check matching "()"
int Pcheck ( char * ist){
    int lcount =0;
    int rcount =0;
    char *tmp1 = ist;
    char * tmp2 = ist;
    //count "("
    while(tmp1 = strstr(tmp1, "("))
    {
        lcount++;
        tmp1++;
    }
    //count ")"
    while(tmp2 = strstr(tmp2, ")"))
    {
        rcount++;
        tmp2++;
    }
    if (lcount == rcount) return 1;
    else return 0;
}
//get lines from intputfile, check parentheses , remove extra space and ,
//return a parsed string
char *progScanner(FILE *ipf, char* instruction){//,char *ist ){
    char ist[100];  //= (char *) malloc(100*sizeof(char));
    char *token;
    *instruction = '\0';
    if ( fgets(ist,100,ipf)!=NULL){
        if(!(Pcheck(ist))) {
            printf("Mismatch parenthese exiting ");
            exit(1);
        }
        printf("String input is %s \n", ist);

        // ///////////////remove spaces and comma
        /* get the first token */
        token = strtok(ist, ", \t\r\n()");
        strcpy(instruction,token);
        token = strtok(NULL, ", \t\r\n()");
        /* walk through other tokens */
        while( token != NULL ) {
            strcat(instruction, " ");
            strcat(instruction, token);
            printf( "----- %s\n", instruction );

            token = strtok(NULL,", \t\r\n()" );
        }
        /////////////////
        return instruction;
    }
    else return NULL;

}
//take a parsed string from progScanner and replace $registor with $number
// check if registor is valid , return string with converted reg number
char *regNumberConverter(char * instruction){
    char *token;
    char *ist = (char *) malloc(100*sizeof(char));
    strcpy(ist, instruction);
    *instruction ='\0';
    token = strtok(ist, " \r\n");
    //*instruction = '\0';
    /* walk through other tokens */
    while( token != NULL ) {
        if((strcmp(token,"$zero")==0)||(strcmp(token,"$0")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$0");
        }else if((strcmp(token,"$at")==0)||(strcmp(token,"$1")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$1");
        }else if((strcmp(token,"$v0")==0)||(strcmp(token,"$2")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$2");
        }else if((strcmp(token,"$v1")==0)||(strcmp(token,"$3")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$3");
        }else if((strcmp(token,"$a0")==0)||(strcmp(token,"$4")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$4");
        }else if((strcmp(token,"$a1")==0)||(strcmp(token,"$5")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$5");
        }else if((strcmp(token,"$a2")==0)||(strcmp(token,"$6")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$6");
        }else if((strcmp(token,"$a3")==0)||(strcmp(token,"$7")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$7");
        }else if((strcmp(token,"$t0")==0)||(strcmp(token,"$8")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$8");
        }else if((strcmp(token,"$t1")==0)||(strcmp(token,"$9")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$9");
        }else if((strcmp(token,"$t2")==0)||(strcmp(token,"$10")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$10");
        }else if((strcmp(token,"$t3")==0)||(strcmp(token,"$11")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$11");
        }else if((strcmp(token,"$t4")==0)||(strcmp(token,"$12")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$12");
        }else if((strcmp(token,"$t5")==0)||(strcmp(token,"$13")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$13");
        }else if((strcmp(token,"$t6")==0)||(strcmp(token,"$14")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$14");
        }else if((strcmp(token,"$t7")==0)||(strcmp(token,"$15")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$15");
        }else if((strcmp(token,"$s0")==0)||(strcmp(token,"$16")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$16");
        }else if((strcmp(token,"$s1")==0L)||(strcmp(token,"$17")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$17");
        }else if((strcmp(token,"$s2")==0)||(strcmp(token,"$18")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$18");
        }else if((strcmp(token,"$s3")==0)||(strcmp(token,"$19")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$19");
        }else if((strcmp(token,"$s4")==0)||(strcmp(token,"$20")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$20");
        }else if((strcmp(token,"$s5")==0)||(strcmp(token,"$21")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$21");
        }else if((strcmp(token,"$s6")==0)||(strcmp(token,"$22")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$22");
        }else if((strcmp(token,"$s7")==0)||(strcmp(token,"$23")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$23");
        }else if((strcmp(token,"$t8")==0)||(strcmp(token,"$24")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$24");
        }else if((strcmp(token,"$t9")==0)||(strcmp(token,"$25")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$25");
        }else if((strcmp(token,"$k0")==0)||(strcmp(token,"$26")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$26");
        }else if((strcmp(token,"$k1")==0)||(strcmp(token,"$27")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$27");
        }else if((strcmp(token,"$gp")==0)||(strcmp(token,"$28")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$28");
        }else if((strcmp(token,"$sp")==0)||(strcmp(token,"$29")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$29");
        }else if((strcmp(token,"$fp")==0)||(strcmp(token,"$30")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$30");
        }else if((strcmp(token,"$ra")==0)||(strcmp(token,"$31")==0)){
            strcat(instruction, " ");
            strcat(instruction, "$31");
        }else if(atoi(token)>31){
            if (strstr(token,"$")!=NULL) {
                printf("Register out of bounds, %d is higher than 31 \n", atoi(token));
                exit(1);
            }
            else {
                strcat(instruction, " ");
                strcat(instruction,token);
            }
        }else {
            strcat(instruction, " ");
            strcat(instruction,token);
        }


        token = strtok(NULL," \r\n" );
    }
    free(ist);
    return instruction;
}
// check format , make a structure based on type of format , return it
inst parser(char *instruction){
    inst ninst ={add,0,0,0,0};
    char *type = "?";
    char *op = (char *) malloc(100*sizeof(char));
    char *s1 = (char *) malloc(100*sizeof(char));
    char *s2 = (char *) malloc(100*sizeof(char));
    char *s3 = (char *) malloc(100*sizeof(char));
    op =strtok (instruction, " ");
    s1 =strtok (NULL, " ");
    s2 =strtok (NULL, " ");
    s3 =strtok (NULL, " \r\n");
// check format
    if(strcmp(op,"add")==0){
        //trcpy(instruction,"add");
        type ="R";
        ninst.op = add;
    }else if(strcmp(op,"sub")==0){
        //strcpy(instruction,"sub");
        type ="R";
        ninst.op = sub;
    }else if(strcmp(op,"mul")==0){
        //strcpy(instruction,"mul");
        type ="R";
        ninst.op = mul;
    }else if(strcmp(op,"addi")==0){
        //strcpy(instruction,"addi");
        type ="I";
        ninst.op = addi;
    }else if(strcmp(op,"beq")==0){
        // strcpy(instruction,"beq");
        type ="I";
        ninst.op = beq;
    }else if(strcmp(op,"lw")==0){
        type ="M";
        // strcpy(instruction,"lw");
        ninst.op = lw;
    }else if(strcmp(op,"sw")==0){
        type ="M";
        //strcpy(instruction,"sw");
        ninst.op = sw;
    }else if(strcmp(op,"haltSimulation")==0) {
        type ="S";
        //strcpy(instruction,"haltSimulation");
        ninst.op = haltSimulation;
    }
// create struct base on type
    switch(*type){

        case '?':
            printf("Illegal opcode %s\n",op);
            exit(1);
        case 'R':
            if (strstr(s1,"$")==NULL||strstr(s2,"$")==NULL||strstr(s3,"$")==NULL){  // check registor has $
                printf("register no $");
                exit(1);
            }
            ninst.dest = strtol(strtok(s1,"$"),NULL,10);
            ninst.s1 = strtol(strtok(s2,"$"),NULL,10);
            ninst.s2 = strtol(strtok(s3,"$"),NULL,10);
            return ninst;
        case 'I':
            if (strstr(s1,"$")==NULL||strstr(s2,"$")==NULL){// check registor has $
                printf("register no $\n");
                exit(1);
            }
            else if(atoi(s3)>=65536){                        //check valid im
                printf("invalid im\n");
                exit(1);
            }
            ninst.s1 = strtol(strtok(s2,"$"),NULL,10);
            ninst.s2 = strtol(strtok(s1,"$"),NULL,10);
            ninst.im = strtol(strtok(s3,"$"),NULL,10);
            return ninst;
        case 'M':
            if (strstr(s1,"$")==NULL||strstr(s3,"$")==NULL){// check registor has $
                printf("register no $\n");
                exit(1);
            }
            else if(atoi(s2)%4!=0||atoi(s2)>=65536){//check valid im and proper memory address
                printf("invalid im\n");
                exit(1);
            }
            ninst.s1 = strtol(strtok(s3,"$"),NULL,10);
            ninst.s2 = strtol(strtok(s1,"$"),NULL,10);
            ninst.im = strtol(strtok(s2,"$"),NULL,10 );

            return ninst;
        case 'S':

            return ninst;

    }
    free(op);free(s1);free(s2);free(s3);
}


/********************Stages************************/

int regCheck(int sReg){									//sReg is the register number that is being checked
    if(MEMlatch.validBit == 1){							//This only checks against registers that are in valid latches
        switch(MEMlatch.operation.op){					//Needs to check against different registers for different instructions
            case 0:										//add, sub, mul all produce to the d register
            case 2:
            case 3:
                if(MEMlatch.operation.dest == sReg) return 1;
                break;
            case 1:										//addi and lw produce to the s2 register
            case 5:
                if(MEMlatch.operation.s2 == sReg) return 1;
                break;
            case 4:										//beq, sw and halt produce nothing
            case 6:
            case 7:
                break;
            default:
                printf("Op Code not recognized: EX_MEM Latch \n");
                exit(1);								//This should never be reached
        }
    }
    if(WBlatch.validBit == 1){
        switch(WBlatch.operation.op){					//Needs to check against different registers for different instructions
            case 0:										//add, sub, mul all produce to the d register
            case 2:
            case 3:
                if(WBlatch.operation.dest == sReg) return 1;
                break;
            case 1:										//addi and lw produce to the s2 register
            case 5:
                if(WBlatch.operation.s2 == sReg) return 1;
                break;
            case 4:										//beq, sw and halt produce nothing
            case 6:
            case 7:
                break;
            default:
                printf("Op Code not recognized: MEM_WB Latch \n");
                exit(1);								//This should never be reached
        }
    }
    return 0;											//If it never catches, the register is fine
}
void ID(struct inst){
    if(idex.operation.op == 7){						//If halt has passed through here, it just auto returns
        return;
    }
    switch(ifid.operation.op){						//Instruction cases are grouped for efficiency because certain instructions have the same actions
        case 0:										//add
        case 2:										//sub
            if(regCheck(ifid.operation.s2) == 1) return;	//Checks s2 for add and sub, these two instructions have the same ID step
            idex =ifid;
            idex.operation.s1 = reg[ifid.operation.s1];	//loads s1 into the next latch
            idex.operation.s2 = reg[ifid.operation.s2];	//loads s2 into the next latch
            EXdelay = n;							//Set the EXdelay for normal operation delay
            break;

        case 3:										//mul
            if(regCheck(ifid.operation.s2) == 1) return;	//Checks s2 for mul, it has a different ID step than add and sub
            idex = ifid;
            idex.operation.s1 = reg[ifid.operation.s1];	//loads s1 into the next latch
            idex.operation.s2 = reg[ifid.operation.s2];	//loads s2 into the next latch
            EXdelay = m;							//Set the EXdelay for multiplication delay (difference between add and sub for passing to EX)
            break;

        case 4:										//beq
            if(regCheck(ifid.operation.s2) == 1) return;	//Checks s2 of beq, it has a different ID step than the mul, add and sub
            //check that im value is in range for 16 bit 2's comp, +/- 32767 (2^15 - 1)
            if((ifid.operation.im < -32767)||(ifid.operation.im > 32767)){
                printf("Immediate field out of numerical bounds, %d \n", ifid.operation.im);	//Note we did not just use two assertions because we wanted to print an error message
                exit(1);							//Halt the code if this error message has been printed
            }
            idex = ifid;
            idex.operation.s1 = reg[ifid.operation.s1];	//loads s1 into the next latch
            idex.operation.s2 = reg[ifid.operation.s2];	//loads s2 into the next latch
            Branchflag = 1;						//If it reads a branch, mark branch pending
            EXdelay = n;							//Set the EXdelay for normal operation delay
            break;

        case 1:										//addi
        case 5:										//lw
            //check that im value is in range for 16 bit 2's comp, +/- 32767 (2^15 - 1)
            if((ifid.operation.im < -32767)||(ifid.operation.im > 32767)){
                printf("Immediate field out of numerical bounds, %d \n", ifid.operation.im);	//Note we did not just use two assertions because we wanted to print an error message
                exit(1);							//Halt the code if this error message has been printed
            }
            idex =ifid;
            idex.operation.s1 = reg[ifid.operation.s1];	//loads s1 into the next latch
            EXdelay = n;							//Set the EXdelay for normal operation delay
            break;

        case 6:										//sw
            if(regCheck(ifid.operation.s2) == 1) return;	//Checks s2 of sw, it relies on s2 while addi and lw do not
            //check that im value is in range for 16 bit 2's comp, +/- 32767 (2^15 - 1)
            if((ifid.operation.im < -32767)||(ifid.operation.im > 32767)){
                printf("Immediate field out of numerical bounds, %d \n", ifid.operation.im);	//Note we did not just use two assertions because we wanted to print an error message
                exit(1);							//Halt the code if this error message has been printed
            }
            idex = ifid;
            idex.operation.s1 = reg[ifid.operation.s1];	//loads s1 into the next latch
            idex.operation.s2 = reg[ifid.operation.s2];	//loads s2 into the next latch
            EXdelay = n;							//Set the EXdelay for normal operation delay
            break;

        case 7:										//halt
            idex = ifid;
            EXdelay = 1;							//Set the EXdelay to be 1 for fast propagation
            return;									//Return so we don't count as useful work or clear the latch

        default:
            printf("Op Code not recognized: ID Stage \n");
            exit(1);								//If it doesn't decode one of the 7 possibilities there is an error, should be caught by the parser, but this is a backup
            return;
    }

    ifid = tempLatch;
    idUtil ++;
}

void EX(){
    if(exdata.operation.op == 7){						//If halt has passed through here, it just auto returns
        return;
    }
    else if(idex.validBit == 1){						//Check previous latch to see if it can start it's operation
        if((EXdelay == 1)&&(exdata.validBit == 0)){ 	//Once the delay count down is finished and the latch is ready to receive, compute and push
            exdata = idex;
            switch(idex.operation.op){
                case 0:									//add
                    exdata.EXresult = idex.operation.s1 + idex.operation.s2;
                   
                    break;
                case 2:									//sub
                    exdata.EXresult = idex.operation.s1 - idex.operation.s2;
                    
                    break;
                    
                case 3:									//mul
                    exdata.EXresult = idex.operation.s1 * idex.operation.s2;	//Assumes that the system is operating at 32 bits so the result will be cast down to the right size
                    
                    break;
                case 4:
                    if(idex.operation.s1 == idex.operation.s2){
                        IFPC = idex.operation.im + idex.PC + 1;	//Advances pc by the immediate value past the pc of the next instruction
                    }
                    Branchflag = 2;					//Set to 2 so the IF stage knows not to unfreeze until the next cycle
                    break;
                case 1:									//addi
                case 5:									//lw
                case 6:									//sw
                    exdata.EXresult = idex.operation.s1 + idex.operation.im;	//Note these three all do the same thing in EX, the results are just handled differently
                    break;

                case 7:
                    return;
                default:
                    printf("Op Code not recognized: EX stage \n");
                    exit(1);
            }
            idex = tempLatch;
            exUtil++;
        }
        else if(EXdelay != 1){							//Note this is set by the previous stage, so it will never be changed until this stage clears the latch
            EXdelay -= 1;								//Counts down the delay for the EX stage
            exUtil ++;
            assert(EXdelay > -0); 						//Catch if this decreases too much
        }
    }
}

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
        ifUtil++;
        Branchflag=1;//branch is here
    }
    if(ifOp.op==haltSimulation){
        EOIflag=1;
    }
    return;
}






/* if not LW or SW, then just skip MEM and move on to
 * next stage, if LW or SW then wait c cycles;
 */
void MEM(inst memOp){
    memOp.mtime++;
    if(memOp.op==lw){//if LW, 0b100011
        memUtil++;
        int accessAdd=reg[memOp.s1]+memOp.im;//accessing address
        lwresult=LW(accessAdd);//reading data to dest

    }else if(memOp.op==sw){//if SW, 0b101011
        memUtil++;
        int accessAdd=reg[memOp.s1]+memOp.im;//accessing address
        SW(accessAdd,reg[memOp.s2]);

    }else{//NOT LW or SW, just next stage
        //nothing
    }


    return;//dummy return
}


void WB(inst wbOp, int data){

    if((wbOp.op==add)||(wbOp.op==sub)||(wbOp.op==mul)){//storing to $d(dest)
        wbUtil++;
        reg[wbOp.dest]=data;
    }
    else if((wbOp.op==addi)||(wbOp.op==lw)){//storing to $t(s2)
        reg[wbOp.s2]=data;
        wbUtil++;
    }
    return;
}




/********************Main************************/
int main (int argc, char *argv[]){
    int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
   
    int i;//for loop counter
    long mips_reg[REG_NUM];
    long pgm_c=0;//program counter
    long sim_cycle=0;//simulation cycle counter
    //define your own counter for the usage of each pipeline stage here

    int test_counter=0;
    FILE *input=NULL;
    FILE *output=NULL;
    printf("The arguments are:");

    for(i=1;i<argc;i++){
        printf("%s ",argv[i]);
    }
    printf("\n");
    if(argc==7){
        if(strcmp("-s",argv[1])==0){
            sim_mode=SINGLE;
        }
        else if(strcmp("-b",argv[1])==0){
            sim_mode=BATCH;
        }
        else{
            printf("Wrong sim mode chosen\n");
            exit(0);
        }

        m=atoi(argv[2]);
        n=atoi(argv[3]);
        c=atoi(argv[4]);
        input=fopen(argv[5],"r");
        output=fopen(argv[6],"w");

    }

    else{
        printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
        printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
        exit(0);
    }
    if(input==NULL){
        printf("Unable to open input or output file\n");
        exit(0);
    }
    if(output==NULL){
        printf("Cannot create output file\n");
        exit(0);
    }
    //initialize registers and program counter
    if(sim_mode==1){
        for (i=0;i<REG_NUM;i++){
            mips_reg[i]=0;
        }
    }

    //start your code from here
    dMem=(int *)malloc(sizeof(int)*512);
    iMem=(inst *)malloc(512* sizeof(inst));
    reg=(int *)malloc(32* sizeof(int));
//    reg[0]=0;//cant change

    int MEMPC;//MEM PC pointer
    int WBPC;//WB PC pointer
    int MEMexResult;//MEM EX result
    int WBvalue;
    int IFPC=0;//IF PC pointer

   

    latch MEMlatch;//latch to MEM
    latch WBlatch;//latch to WB
    latch NMWBlatch;//for those who are not MEM op
    latch IFIDlatch;//latch to ID

    c=6;
    inst MEMinst;
    inst WBinst;
    inst IFinst;
    inst realMEM[c];//true mem op array
    int MEMempty=0;//empty slot pointer
    int MEMtop=0;


    //store instruction to instruction memory
    iMem = malloc(512*sizeof(inst));
    char *instruction = (char *) malloc(100*sizeof(char));
    i = 0;
    while( iMem[i].op!=haltSimulation){
        progScanner(input,instruction);
        regNumberConverter(instruction);
        iMem[i] = parser(instruction);
        printf( "---- %d--%d--%d--%d--%d\n",iMem[i].op,iMem[i].dest,iMem[i].im,iMem[i].s1,iMem[i].s2);
        i++;
    }
    //////////// main loop
    int foo=1;//loop check
    c=4;
    ifUtil=0;
    idUtil=0;
    exUtil=0;
    memUtil=0;
    wbUtil=0;

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
        MEMlatch=memdata;
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
        if(!EOIflag){//if not end of instruction
            IFIDlatch.PC=IFPC;
            IFIDlatch.operation=IFinst;
            IFPC++;
        }else{
            IFIDlatch.validBit=0;//disable IFID latch
        }
        ifid=IFIDlatch;
        ifUtil++;



        /**********************end*****************************/
        if((IFIDlatch.validBit)||(idex.validBit)||(MEMlatch.validBit)||(WBlatch.validBit)||!(EOIflag)){
            if(WBinst.op==haltSimulation)
            foo=0;//iff all latches disabled, and eoi flag up, then end the loop
        }


        ////////////////////////////////////////////////////////
        /*********************code 2***************************/
        printf("cycle: %d ",sim_cycle);
        if(sim_mode==1){
            for (i=1;i<REG_NUM;i++){
                printf("%d  ",mips_reg[i]);
            }
        }
        printf("%d\n",pgm_c);
        pgm_c+=4;
        sim_cycle+=1;
        test_counter++;
        printf("press ENTER to continue\n");
        while(getchar() != '\n');
        if(sim_cycle=10){
            foo=0;
        }
    }

    //add the following code to the end of the simulation,
    //to output statistics in batch mode
    if(sim_mode==0){
        fprintf(output,"program name: %s\n",argv[5]);
        fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n",
                ifUtil, idUtil, exUtil, memUtil, wbUtil);
        // add the (double) stage_counter/sim_cycle for each
        // stage following sequence IF ID EX MEM WB

        fprintf(output,"register values ");
        for (i=1;i<REG_NUM;i++){
            fprintf(output,"%d  ",mips_reg[i]);
        }
        fprintf(output,"%d\n",pgm_c);

    }
    //close input and output files at the end of the simulation
    fclose(input);
    fclose(output);
    return 0;
}
