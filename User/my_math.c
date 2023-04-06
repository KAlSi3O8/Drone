float invsqrt(float number){
    int FinL;
    float res, halfNum = number * 0.5f;
    const float threeHalfs = 1.5f;

    FinL = *(int *)&number;
    FinL = 0x5F3759DF - (FinL >> 1);
    res = *(float *)&FinL;
    res = res * (threeHalfs - (halfNum * res * res));
    res = res * (threeHalfs - (halfNum * res * res));
    return res;
}

float my_sqrt(float number){
    return 1/invsqrt(number);
}
