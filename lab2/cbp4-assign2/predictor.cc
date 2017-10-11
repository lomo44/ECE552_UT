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

#define GSHARE_SIZE 10
int gShare;
#define GSELECT_SIZE (1<<GSHARE_SIZE)

BYTE gGlobalSelect[GSELECT_SIZE];

int GetPredictionIndex_Global(UINT32 PC){
    return (((PC >> 4) & (GSELECT_SIZE-1)) ^ (gShare & (GSELECT_SIZE-1)));
}

void InitPredictor_Global(){
    gShare = 0;
    memset(gGlobalSelect, WEAK_NOT_TAKEN,GSELECT_SIZE);
}

bool GetPrediction_Global(UINT32 PC){
    int index = GetPredictionIndex_Global(PC);
    if(gGlobalSelect[index] >= WEAK_TAKEN)
        return TAKEN;
    else{
        return NOT_TAKEN;
    }
}
void UpdatePredictor_Global(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    int index = GetPredictionIndex_Global(PC);
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


BYTE RP_cnt[256];
void InitPredictor_RP() {\
    for (int i = 0; i<256;i++){
        RP_cnt[i]=128;
    }

}

bool GetPrediction_RP(UINT32 PC) {
    int index = (PC>>4) % 255;
    if (RP_cnt[index] >= 128)
        return TAKEN;
    else
        return NOT_TAKEN;
}

void UpdatePredictor_RP(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    int index = (PC>>4) % 255;
    if (resolveDir == TAKEN) {
        if (RP_cnt[index] < 256)
            RP_cnt[index]++;
    } else {
        if (RP_cnt[index] > 0)
            RP_cnt[index] --;
    }
}


/////////////////////////////////////////////////////////////
// openend
/////////////////////////////////////////////////////////////

#define PERCEPTRON_ROW 512
#define PERCEPTRON_COL 31
#define P_THRESHOLD 73

char percep_table[PERCEPTRON_ROW][PERCEPTRON_COL];
bool p_hist[PERCEPTRON_COL];
int output = 0;

int GetPrediction_PerC(UINT32 PC) {
    int row_index = PC % PERCEPTRON_ROW;
    int j, weight_sum = 0;
    for(j = 1; j < PERCEPTRON_COL; j++){
        int masked_weight = percep_table[row_index][j];     //8 bits for all weights
        if(p_hist[j]){
            //taken in global history
            weight_sum += masked_weight;
        }
        else{
            weight_sum += -1*masked_weight;
        }
    }
    int masked_bias = percep_table[row_index][0];
    output = masked_bias + weight_sum;
    return (output >= 0) ? TAKEN : NOT_TAKEN;
}
void UpdatePredictor_PerC(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    int i = PC % PERCEPTRON_ROW;
    if(resolveDir != predDir || abs(output) <= P_THRESHOLD){
        if(resolveDir == TAKEN){
            percep_table[i][0] += 1;
        }
        else{
            percep_table[i][0] += -1;
        }
        int j = 1;
        for(; j < PERCEPTRON_COL; j++){
            if(resolveDir == p_hist[j]){
                percep_table[i][j] += 1;
            }
            else{
                percep_table[i][j] += -1;
            }
        }
    }
    int k;

    for (k = 1; k < PERCEPTRON_COL-1; k++){
        p_hist[k] = p_hist[k+1];
    }
    p_hist[k] = (resolveDir == TAKEN);
}
void InitPredictor_PerC(){

}
void InitPredictor_openend() {
    InitPredictor_PerC();
}

bool GetPrediction_openend(UINT32 PC) {
    return GetPrediction_PerC(PC);

}

void UpdatePredictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
    UpdatePredictor_PerC(PC,resolveDir,predDir,branchTarget);
}


