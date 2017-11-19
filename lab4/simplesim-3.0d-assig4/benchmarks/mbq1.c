int mbq1(){
    int a[8]=0;
    int i,j;
    int result;
    for(i=0;i<1000000;i++){
        j = i%8;
        result = a[j];
    }
    return result;
}
