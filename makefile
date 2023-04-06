target=test
C_source=*.c
S_source=*.s
object=*.o
include=-I Drivers\ -I Lib\ -I User\ -I "D:\Keil 5\Pack\Keil\STM32F4xx_DFP\2.15.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I
$(target):$(object)
	gcc $(include) -o $@ $^
$(boject):%.o:%.c
	gcc $(include) -c $
