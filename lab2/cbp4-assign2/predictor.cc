#include "predictor.h"

/////////////////////////////////////////////////////////////
// 2bitsat
/////////////////////////////////////////////////////////////
#define BYTE unsigned char
#define BYTE_SIZE sizeof(BYTE)
#define BHT_ENTRY_COUNT 512
#define PHT_ENTRY_COUNT 8
#define STRONG_TAKEN 3
#define WEAK_TAKEN 2
#define WEAK_NOT_TAKEN 1
#define STRONG_NOT_TAKEN 0

int two_bit_cont [4096];

void InitPredictor_2bitsat() {

    for (int i =0; i <4096; i++){
        two_bit_cont[i] = WEAK_NOT_TAKEN;
    }
}

bool GetPrediction_2bitsat(UINT32 PC) {
    int index = PC % 4096;
    if (two_bit_cont[index] == STRONG_TAKEN || two_bit_cont[index] == WEAK_TAKEN)
        return TAKEN;
    else
        return NOT_TAKEN;
}

void UpdatePredictor_2bitsat(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    int index = PC % 4096;
    if (resolveDir == TAKEN) {
        if (two_bit_cont[index] != STRONG_TAKEN)
            two_bit_cont[index] ++;
    }
    if (resolveDir == NOT_TAKEN) {
        if (two_bit_cont[index] != STRONG_NOT_TAKEN)
            two_bit_cont[index] --;
    }

}

/////////////////////////////////////////////////////////////
// 2level
/////////////////////////////////////////////////////////////

BYTE gBHT[512];
BYTE gPHT[8];

void InitPredictor_2level() {
   // memset(gBHT,0, BYTE_SIZE * BHT_ENTRY_COUNT);
    //memset(gPHT,0, BYTE_SIZE * PHT_ENTRY_COUNT);
}

bool GetPrediction_2level(UINT32 PC) {
    return TAKEN;
}

void UpdatePredictor_2level(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {

}

/////////////////////////////////////////////////////////////
// openend
/////////////////////////////////////////////////////////////

void InitPredictor_openend() {

}

bool GetPrediction_openend(UINT32 PC) {

    return TAKEN;
}

void UpdatePredictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {

}

