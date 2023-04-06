#include "Motor.h"

void Motor_Init(void){
    TIM3->CCR1 = TIM3->CCR2 = TIM3->CCR3 = TIM3->CCR4 = 1500;
    OLED_println((uint8_t *)"Put CH7 to LOW");
    while(!(Receiver_Data[7] <= 1000));
    OLED_println((uint8_t *)"Pull Down");
    while(TIM3->CCR3 >= 1000){
        TIM3->CCR1 = TIM3->CCR2 = TIM3->CCR3 = TIM3->CCR4 = Receiver_Data[3];
    }
}

//RollPID
//Tar: Target value
//Cur: Current value
//retrun: Value to be changed
#define Kpri 35.f
#define Kiri 0.f
#define Kdri 0.35f
#define Kpro 0.1f
#define Kiro 0.f
#define Kdro 0.f
float RollPID(float Tar, float Cur){
    static float Alast = 0, Asum = 0, Vlast = 0, Vsum = 0;
    float delta, Aerror = Tar - Cur, Verror;
    if(Asum+Aerror < 10.0f && Asum+Aerror > -10.0f)
        Asum += Aerror;
    if(Vsum+Verror < 10.f && Vsum+Verror > -10.f)
        Vsum += Verror;
    delta = Kpro*(Aerror) + Kiro*(Asum) + Kdro*(Aerror - Alast);    //out-loop PID to angle
    Verror = delta - Gyro_Data.Y / ANGLETORAD;
    delta = Kpri*(Verror) + Kiri*(Vsum) + Kdri*(Verror - Vlast);    //in-loop PID to angular velocity
    Alast = Aerror;
    Vlast = Verror;
    return delta;
}

//PitchPID
//Tar: Target value
//Cur: Current value
//retrun: Value to be changed
#define Kppi 25.f
#define Kipi 0.f
#define Kdpi 0.25f
#define Kppo 0.12f
#define Kipo 0.f
#define Kdpo 0.f
float PitchPID(float Tar, float Cur){
    static float Alast = 0, Asum = 0, Vlast = 0, Vsum = 0;
    float delta, Aerror = Tar - Cur, Verror;
    if(Asum+Aerror < 10.0f && Asum+Aerror > -10.0f)
        Asum += Aerror;
    if(Vsum+Verror < 10.f && Vsum+Verror > -10.f)
        Vsum += Verror;
    delta = Kppo*(Aerror) + Kipo*(Asum) + Kdpo*(Aerror - Alast);    //out-loop PID to angle
    Verror = delta - Gyro_Data.X / ANGLETORAD;
    delta = Kppi*(Verror) + Kipi*(Vsum) + Kdpi*(Verror - Vlast);   //in-loop PID to angular velocity
    Alast = Aerror;
    Vlast = Verror;
    return delta;
}

//YawPID
//Tar: Target value
//Cur: Current value
//retrun: Value to be changed
#define Kpyi 3.f
#define Kiyi 0.f
#define Kdyi 0.f
#define Kpyo 0.8f
#define Kiyo 0.f
#define Kdyo 0.f
float YawPID(float Tar, float Cur){
    static float Alast = 0, Asum = 0, Vlast = 0, Vsum = 0;
    float delta, Aerror = Tar - Cur, Verror;
    if(Asum+Aerror < 10.0f && Asum+Aerror > -10.0f)
        Asum += Aerror;
    if(Vsum+Verror < 10.f && Vsum+Verror > -10.f)
        Vsum += Verror;
    delta = Kpyo*(Aerror) + Kiyo*(Asum) + Kdyo*(Aerror - Alast);    //out-loop PID to angle
    Verror = delta - Gyro_Data.X;
    delta = Kpyi*(Verror) + Kiyi*(Vsum) + Kdyi*(Verror - Vlast);   //in-loop PID to angular velocity
    Alast = Aerror;
    Vlast = Verror;
    return delta;
}

//Control Motor
//  (S)CN1   CN2(N)
//        \ /
//        / \
//  (N)CN3   CN4(S)
void Motor_Output(void){

    float dR, dP, dY, tR, tP;
    tR = (float)(Receiver_Data[1] - 1500) / 500.f * 20.f;
    tP = -(float)(Receiver_Data[2] - 1500) / 500.f * 15.f;
    dR = RollPID(tR, eulra.roll);
    dP = PitchPID(tP, eulra.pitch);
    dY = YawPID(0, eulra.yaw);
    if(!(Receiver_Data[7] <= 1000)){
        //                                       Roll                                                         Pitch                    Yaw
        TIM3->CCR1 = Receiver_Data[3] + dR + dP - dY;//(Receiver_Data[4]-1500)/10;
        TIM3->CCR2 = Receiver_Data[3] - dR + dP + dY;//(Receiver_Data[4]-1500)/10;
        TIM3->CCR3 = Receiver_Data[3] + dR - dP + dY;//(Receiver_Data[4]-1500)/10;
        TIM3->CCR4 = Receiver_Data[3] - dR - dP - dY;//(Receiver_Data[4]-1500)/10;
    }
    else
        TIM3->CCR1 = TIM3->CCR2 = TIM3->CCR3 = TIM3->CCR4 = 997;
}
