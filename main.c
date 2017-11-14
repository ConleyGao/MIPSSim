// List the full names of ALL group members at the top of your code.
// Group Members: Gongtao Yang, Kai Qian, Conley Gao
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
//feel free to add here any additional library names you may need
#define SINGLE 1
#define BATCH 0
#define REG_NUM 32

/**********Global variables **********************/
enum opcode {
    add, sub, mul, addi, beq, lw, sw, haltSimulation, nop
};


///////////////////Structs/////////////////////////
typedef struct {
    enum opcode op;//op code
    int dest;//destination register
    int s1;// source 1, piority, $s, $s1
    int s2;// source 2, Or $t!
    int im;//immediate value
    int mtime;
} inst;  // instruction set

typedef struct {
    int validBit;//0 if latch not ready, 1 if ready, reset after use
    int PC;//where is PC now
    inst operation;//op that needs to be passed

    int EXresult;//ex stage result in to store or do stuff
} latch;//latch between stages

latch IFIDlatch = {0, 0, {nop, 0, 0, 0, 0,}, 0};
latch IDEXlatch = {0, 0, {nop, 0, 0, 0, 0,}, 0};
latch EXMEMlatch = {0, 0, {nop, 0, 0, 0, 0,}, 0};
latch MEMWBlatch = {0, 0, {nop, 0, 0, 0, 0,}, 0};//latch to WB


/////////////////////Flags/////////////////////////

int c;//how many cycle MEM need, give by I/P
inst *iMem;

int mips_reg[REG_NUM];

int *dMem;

const latch tempLatch = (latch) {0, 0, {nop, 0, 0, 0, 0,}, 0};

int EXdelay;
int Branchflag;//beq flag
int ifUtil;
int idUtil;
int exUtil;
int memUtil;
int wbUtil;
int EOIflag;
int c, m, n;
int IFPC;
int Mdelay;
int IFdelay;

/**********************InputProcess************************/

int my_strcmp(const char *s1, const char *s2) {
    for (; *s1 == *s2; ++s1, ++s2)
        if (*s1 == 0)
            return 0;
    return *(unsigned char *) s1 < *(unsigned char *) s2 ? -1 : 1;
}

char *my_strcat(char *dest, const char *src) {
    size_t i, j;
    for (i = 0; dest[i] != '\0'; i++);
    for (j = 0; src[j] != '\0'; j++)
        dest[i + j] = src[j];
    dest[i + j] = '\0';
    return dest;
}

char *my_strcpy(char *s1, const char *s2) {
    char *s = s1;
    while ((*s++ = *s2++) != 0);
    return (s1);
}

char *my_strstr(string, substring)
        register char *string;    /* String to search. */
        char *substring;        /* Substring to try to find in string. */
{
    register char *a, *b;

    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = substring;
    if (*b == 0) {
        return string;
    }
    for (; *string != 0; string += 1) {
        if (*string != *b) {
            continue;
        }
        a = string;
        while (1) {
            if (*b == 0) {
                return string;
            }
            if (*a++ != *b++) {
                break;
            }
        }
        b = substring;
    }
    return NULL;
}

int Pcheck(char *ist) {
    int lcount = 0;
    int rcount = 0;
    char *tmp1 = ist;
    char *tmp2 = ist;
    //count "("
    while (tmp1 = my_strstr(tmp1, "(")) {
        lcount++;
        tmp1++;
    }
    //count ")"
    while (tmp2 = my_strstr(tmp2, ")")) {
        rcount++;
        tmp2++;
    }
    if (lcount == rcount) return 1;
    else return 0;
}

//get lines from intputfile, check parentheses , remove extra space and ,
//return a parsed string
char *progScanner(FILE *ipf, char *instruction) {//,char *ist ){
    char ist[100];  //= (char *) malloc(100*sizeof(char));
    char *token;
    *instruction = '\0';
    if (fgets(ist, 100, ipf) != NULL) {
        if (!(Pcheck(ist))) {
            printf("Mismatch parenthese exiting ");
            exit(1);
        }


        // ///////////////remove spaces and comma
        /* get the first token */
        token = strtok(ist, ", \t\r\n()");
        my_strcpy(instruction, token);
        token = strtok(NULL, ", \t\r\n()");
        /* walk through other tokens */
        while (token != NULL) {

            my_strcat(instruction, " ");
            my_strcat(instruction, token);
            // printf( "----- %s\n", instruction );


            token = strtok(NULL, ", \t\r\n()");
        }
        /////////////////
        return instruction;
    } else return NULL;

}

//take a parsed string from progScanner and replace $registor with $number
// check if registor is valid , return string with converted mips_reg number
char *regNumberConverter(char *instruction) {
    char *token;
    char *ist = (char *) malloc(100 * sizeof(char));
    my_strcpy(ist, instruction);
    *instruction = '\0';
    token = strtok(ist, " \r\n");
    //*instruction = '\0';
    /* walk through other tokens */
    while (token != NULL) {
        if ((my_strcmp(token, "$zero") == 0) || (my_strcmp(token, "$0") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$0");
        } else if ((my_strcmp(token, "$at") == 0) || (my_strcmp(token, "$1") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$1");
        } else if ((my_strcmp(token, "$v0") == 0) || (my_strcmp(token, "$2") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$2");
        } else if ((my_strcmp(token, "$v1") == 0) || (my_strcmp(token, "$3") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$3");
        } else if ((my_strcmp(token, "$a0") == 0) || (my_strcmp(token, "$4") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$4");
        } else if ((my_strcmp(token, "$a1") == 0) || (my_strcmp(token, "$5") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$5");
        } else if ((my_strcmp(token, "$a2") == 0) || (my_strcmp(token, "$6") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$6");
        } else if ((my_strcmp(token, "$a3") == 0) || (my_strcmp(token, "$7") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$7");
        } else if ((my_strcmp(token, "$t0") == 0) || (my_strcmp(token, "$8") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$8");
        } else if ((my_strcmp(token, "$t1") == 0) || (my_strcmp(token, "$9") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$9");
        } else if ((my_strcmp(token, "$t2") == 0) || (my_strcmp(token, "$10") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$10");
        } else if ((my_strcmp(token, "$t3") == 0) || (my_strcmp(token, "$11") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$11");
        } else if ((my_strcmp(token, "$t4") == 0) || (my_strcmp(token, "$12") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$12");
        } else if ((my_strcmp(token, "$t5") == 0) || (my_strcmp(token, "$13") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$13");
        } else if ((my_strcmp(token, "$t6") == 0) || (my_strcmp(token, "$14") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$14");
        } else if ((my_strcmp(token, "$t7") == 0) || (my_strcmp(token, "$15") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$15");
        } else if ((my_strcmp(token, "$s0") == 0) || (my_strcmp(token, "$16") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$16");
        } else if ((my_strcmp(token, "$s1") == 0L) || (my_strcmp(token, "$17") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$17");
        } else if ((my_strcmp(token, "$s2") == 0) || (my_strcmp(token, "$18") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$18");
        } else if ((my_strcmp(token, "$s3") == 0) || (my_strcmp(token, "$19") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$19");
        } else if ((my_strcmp(token, "$s4") == 0) || (my_strcmp(token, "$20") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$20");
        } else if ((my_strcmp(token, "$s5") == 0) || (my_strcmp(token, "$21") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$21");
        } else if ((my_strcmp(token, "$s6") == 0) || (my_strcmp(token, "$22") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$22");
        } else if ((my_strcmp(token, "$s7") == 0) || (my_strcmp(token, "$23") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$23");
        } else if ((my_strcmp(token, "$t8") == 0) || (my_strcmp(token, "$24") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$24");
        } else if ((my_strcmp(token, "$t9") == 0) || (my_strcmp(token, "$25") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$25");
        } else if ((my_strcmp(token, "$k0") == 0) || (my_strcmp(token, "$26") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$26");
        } else if ((my_strcmp(token, "$k1") == 0) || (my_strcmp(token, "$27") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$27");
        } else if ((my_strcmp(token, "$gp") == 0) || (my_strcmp(token, "$28") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$28");
        } else if ((my_strcmp(token, "$sp") == 0) || (my_strcmp(token, "$29") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$29");
        } else if ((my_strcmp(token, "$fp") == 0) || (my_strcmp(token, "$30") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$30");
        } else if ((my_strcmp(token, "$ra") == 0) || (my_strcmp(token, "$31") == 0)) {
            my_strcat(instruction, " ");
            my_strcat(instruction, "$31");
        } else if (atoi(token) > 31) {
            if (my_strstr(token, "$") != NULL) {
                printf("Register out of bounds, %d is higher than 31 \n", atoi(token));
                exit(1);
            } else {
                my_strcat(instruction, " ");
                my_strcat(instruction, token);
            }
        } else {
            my_strcat(instruction, " ");
            my_strcat(instruction, token);
        }


        token = strtok(NULL, " \r\n");
    }
    free(ist);
    return instruction;
}

// check format , make a structure based on type of format , return it
inst parser(char *instruction) {
    inst ninst = {nop, 0, 0, 0, 0, 0};
    char *type = "?";
    char *op = (char *) malloc(100 * sizeof(char));
    char *s1 = (char *) malloc(100 * sizeof(char));
    char *s2 = (char *) malloc(100 * sizeof(char));
    char *s3 = (char *) malloc(100 * sizeof(char));
    op = strtok(instruction, " ");
    s1 = strtok(NULL, " ");
    s2 = strtok(NULL, " ");
    s3 = strtok(NULL, " \r\n");
// check format
    if (my_strcmp(op, "add") == 0) {
        //trcpy(instruction,"add");
        type = "R";
        ninst.op = add;
    } else if (my_strcmp(op, "sub") == 0) {
        //my_strcpy(instruction,"sub");
        type = "R";
        ninst.op = sub;
    } else if (my_strcmp(op, "mul") == 0) {
        //my_strcpy(instruction,"mul");
        type = "R";
        ninst.op = mul;
    } else if (my_strcmp(op, "addi") == 0) {
        //my_strcpy(instruction,"addi");
        type = "I";
        ninst.op = addi;
    } else if (my_strcmp(op, "beq") == 0) {
        // my_strcpy(instruction,"beq");
        type = "I";
        ninst.op = beq;
    } else if (my_strcmp(op, "lw") == 0) {
        type = "M";
        // my_strcpy(instruction,"lw");
        ninst.op = lw;
    } else if (my_strcmp(op, "sw") == 0) {
        type = "M";
        //my_strcpy(instruction,"sw");
        ninst.op = sw;
    } else if (my_strcmp(op, "haltSimulation") == 0) {
        type = "S";
        //my_strcpy(instruction,"haltSimulation");
        ninst.op = haltSimulation;
    }
// create struct base on type
    switch (*type) {

        case '?':
            printf("Illegal opcode %s\n", op);
            exit(1);
        case 'R':
            if (my_strstr(s1, "$") == NULL || my_strstr(s2, "$") == NULL ||
                my_strstr(s3, "$") == NULL) {  // check registor has $
                printf("register no $");
                exit(1);
            }

            ninst.dest = (int) strtol(strtok(s1, "$"), NULL, 10); //dest
            ninst.s1 = (int) strtol(strtok(s2, "$"), NULL, 10);   //%s
            ninst.s2 = (int) strtol(strtok(s3, "$"), NULL, 10);  //$t
            if(!isdigit(ninst.s1)||!isdigit(ninst.s2)||!isdigit(ninst.dest)){
                exit(11);
            }
            return ninst;
        case 'I':
            if (my_strstr(s1, "$") == NULL || my_strstr(s2, "$") == NULL) {// check registor has $
                printf("register no $\n");
                exit(1);
            } else if (atoi(s3) >= 32767) {                        //check valid im
                printf("invalid im\n");
                exit(1);
            }
            ninst.s1 = (int) strtol(strtok(s2, "$"), NULL, 10);
            ninst.s2 = (int) strtol(strtok(s1, "$"), NULL, 10); //s2 is dest
            ninst.im = (int) strtol(strtok(s3, "$"), NULL, 10);
            return ninst;
        case 'M':
            if (my_strstr(s1, "$") == NULL || my_strstr(s3, "$") == NULL) {// check registor has $
                printf("register no $\n");
                exit(1);
            } else if (atoi(s2) % 4 != 0 || atoi(s2) >= 32767) {//check valid im and proper memory address
                printf("invalid im\n");
                exit(1);
            }
            ninst.s1 = (int) strtol(strtok(s3, "$"), NULL, 10);
            ninst.s2 = (int) strtol(strtok(s1, "$"), NULL, 10);  //dest
            ninst.im = (int) strtol(strtok(s2, "$"), NULL, 10);

            return ninst;
        case 'S':

            return ninst;

    }
    free(op);
    free(s1);
    free(s2);
    free(s3);
}


/********************Stages************************/

int regCheck(int sReg) {                                    //sReg is the register number that is being checked
    if (EXMEMlatch.validBit ==
        1) {                            //This only checks against registers that are in valid latches
        switch (EXMEMlatch.operation.op) {                    //Needs to check against different registers for different instructions
            case add:                                        //add, sub, mul all produce to the d register
            case sub:
            case mul:
                if (EXMEMlatch.operation.dest == sReg) return 1;
                break;
            case addi:                                        //addi and lw produce to the s2 register
            case lw:
                if (EXMEMlatch.operation.s2 == sReg) return 1;
                break;
            case beq:                                        //beq, sw and halt produce nothing
            case sw:
            case haltSimulation:
                break;
            default:
                printf("Op Code not recognized: EX_MEM Latch \n");
                exit(1);                                //This should never be reached
        }
    }
    if (MEMWBlatch.validBit == 1) {
        switch (MEMWBlatch.operation.op) {                    //Needs to check against different registers for different instructions
            case add:                                        //add, sub, mul all produce to the d register
            case mul:

            case sub:
                if (MEMWBlatch.operation.dest == sReg) return 1;
                break;
            case addi:                                        //addi and lw produce to the s2 register

            case lw:
                if (MEMWBlatch.operation.s2 == sReg) return 1;
                break;
            case beq:                                        //beq, sw and halt produce nothing
            case sw:
            case haltSimulation:
                break;
            default:
                printf("Op Code not recognized: MEM_WB Latch \n");
                exit(1);                                //This should never be reached
        }
    }
    return 0;                                            //If it never catches, the register is fine
}

void ID() {
    if (IDEXlatch.operation.op ==
        haltSimulation) {                        //If halt has passed through here, it just auto returns
        return;
    }
    if (((IFIDlatch.validBit == 1) && (IDEXlatch.validBit == 0))) {
        if (regCheck(IFIDlatch.operation.s1) == 1) return;

        switch (IFIDlatch.operation.op) {                        //Instruction cases are grouped for efficiency because certain instructions have the same actions
            case add:                                        //add
            case sub:                                        //sub
                if (regCheck(IFIDlatch.operation.s2) == 1)
                    return;    //Checks s2 for add and sub, these two instructions have the same ID step
                IDEXlatch = IFIDlatch;

                IDEXlatch.operation.s1 = mips_reg[IFIDlatch.operation.s1];    //loads s1 into the next latch
                IDEXlatch.operation.s2 = mips_reg[IFIDlatch.operation.s2];    //loads s2 into the next latch

                EXdelay = n;                            //Set the EXdelay for normal operation delay
                break;

            case mul:                                        //mul
                if (regCheck(IFIDlatch.operation.s2) == 1)
                    return;    //Checks s2 for mul, it has a different ID step than add and sub
                IDEXlatch = IFIDlatch;

                IDEXlatch.operation.s1 = mips_reg[IFIDlatch.operation.s1];    //loads s1 into the next latch
                IDEXlatch.operation.s2 = mips_reg[IFIDlatch.operation.s2];    //loads s2 into the next latch

                EXdelay = m;                            //Set the EXdelay for multiplication delay (difference between add and sub for passing to EX)
                break;

            case beq:                                        //beq
                if (regCheck(IFIDlatch.operation.s2) == 1)
                    return;    //Checks s2 of beq, it has a different ID step than the mul, add and sub
                //check that im value is in range for 16 bit 2's comp, +/- 32767 (2^15 - 1)
                if ((IFIDlatch.operation.im < -32767) || (IFIDlatch.operation.im > 32767)) {
                    printf("Immediate field out of numerical bounds, %d \n",
                           IFIDlatch.operation.im);    //Note we did not just use two assertions because we wanted to print an error message
                    exit(1);                            //Halt the code if this error message has been printed
                }
                IDEXlatch = IFIDlatch;
                IDEXlatch.operation.s1 = mips_reg[IFIDlatch.operation.s1];    //loads s1 into the next latch
                IDEXlatch.operation.s2 = mips_reg[IFIDlatch.operation.s2];    //loads s2 into the next latch
                Branchflag = 1;                        //If it reads a branch, mark branch pending
                EXdelay = n;                            //Set the EXdelay for normal operation delay
                break;

            case addi:                                        //addi
            case lw:                                        //lw
                //check that im value is in range for 16 bit 2's comp, +/- 32767 (2^15 - 1)
                if ((IFIDlatch.operation.im < -32767) || (IFIDlatch.operation.im > 32767)) {
                    printf("Immediate field out of numerical bounds, %d \n",
                           IFIDlatch.operation.im);    //Note we did not just use two assertions because we wanted to print an error message
                    exit(1);                            //Halt the code if this error message has been printed
                }
                IDEXlatch = IFIDlatch;
                IDEXlatch.operation.s1 = mips_reg[IFIDlatch.operation.s1];    //loads s1 into the next latch
                EXdelay = n;                            //Set the EXdelay for normal operation delay
                break;

            case sw:                                        //sw
                if (regCheck(IFIDlatch.operation.s2) == 1)
                    return;    //Checks s2 of sw, it relies on s2 while addi and lw do not
                //check that im value is in range for 16 bit 2's comp, +/- 32767 (2^15 - 1)
                if ((IFIDlatch.operation.im < -32767) || (IFIDlatch.operation.im > 32767)) {
                    printf("Immediate field out of numerical bounds, %d \n",
                           IFIDlatch.operation.im);    //Note we did not just use two assertions because we wanted to print an error message
                    exit(1);                            //Halt the code if this error message has been printed
                }
                IDEXlatch = IFIDlatch;
                IDEXlatch.operation.s1 = mips_reg[IFIDlatch.operation.s1];    //loads s1 into the next latch
                IDEXlatch.operation.s2 = mips_reg[IFIDlatch.operation.s2];    //loads s2 into the next latch
                EXdelay = n;                            //Set the EXdelay for normal operation delay
                break;

            case haltSimulation:                                        //halt
                IDEXlatch = IFIDlatch;
                EXdelay = 1;                            //Set the EXdelay to be 1 for fast propagation
                return;                                    //Return so we don't count as useful work or clear the latch

            default:
                printf("Op Code not recognized: ID Stage \n");
                exit(1);                                //If it doesn't decode one of the 7 possibilities there is an error, should be caught by the parser, but this is a backup
        }

        IFIDlatch = tempLatch;
        idUtil++;
    }

    return;

}

void EX() {
    if (EXMEMlatch.operation.op == haltSimulation) {//If halt has passed through here, it just auto returns
        return;
    } else if (IDEXlatch.validBit ==
               1) {                        //Check previous latch to see if it can start it's operation
        if (((EXdelay == 1) && (EXMEMlatch.validBit ==
                                0))) {    //Once the delay count down is finished and the latch is ready to receive, compute and push

            EXMEMlatch = IDEXlatch;
            switch (IDEXlatch.operation.op) {
                case add:                                    //add
                    EXMEMlatch.EXresult = IDEXlatch.operation.s1 + IDEXlatch.operation.s2;

                    break;
                case sub:                                    //sub
                    EXMEMlatch.EXresult = IDEXlatch.operation.s1 - IDEXlatch.operation.s2;

                    break;

                case mul:                                    //mul
                    EXMEMlatch.EXresult = IDEXlatch.operation.s1 *
                                          IDEXlatch.operation.s2;    //Assumes that the system is operating at 32 bits so the result will be cast down to the right size

                    break;
                case beq:
                    if (IDEXlatch.operation.s1 == IDEXlatch.operation.s2) {
                        IFPC = IDEXlatch.operation.im + IDEXlatch.PC +
                               1;    //Advances pc by the immediate value past the pc of the next instruction
                    }
                    Branchflag = 2;                    //Set to 2 so the IF stage knows not to unfreeze until the next cycle
                    break;
                case addi:                                    //addi
                case lw:                                    //lw
                case sw:                                    //sw
                    EXMEMlatch.EXresult = IDEXlatch.operation.s1 +
                                          IDEXlatch.operation.im;    //Note these three all do the same thing in EX, the results are just handled differently
                    break;

                case haltSimulation:

                    return;
                default:
                    printf("Op Code not recognized: EX stage \n");
                    exit(1);
            }
            IDEXlatch = tempLatch;
            exUtil++;
        } else if (EXdelay !=
                   1) {                            //Note this is set by the previous stage, so it will never be changed until this stage clears the latch
            EXdelay -= 1;                                //Counts down the delay for the EX stage
            exUtil++;
            assert(EXdelay > -0);                        //Catch if this decreases too much
        }
    }

    return;

}

/*****************Support Functions***************/




////********************Todd**********************////

void IF(void) {
    if (IFIDlatch.operation.op == haltSimulation) {//if halt
        return;

    }
    if ((IFIDlatch.validBit == 0) && (Branchflag == 0)) {
        if (iMem[IFPC].op == haltSimulation) {
            IFIDlatch.validBit = 1;
            IFIDlatch.operation = iMem[IFPC];
            IFIDlatch.PC = IFPC;
        } else if (IFdelay == 1) {//if finished
            IFIDlatch.validBit = 1;
            IFIDlatch.operation = iMem[IFPC];
            IFIDlatch.PC = IFPC;

            IFPC++;
            if (IFPC > 511) {//if to the end then stay
                IFPC = 511;
            }
            ifUtil++;
            IFdelay = c;
        } else {       //counting
            IFdelay--;
            ifUtil++;
            assert(IFdelay > 0);
        }
    } else if (Branchflag == 2) {
        Branchflag = 0;
    }
    return;

}


/* if not LW or SW, then just skip MEM and move on to
 * next stage, if LW or SW then wait c cycles;
 */
void MEM(void) {

    if ((EXMEMlatch.validBit == 1) && (MEMWBlatch.validBit == 0)) {

        switch (EXMEMlatch.operation.op) {
            default: // non-mem op
                MEMWBlatch = EXMEMlatch;
                EXMEMlatch = tempLatch;
                return;
            case lw:
                memUtil++;
                if (Mdelay == 1) {
                    MEMWBlatch = EXMEMlatch;
                    if (EXMEMlatch.EXresult % 4 != 0) {
                        printf("Data Alignment Error, %d not divisible by 4 \n", EXMEMlatch.EXresult);
                        exit(1);
                    }

                    MEMWBlatch.EXresult = dMem[EXMEMlatch.EXresult / 4];

                    Mdelay = c;
                    EXMEMlatch = tempLatch;
                    memUtil++;
                } else {
                    Mdelay--;
                    memUtil++;
                    assert(Mdelay > 0);
                }
                return;
            case sw:
                memUtil++;
                if (Mdelay == 1) {
                    MEMWBlatch = EXMEMlatch;
                    if (EXMEMlatch.EXresult % 4 != 0) {
                        printf("Data Alignment Error, %d not divisible by 4 \n", EXMEMlatch.EXresult);
                        exit(1);
                    }


                    dMem[EXMEMlatch.EXresult / 4] = EXMEMlatch.operation.s2;

                    Mdelay = c;
                    EXMEMlatch = tempLatch;
                    memUtil++;
                } else {
                    Mdelay--;
                    memUtil++;
                    assert(Mdelay > 0);
                }

                return;

            case haltSimulation:
                MEMWBlatch = EXMEMlatch;
                MEMWBlatch.validBit = 1;
                return;
        }

    }

    return;//dummy return
}


void WB(inst wbOp, int data) {

    if ((wbOp.op == add) || (wbOp.op == sub) || (wbOp.op == mul)) {//storing to $d(dest)
        wbUtil++;
        mips_reg[wbOp.dest] = data;
    } else if ((wbOp.op == addi) || (wbOp.op == lw)) {//storing to $t(s2)
        mips_reg[wbOp.s2] = data;
        wbUtil++;
    }
    return;
}


/********************Main************************/
int main(int argc, char *argv[]) {

    int sim_mode = 0;//mode flag, 1 for single-cycle, 0 for batch

    int i;//for loop counter
    int a;//for dMEM loop counter
    long pgm_c = 0;//program counter
    long sim_cycle = 0;//simulation cycle counter
    //define your own counter for the usage of each pipeline stage here

    int test_counter = 0;
    FILE *input = NULL;
    FILE *output = NULL;
    printf("The arguments are:");

    for (i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    if (argc == 7) {

        if (my_strcmp("-s", argv[1]) == 0) {
            sim_mode = SINGLE;
        } else if (my_strcmp("-b", argv[1]) == 0) {

            sim_mode = BATCH;
        } else {
            printf("Wrong sim mode chosen\n");
            exit(1);
        }

        m = atoi(argv[2]);
        n = atoi(argv[3]);
        c = atoi(argv[4]);
        input = fopen(argv[5], "r");
        output = fopen(argv[6], "w");

    } else {
        printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
        printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
        exit(1);
    }
    if (input == NULL) {
        printf("Unable to open input or output file\n");
        exit(1);
    }
    if (output == NULL) {
        printf("Cannot create output file\n");
        exit(1);
    }


    //initialize registers and program counter

    for (i = 0; i < REG_NUM; i++) {
        mips_reg[i] = 0;
    }


    //start your code from here
    dMem = (int *) malloc(sizeof(int) * 512);            //mallocing int array

    iMem = (inst *) malloc(512 * sizeof(inst));


    //********intialize dMem*************//
    for (a = 0; a < 511; a++) {
        dMem[a] = 0;
    }


//    mips_reg[0]=0;//cant change

    int MEMPC;//MEM PC pointer
    int WBPC;//WB PC pointer
    int MEMexResult;//MEM EX result
    int WBvalue = 0;
    IFPC = 0;//IF PC pointer


    inst WBinst = {nop, 0, 0, 0, 0, 0};
    inst IFinst = {nop, 0, 0, 0, 0, 0};

    inst *realMEM = (inst *) malloc(c * sizeof(inst));//true mem op array



    //store instruction to instruction memory
    iMem = malloc(512 * sizeof(inst));
    char *instruction = (char *) malloc(100 * sizeof(char));
    i = 0;
    int f = 1;
    while (f) {
        progScanner(input, instruction);
        regNumberConverter(instruction);
        iMem[i] = parser(instruction);
        if (iMem[i].op == haltSimulation)
            f = 0;

        i++;
    }
    //////////// main loop

    int foo = 1;//loop check
    ifUtil = 0;
    idUtil = 0;
    exUtil = 0;
    memUtil = 0;
    wbUtil = 0;
    Mdelay = c;
    IFdelay = c;
///////////////////////////////////////////
    MEMWBlatch = tempLatch;
    while (foo) {//test while loop
        if (MEMWBlatch.operation.op == haltSimulation) {
            foo = 0;
        }
        /*
         * WB stage
         */
        if (MEMWBlatch.validBit) {//if valid, download info
            WBinst = MEMWBlatch.operation;//passing inst
            MEMWBlatch.validBit = 0;//reset valid
            WBPC = MEMWBlatch.PC;
            WBvalue = MEMWBlatch.EXresult;
        }
        WB(WBinst, MEMWBlatch.EXresult);//store and this is the end of the inst

        /* MEM stage*/
        MEM();
        EX();
        ID();
        IF();//this will set the branchflag
        /**********************end*****************************/
        /*********************code 2***************************/
        if (sim_mode == 1) {
            printf("cycle: %d register value: ", sim_cycle);
            for (i = 1; i < REG_NUM; i++) {
                printf("%d  ", mips_reg[i]);
            }
            printf("program counter: %d\n", IFPC);
            printf("press ENTER to continue\n");
            while (getchar() != '\n');
        }

        sim_cycle += 1;
    }


    //add the following code to the end of the simulation,
    //to output statistics in batch mode
    if (sim_mode == 0) {
        fprintf(output, "program name: %s\n", argv[5]);
        fprintf(output, "stage utilization: %f  %f  %f  %f  %f \n",
                (double) ((ifUtil * 100) / sim_cycle), (double) ((idUtil * 100) / sim_cycle),
                (double) ((exUtil * 100) / sim_cycle), (double) ((memUtil * 100) / sim_cycle),
                (double) ((wbUtil * 100) / sim_cycle));
        // add the (double) stage_counter/sim_cycle for each
        // stage following sequence IF ID EX MEM WB

        fprintf(output, "register values ");
        for (i = 1; i < REG_NUM; i++) {
            fprintf(output, "%d  ", mips_reg[i]);
        }
        fprintf(output, "%d\n", pgm_c);

    }

    //close input and output files at the end of the simulation
    fclose(input);
    fclose(output);
    free(iMem);
    free(realMEM);
    free(dMem);
    free(instruction);
    return 110;
}
