#include "Madgwick.h"
#define USE_MAG 1

struct EULRA_Struct eulra;
static struct{
    float q0;
    float q1;
    float q2;
    float q3;
}quat = {1.0f, 0.0f, 0.0f, 0.0f};

//function: Q = Q0 + ((1/2 Q0 * Ws) - b(dS(Q0)/|dS(Q0)|))dt
//input:  gyroscope-x-y-z\
          accelerate-x-y-z\
          magnetic-x-y-z
//output: global Q-0-1-2-3\
          global yaw-pitch-roll
void My_Madgwick_Update(float ax, float ay, float az,\
                        float gx, float gy, float gz,\
                        float mx, float my, float mz)
{
    float Mid_vec[4];
    #if USE_MAG
    float hx, hy;
    float bx, bz;
    #endif
    float Mid_gyro[4];
    float Mid_vec_len;
    float Mid_quat[4];
//    float dt;
    // 1/2 Q0 * Ws              \
       Ws = (0,  gx, gy, gz)    \
       Q0 = (q0, q1, q2, q3)    \
       /0  -gx -gy -gz\ /q0\    \
       |gx  0   gz -gy| |q1|    \
       |gy -gz  0   gx| |q2|    \
       \gz  gy -gx  0 / \q3/
    
    Mid_gyro[0] = (-quat.q1*gx -quat.q2*gy -quat.q3*gz) * 0.5f;
    Mid_gyro[1] = ( quat.q0*gx +quat.q2*gz -quat.q3*gy) * 0.5f;
    Mid_gyro[2] = ( quat.q0*gy -quat.q1*gz +quat.q3*gx) * 0.5f;
    Mid_gyro[3] = ( quat.q0*gz +quat.q1*gy -quat.q2*gx) * 0.5f;
    
    /* dS(Q0)
      /-2q2             2q3            -2q0             2q1         \ /2q1q3-2q0q2   -ax                     \
      | 2q1             2q0             2q3             2q2         | |2q2q3+2q0q1   -ay                     |
      | 0              -4q1            -4q2             0           | |1-2q1q1-2q2q2 -az                     |
      |-2q2hz           2q3hz          -4q2hx-2q0hz    -4q3hx+2q1hz | |(1-2q2q2-2q3q3)bx +(2q1q3-2q0q2)bz -mx|
      | 2q1hz-2q3hx     2q2hx+2q0hz     2q1hx+2q3hz    -2q0hx+2q2hz | |(2q1q2-2q0q3)bx   +(2q2q3+2q0q1)bz -my|
      \ 2q2hx           2q3hx-4q1hz     2q0hx-4q2hz     2q1hx       / \(2q1q3+2q0q2)bx +(1-2q1q1-2q2q2)bz -mz/  */
    
    #if USE_MAG
    hx = mx*(1.0f-2.0f*quat.q2*quat.q2-2.0f*quat.q3*quat.q3) + \
         my*(2.0f*quat.q1*quat.q2 - 2.0f*quat.q0*quat.q3) + \
         mz*(2.0f*quat.q1*quat.q3 + 2.0f*quat.q0*quat.q2);
    hy = mx*(2.0f*quat.q0*quat.q3 + 2.0f*quat.q1*quat.q2) + \
         my*(1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q3*quat.q3) + \
         mz*(2.0f*quat.q2*quat.q3 + 2.0f*quat.q0*quat.q1);
    bx = my_sqrt(hx*hx + hy*hy);
    bz = mx*(2.0f*quat.q1*quat.q3 - 2.0f*quat.q0*quat.q2) + \
         my*(2.0f*quat.q0*quat.q1 + 2.0f*quat.q2*quat.q3) + \
         mz*(1.0f-2.0f*quat.q1*quat.q1 - 2.0f*quat.q2*quat.q2);
//    bx = my_sqrt(mx*mx + my*my);
//    bz = mz;
    Mid_vec[0] = (2.0f*quat.q1*quat.q3 - 2.0f*quat.q0*quat.q2 - ax)*(-2.0f*quat.q2)+\
                 (2.0f*quat.q2*quat.q3 + 2.0f*quat.q0*quat.q1 - ay)*(2.0f*quat.q1)+\
                 ((1.0f-2.0f*quat.q2*quat.q2-2.0f*quat.q3*quat.q3)*bx +(2.0f*quat.q1*quat.q3-2.0f*quat.q0*quat.q2)*bz -mx)*(-2.0f*quat.q2*bz)+\
                 ((2.0f*quat.q1*quat.q2-2.0f*quat.q0*quat.q3)*bx   +(2.0f*quat.q2*quat.q3+2.0f*quat.q0*quat.q1)*bz -my)*(2.0f*quat.q1*bz-2.0f*quat.q3*bx)+\
                 ((2.0f*quat.q1*quat.q3+2.0f*quat.q0*quat.q2)*bx +(1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q2*quat.q2)*bz -mz)*(2.0f*quat.q2*bx);
    Mid_vec[1] = (2.0f*quat.q1*quat.q3 - 2.0f*quat.q0*quat.q2 - ax)*(2.0f*quat.q3)+\
                 (2.0f*quat.q2*quat.q3 + 2.0f*quat.q0*quat.q1 - ay)*(2.0f*quat.q0)+\
                 (1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q2*quat.q2 - az)*(-4.0f*quat.q1)+\
                 ((1.0f-2.0f*quat.q2*quat.q2-2.0f*quat.q3*quat.q3)*bx +(2.0f*quat.q1*quat.q3-2.0f*quat.q0*quat.q2)*bz -mx)*(2.0f*quat.q3*bz)+\
                 ((2.0f*quat.q1*quat.q2-2.0f*quat.q0*quat.q3)*bx   +(2.0f*quat.q2*quat.q3+2.0f*quat.q0*quat.q1)*bz -my)*(2.0f*quat.q2*bx+2.0f*quat.q0*bz)+\
                 ((2.0f*quat.q1*quat.q3+2.0f*quat.q0*quat.q2)*bx +(1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q2*quat.q2)*bz -mz)*(2.0f*quat.q3*bx-4.0f*quat.q1*bz);
    Mid_vec[2] = (2.0f*quat.q1*quat.q3 - 2.0f*quat.q0*quat.q2 - ax)*(-2.0f*quat.q0)+\
                 (2.0f*quat.q2*quat.q3 + 2.0f*quat.q0*quat.q1 - ay)*(2.0f*quat.q3)+\
                 (1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q2*quat.q2 - az)*(-4.0f*quat.q2)+\
                 ((1.0f-2.0f*quat.q2*quat.q2-2.0f*quat.q3*quat.q3)*bx +(2.0f*quat.q1*quat.q3-2.0f*quat.q0*quat.q2)*bz -mx)*(-4.0f*quat.q2*bx-2.0f*quat.q0*bz)+\
                 ((2.0f*quat.q1*quat.q2-2.0f*quat.q0*quat.q3)*bx   +(2.0f*quat.q2*quat.q3+2.0f*quat.q0*quat.q1)*bz -my)*(2.0f*quat.q1*bx+2.0f*quat.q3*bz)+\
                 ((2.0f*quat.q1*quat.q3+2.0f*quat.q0*quat.q2)*bx +(1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q2*quat.q2)*bz -mz)*(2.0f*quat.q0*bx-4.0f*quat.q2*bz);
    Mid_vec[3] = (2.0f*quat.q1*quat.q3 - 2.0f*quat.q0*quat.q2 - ax)*(2.0f*quat.q1)+\
                 (2.0f*quat.q2*quat.q3 + 2.0f*quat.q0*quat.q1 - ay)*(2.0f*quat.q2)+\
                 ((1.0f-2.0f*quat.q2*quat.q2-2.0f*quat.q3*quat.q3)*bx +(2.0f*quat.q1*quat.q3-2.0f*quat.q0*quat.q2)*bz -mx)*(-4.0f*quat.q3*bx+2.0f*quat.q1*bz)+\
                 ((2.0f*quat.q1*quat.q2-2.0f*quat.q0*quat.q3)*bx   +(2.0f*quat.q2*quat.q3+2.0f*quat.q0*quat.q1)*bz -my)*(-2.0f*quat.q0*bx+2.0f*quat.q2*bz)+\
                 ((2.0f*quat.q1*quat.q3+2.0f*quat.q0*quat.q2)*bx +(1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q2*quat.q2)*bz -mz)*(2.0f*quat.q1*bx);
    #else
    Mid_vec[0] = (2.0f*quat.q1*quat.q3 - 2.0f*quat.q0*quat.q2 - ax)*(-2.0f*quat.q2)+\
                 (2.0f*quat.q2*quat.q3 + 2.0f*quat.q0*quat.q1 - ay)*(2.0f*quat.q1);
    Mid_vec[1] = (2.0f*quat.q1*quat.q3 - 2.0f*quat.q0*quat.q2 - ax)*(2.0f*quat.q3)+\
                 (2.0f*quat.q2*quat.q3 + 2.0f*quat.q0*quat.q1 - ay)*(2.0f*quat.q0)+\
                 (1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q2*quat.q2 - az)*(-4.0f*quat.q1);
    Mid_vec[2] = (2.0f*quat.q1*quat.q3 - 2.0f*quat.q0*quat.q2 - ax)*(-2.0f*quat.q0)+\
                 (2.0f*quat.q2*quat.q3 + 2.0f*quat.q0*quat.q1 - ay)*(2.0f*quat.q3)+\
                 (1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q2*quat.q2 - az)*(-4.0f*quat.q2);
    Mid_vec[3] = (2.0f*quat.q1*quat.q3 - 2.0f*quat.q0*quat.q2 - ax)*(2.0f*quat.q1)+\
                 (2.0f*quat.q2*quat.q3 + 2.0f*quat.q0*quat.q1 - ay)*(2.0f*quat.q2);
    #endif
                 
    /*|dS(Q0)|
    sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3)   */
    Mid_vec_len = my_sqrt(Mid_vec[0]*Mid_vec[0] + Mid_vec[1]*Mid_vec[1] + Mid_vec[2]*Mid_vec[2] + Mid_vec[3]*Mid_vec[3]);
    
    /*((1/2 Q0 * Ws) - b(dS(Q0)/|dS(Q0)|))dt    */
    #define B (0.003f+0.01f*Mid_vec_len)
    #define dT 0.5f
    /*
    TIM5->CR1 &= ~TIM_CR1_CEN;
    dt = (float)TIM5->CNT / 1000000.0f;
    */
    Mid_quat[0] = quat.q0 + (Mid_gyro[0] - B*Mid_vec[0]/Mid_vec_len)*dT;
    Mid_quat[1] = quat.q1 + (Mid_gyro[1] - B*Mid_vec[1]/Mid_vec_len)*dT;
    Mid_quat[2] = quat.q2 + (Mid_gyro[2] - B*Mid_vec[2]/Mid_vec_len)*dT;
    Mid_quat[3] = quat.q3 + (Mid_gyro[3] - B*Mid_vec[3]/Mid_vec_len)*dT;
    /*
    TIM5->CNT = 0;
    TIM5->CR1 |= TIM_CR1_CEN;
    */
    
    quat.q0 = Mid_quat[0];
    quat.q1 = Mid_quat[1];
    quat.q2 = Mid_quat[2];
    quat.q3 = Mid_quat[3];
    
    /*quat to eulra
    roll  = -arcsin(2q1q3-2q0q2)
    pitch = arctan((2q0q1+2q2q3)/(1-2q1q1-2q2q2))
    yaw   = arctan((2q0q3-2q1q2)/(1-2q2q2-2q3q3))   */
    eulra.roll  = -asinf(2.0f*quat.q1*quat.q3-2.0f*quat.q0*quat.q2) *180.f/3.14159f;
    eulra.pitch = atan2((2.0f*quat.q0*quat.q1+2.0f*quat.q2*quat.q3),(1.0f-2.0f*quat.q1*quat.q1-2.0f*quat.q2*quat.q2)) *180.f/3.14159f;
    eulra.yaw   = atan2(-(2.0f*quat.q0*quat.q3-2.0f*quat.q1*quat.q2),(1.0f-2.0f*quat.q2*quat.q2-2.0f*quat.q3*quat.q3)) *180.f/3.14159f;
}
