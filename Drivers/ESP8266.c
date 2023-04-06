#include "ESP8266.h"

//��ʼ��ESP8266, ����
void ESP_Init(void){
    char *strp;
    
    OLED_println((uint8_t *)"close trans");             //�ر�͸��, ׼������ATָ��
    MY_Delay(30);
    MY_UART_Send((uint8_t *)"+++", 3);
    MY_Delay(1000);
    
    RERST:
    OLED_println((uint8_t *)"reset esp");               //�������ESP8266
    MY_UART_Send((uint8_t *)"AT+RST\r\n", sizeof("AT+RST\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, "ready\r\n"))){
        if(NULL != (strp = strstr((const char *)ReceiveBuff, "ERROR\r\n"))){
            *strp = 0;
            OLED_println((uint8_t *)"reset ERROR");     //����ʧ��, ��������
            goto RERST;
        }
    }
    *strp = 0;
    OLED_println((uint8_t *)"ready");                   //�����ɹ�, ׼����ʼ��
    
    OLED_println((uint8_t *)"test CWMODE");             //��ѯWiFiģʽ
    MY_UART_Send((uint8_t *)"AT+CWMODE?\r\n", sizeof("AT+CWMODE?\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
    *strp = 0;
    
    if(NULL == (strp = strstr((const char *)ReceiveBuff, "+CWMODE:1\r\n"))){
        OLED_println((uint8_t *)"changing CWMODE");     //��WiFiģʽ��Ϊ1, ����Ϊ1
        MY_UART_Send((uint8_t *)"AT+CWMODE=1\r\n", sizeof("AT+CWMODE=1\r\n"));
        while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
        *strp = 0;
    }
    OLED_println((uint8_t *)"CWMODE=1");
    
    OLED_println((uint8_t *)"test CIPMODE");            //��ѯ����ģʽ
    MY_UART_Send((uint8_t *)"AT+CIPMODE?\r\n", sizeof("AT+CIPMODE?\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
    *strp = 0;
    
    if(NULL == (strp = strstr((const char *)ReceiveBuff, "+CIPMODE:1\r\n"))){
        OLED_println((uint8_t *)"changing CIPMODE");    //���ô���ģʽΪ͸��
        MY_UART_Send((uint8_t *)"AT+CIPMODE=1\r\n", sizeof("AT+CIPMODE=1\r\n"));
        while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
        *strp = 0;
    }
    
    OLED_println((uint8_t *)"test WiFi");               //��ѯ�Ƿ�����WiFi
    MY_UART_Send((uint8_t *)"AT+CWJAP?\r\n", sizeof("AT+CWJAP?\r\n"));
    while(NULL == (strp = strstr((const char *)ReceiveBuff, "OK\r\n")));
    *strp = 0;
    
    if(NULL != (strp = strstr((const char *)ReceiveBuff, "No AP\r\n"))){
        RECONN:
        OLED_println((uint8_t *)"WiFi not conn");       //δ����WiFi
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
