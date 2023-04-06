#include "ESP8266.h"

//初始化ESP8266, 联网
void ESP_Init(void){
    char *strp;
    
    OLED_println((uint8_t *)"close trans");             //关闭透传, 准备发送AT指令
    MY_Delay(30);
    MY_UART_Send((uint8_t *)"+++", 3);
    MY_Delay(1000);
    
    RERST:
    OLED_println((uint8_t *)"reset esp");               //软件重启ESP8266
    MY_UART_Send((uint8_t *)"AT+RST\r\n", sizeof("AT+RST\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, "ready\r\n"))){
        if(NULL != (strp = strstr((const char *)ReceiveBuff, "ERROR\r\n"))){
            *strp = 0;
            OLED_println((uint8_t *)"reset ERROR");     //重启失败, 重新重启
            goto RERST;
        }
    }
    *strp = 0;
    OLED_println((uint8_t *)"ready");                   //重启成功, 准备初始化
    
    OLED_println((uint8_t *)"test CWMODE");             //查询WiFi模式
    MY_UART_Send((uint8_t *)"AT+CWMODE?\r\n", sizeof("AT+CWMODE?\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
    *strp = 0;
    
    if(NULL == (strp = strstr((const char *)ReceiveBuff, "+CWMODE:1\r\n"))){
        OLED_println((uint8_t *)"changing CWMODE");     //若WiFi模式不为1, 设置为1
        MY_UART_Send((uint8_t *)"AT+CWMODE=1\r\n", sizeof("AT+CWMODE=1\r\n"));
        while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
        *strp = 0;
    }
    OLED_println((uint8_t *)"CWMODE=1");
    
    OLED_println((uint8_t *)"test CIPMODE");            //查询传输模式
    MY_UART_Send((uint8_t *)"AT+CIPMODE?\r\n", sizeof("AT+CIPMODE?\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
    *strp = 0;
    
    if(NULL == (strp = strstr((const char *)ReceiveBuff, "+CIPMODE:1\r\n"))){
        OLED_println((uint8_t *)"changing CIPMODE");    //设置传输模式为透传
        MY_UART_Send((uint8_t *)"AT+CIPMODE=1\r\n", sizeof("AT+CIPMODE=1\r\n"));
        while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
        *strp = 0;
    }
    
    OLED_println((uint8_t *)"test WiFi");               //查询是否连接WiFi
    MY_UART_Send((uint8_t *)"AT+CWJAP?\r\n", sizeof("AT+CWJAP?\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
    *strp = 0;
    
    if(NULL != (strp = strstr((const char *)ReceiveBuff, "No AP\r\n"))){
        RECONN:
        OLED_println((uint8_t *)"WiFi not conn");       //未连接WiFi
        MY_UART_Send((uint8_t *)"AT+CWJAP=\"Tokiinu\",\"20020613\"\r\n", sizeof("AT+CWJAP=\"Tokiinu\",\"20020613\"\r\n"));
        OLED_println((uint8_t *)"Connecting");
        while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n"))){
            if(NULL != (strp = strstr((const char *)ReceiveBuff, "FAIL\r\n"))){
                *strp = 0;
                OLED_println((uint8_t *)"connect FAIL");
                goto RECONN;
            }
            if(NULL != (strp = strstr((const char *)ReceiveBuff, "ERROR\r\n"))){
                *strp = 0;
                OLED_println((uint8_t *)"command ERROR");
                goto RECONN;
            }
        }
        *strp = 0;
        OLED_println((uint8_t *)"WiFi Connected");
    }
    
    RESTARTCIP:
    OLED_println((uint8_t *)"Connecting TCP");
    MY_UART_Send((uint8_t *)"AT+CIPSTART=\"UDP\",\"192.168.137.1\",8000\r\n", sizeof("AT+CIPSTART=\"UDP\",\"192.168.137.1\",8000\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n"))){
        if(NULL != (strp = strstr((const char *)ReceiveBuff, "ERROR\r\n"))){
            *strp = 0;
            OLED_println((uint8_t *)"CIP ERROR");
            MY_Delay(1000);
            goto RESTARTCIP;
        }
    }
    *strp = 0;
    OLED_println((uint8_t *)"CIP Connected");
    
    MY_UART_Send((uint8_t *)"AT+CIPSEND\r\n", sizeof("AT+CIPSSEND\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, ">")));
    *strp = 0;
    OLED_println((uint8_t *)"Start Send");
    OLED_All(0);
    OLED_ClnAllDRAM();
    OLED_ResetOffset();
}
