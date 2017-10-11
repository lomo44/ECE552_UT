#include "predictor.h"
#include "string.h"
/////////////////////////////////////////////////////////////
// 2bitsat
/////////////////////////////////////////////////////////////
#define BYTE unsigned char
#define HWORD short
#define WORD int
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
    int index =  ( PC % 4096 << 2) >> 2;
    if (two_bit_cont[index] == STRONG_TAKEN || two_bit_cont[index] == WEAK_TAKEN)
        return TAKEN;
    else
        return NOT_TAKEN;
}

void UpdatePredictor_2bitsat(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    int index = ( PC % 4096 << 2) >> 2;
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
BYTE gPHT[64][8];

int GetPHTIndex(UINT32 PC){
    return PC & 0b111;
}

int GetBHTIndex(UINT32 PC){
    return (PC & 0b111111111000) >> 3;
}

int GetHistoryIndex(BYTE history){
    return history & 0b00111111;
}

void InitPredictor_2level() {
    memset(gBHT,0, BYTE_SIZE * BHT_ENTRY_COUNT);
    int i = 0;
    for(i =0 ; i < 64; i++)
        memset(gPHT[i],WEAK_NOT_TAKEN, 8);
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
    int modified_result = current_history;

    if (resolveDir == TAKEN) {
        if (modified_result != STRONG_TAKEN)
            modified_result ++;
    }
    if (resolveDir == NOT_TAKEN) {
        if (modified_result != STRONG_NOT_TAKEN)
             modified_result --;
    }
    gPHT[GetHistoryIndex(gBHT[bht_index])][pht_index] = modified_result;
    gBHT[bht_index] <<= 1;
    gBHT[bht_index] |= (resolveDir == TAKEN) ? TAKEN : NOT_TAKEN;
}


BYTE gShare;
BYTE gGlobalSelect[256];


void InitPredictor_Global(){
    gShare = 0;
    memset(gGlobalSelect, WEAK_NOT_TAKEN,256);
}

bool GetPrediction_Global(UINT32 PC){
    int index = ((PC >> 4) & 255) ^ gShare;
    if(gGlobalSelect[index] >= WEAK_TAKEN)
        return TAKEN;
    else{
        return NOT_TAKEN;
    }
}
void UpdatePredictor_Global(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    int index = ((PC >> 4) & 255) ^ gShare;
    int modified_result = gGlobalSelect[index];
    if (resolveDir == TAKEN) {
        if (modified_result != STRONG_TAKEN)
            modified_result ++;
    }
    if (resolveDir == NOT_TAKEN) {
        if (modified_result != STRONG_NOT_TAKEN)
            modified_result --;
    }
    gGlobalSelect[index] = modified_result;
    gShare <<= 1;
    gShare |= resolveDir == TAKEN? 1 : 0;
}

/////////////////////////////////////////////////////////////
// openend
/////////////////////////////////////////////////////////////

int take[4096];
int not_take[4096];
void Init_RP_Predictor_openend() {\
    for (int i = 0; i<4096;i++){
        take[i]=0;
        not_take[i]=0;
    }

}

bool Get_RP_Prediction_openend(UINT32 PC) {
    int index = (PC>>2) &0b111111111111;
    if (take[index] >= not_take[index])
        return TAKEN;
    else
        return NOT_TAKEN;
}

void Update_RP_Predictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    int index = (PC>>2) &0b111111111111;
    if (resolveDir == TAKEN) {
        take[index]++;
    } else {
        not_take[index] ++;
    }
}

void InitPredictor_openend() {
    InitPredictor_2bitsat();
    InitPredictor_2level();
    InitPredictor_Global();
}

bool GetPrediction_openend(UINT32 PC) {
    int taken_count = 0;
    if(GetPrediction_2bitsat(PC))
        taken_count++;
    if(GetPrediction_2level(PC))
        taken_count++;
    if(GetPrediction_Global(PC))
        taken_count++;
    if(taken_count>=2)
        return TAKEN;
    else
        return NOT_TAKEN;

}

void UpdatePredictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    UpdatePredictor_2bitsat(PC,resolveDir,predDir,branchTarget);
    UpdatePredictor_2level(PC,resolveDir,predDir,branchTarget);
    UpdatePredictor_Global(PC,resolveDir,predDir,branchTarget);
}


