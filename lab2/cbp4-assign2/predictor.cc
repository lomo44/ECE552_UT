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


void InitPredictor_2bitsat() {

}

bool GetPrediction_2bitsat(UINT32 PC) {

    return TAKEN;
}

void UpdatePredictor_2bitsat(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {

}

/////////////////////////////////////////////////////////////
// 2level
/////////////////////////////////////////////////////////////

BYTE gBHT[512];
BYTE gPHT[8];

void InitPredictor_2level() {
    memset(gBHT,0, BYTE_SIZE * BHT_ENTRY_COUNT);
    memset(gPHT,0, BYTE_SIZE * PHT_ENTRY_COUNT);
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

