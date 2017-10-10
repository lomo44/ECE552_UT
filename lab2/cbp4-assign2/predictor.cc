#include "predictor.h"
#include "string.h"
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
BYTE gPHT[64][8];

int GetPHTIndex(UINT32 PC){
    return PC & 0b111;
}

int GetBHTIndex(UINT32 PC){
    return (PC & 0b1111111000) >> 3;
}

int GetHistoryIndex(BYTE history){
    return history & 0b00111111;
}

void InitPredictor_2level() {
    memset(gBHT,0, BYTE_SIZE * BHT_ENTRY_COUNT);
    memset(gPHT,WEAK_NOT_TAKEN, BYTE_SIZE * PHT_ENTRY_COUNT);
}

bool GetPrediction_2level(UINT32 PC) {
    int pht_index = GetPHTIndex(PC);
    int bht_index = GetBHTIndex(PC);
    if (gPHT[GetHistoryIndex(gBHT[bht_index])][pht_index] >= WEAK_TAKEN)
        return TAKEN;
    else
        return NOT_TAKEN;
}

void UpdatePredictor_2level(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    int bht_index = GetBHTIndex(PC);
    int pht_index = GetPHTIndex(PC);
    int current_history = gPHT[GetHistoryIndex(gBHT[bht_index])][pht_index];
    int modified_result = STRONG_NOT_TAKEN;
    switch(current_history){
        case WEAK_NOT_TAKEN:{
            if(resolveDir!=predDir) {
                modified_result = STRONG_NOT_TAKEN;
            }
            else{
                modified_result = WEAK_TAKEN;
            }
            break;
        }
        case STRONG_NOT_TAKEN:{
            if(resolveDir == predDir){
                modified_result = WEAK_NOT_TAKEN;
            }
            break;
        }
        case WEAK_TAKEN:{
            if(resolveDir == predDir){
                modified_result = STRONG_TAKEN;
            }
            else{
                modified_result = WEAK_NOT_TAKEN;
            }
            break;
        }
        case STRONG_TAKEN:{
            if(resolveDir != predDir){
                modified_result = WEAK_NOT_TAKEN;
            }
            break;
        }
        default:
            break;
    }
    gPHT[GetHistoryIndex(gBHT[bht_index])][pht_index] = modified_result;
    gBHT[bht_index] <<= 1;
    gBHT[bht_index] |= resolveDir;
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

