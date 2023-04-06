#include "OLED.h"
#include "MyI2C.h"
#include "zimo.h"

uint8_t DRAM[8][128] = {0};

//OLED屏幕初始化
void OLED_Init(void){
    MY_I2C_Send(OLED_ADDR, (uint32_t)OLED_INITCMD, sizeof(OLED_INITCMD));
    OLED_All(0);
    OLED_ClnAllDRAM();
    OLED_ResetOffset();
}

/*****對OLED內存進行讀寫*****/

//OLED屏幕全部點亮或熄滅
//state: 屏幕狀態
//    0: 熄滅
// 其他: 點亮
void OLED_All(uint8_t state){
    uint32_t cnt = 1024;
    
    if(state)
        state = 0xFF;
    
    MY_I2C_Send(OLED_ADDR, (uint32_t)OLEC_CLRALLCMD, sizeof(OLEC_CLRALLCMD));
    
    MY_I2C_Start(OLED_ADDR & ~1);
    I2C1->DR = 0x40;
    while(cnt > 0){
        while(!(I2C1->SR1 & I2C_SR1_TXE));
        I2C1->DR = state;
        cnt--;
    }
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->CR1 |= I2C_CR1_STOP;
}

//將DRAM裡的數據寫入顯存
//x: 初始列地址 (0~127)
//y: 初始行地址 (0~63)
//w: 寫入寬度
//h: 寫入高度
void OLED_WriteRAM(uint8_t x, uint8_t y, uint8_t w, uint8_t h){
    uint8_t x2, y2, cmd[] = {0x00, 0x21, 0x00, 0x00, 0x22, 0x00, 0x00};
	
    cmd[2] = x &0x7F, cmd[3] = (x +w -1) &0x7F, cmd[5] = (y & 0x3F) /8, cmd[6] = ( (y +h -1) &0x3F) /8;
    MY_I2C_Send(OLED_ADDR, (uint32_t)cmd, sizeof(cmd));
    
    MY_I2C_Start(OLED_ADDR & ~1);
    I2C1->DR = 0x40;
    for(y2 = cmd[5]; y2 <= cmd[6]; y2++){
        for(x2 = cmd[2]; x2 <= cmd[3]; x2++){
            while(!(I2C1->SR1 & I2C_SR1_TXE));
            I2C1->DR = DRAM[y2][x2];
        }
    }
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    SET_BIT(I2C1->CR1, I2C_CR1_STOP);
}

/******對緩存進行讀寫******/

//OLED向DRAM中画点
// x, y:  位置
//state: 点状态
void OLED_Point(uint8_t x, uint8_t y, uint8_t state){
    x &= 0x7F;
    y &= 0x3F;
    
    if(state)
        DRAM[y/8][x] |= 1 << (y%8);
    else
        DRAM[y/8][x] &= ~(1 << (y%8));
}


//DRAM全部清零
//無
void OLED_ClnAllDRAM(void){
	uint8_t i, j;
	for(i = 0; i < 8; i++){
		for(j = 0; j < 128; j++){
			DRAM[i][j] = 0;
		}
	}
}


//DRAM全部清零
//Col1, Col2:   起始列, 终止列 (0~127)
//Page1, Page2: 起始页, 终止页 (0~7)
void OLED_ClnDRAM(uint8_t Col1, uint8_t Page1, uint8_t Col2, uint8_t Page2){
	uint8_t i, j;
	for(i = Page1; i <= Page2; i++){
		for(j = Col1; j <= Col2; j++){
			DRAM[i][j] = 0;
		}
	}
}

//向內存裡寫方塊
//  x, y: 方塊起點
//x1, x2: 方塊大小
//   bit: 點狀態
void OLED_DT(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint8_t bit){
	uint8_t xcnt, ycnt;
	
	x &= 0x7F;
	y &= 0x3F;
	x1 = x+x1 < 0x7F ? x1 : 0x7F-x;
	y1 = y+y1 < 0x3F ? y1 : 0x3F-y;
	
	if(bit)
		for(ycnt=0; ycnt < y1; ycnt++)
			for(xcnt=0; xcnt < x1; xcnt++)
				DRAM[(y+ycnt) / 8][x+xcnt] |= 1 << ((y+ycnt) % 8);
	
	else
		for(ycnt=0; ycnt < y1; ycnt++)
			for(xcnt=0; xcnt < x1; xcnt++)
				DRAM[(y+ycnt) / 8][x+xcnt] &= ~(1 << ((y+ycnt) % 8));
}

//向內存裡寫字符的字模
//x, y: 起始點坐標
//  zi: 單個字模首地址
void OLED_SC(uint8_t x, uint8_t y, uint8_t h, uint8_t w, const uint8_t *zi, uint8_t n){
	uint8_t i, j;
	for(j = 0; j < h; j++){
		for(i = 0; i < w; i++){
			OLED_DT(x + i*n, y + j*n, n, n, (*(zi + i + j / 8 * w)>>(j % 8) & 1) );
		}
	}
	OLED_WriteRAM(x, y, w*n, h*n);
}

//向內存裡寫一串字符串的字模
//x, y:	起始點坐標
// str: 字符串首地址
//ziku: 字模字庫
//   n: 縮放比例
void OLED_SS(uint8_t x, uint8_t y, uint8_t *str, uint32_t ziku, uint8_t n){
	uint8_t temp, h, w;
	
//根據所選字庫調整單字寬度與高度
	if(ziku == (uint32_t)&zimo)
		w = 8, h = 16;
	if(ziku == (uint32_t)&hanzi)
		w = 32, h = 32;
	if(ziku == (uint32_t)&Atali)
		w = 8, h = 8;
	
//超出下界不寫入顯存
	if(y + h > 64)
		return;
	
//將字模寫入顯存, 超過邊界部分捨棄
	while(*str != 0){
		temp = *str++;
		if(ziku == (uint32_t)&Atali){
			if(temp <= 57 && temp >= 48)
				temp = temp - '0' + ATL_NUM_BASE;
			else if(temp >= 97 && temp <= 122)
				temp = temp - 'a' + ATL_CHAR_BASE;
			else if(temp >= 65 && temp <= 90)
				temp = temp - 'A' + ATL_CHAR_BASE;
            else if(temp == 45)
                temp = 37;
            else if(temp == 58)
                temp = 38;
			else
				temp = 0;
		}
		else
			temp -= ' ';
		OLED_SC(x, y, h, w, (uint8_t *)(ziku + h * temp), n);
		x += w*n;
		if(x + w*n > 128){
			if(y + h*n > 64)
				break;
			y += h*n;
			x = 0;
		}
	}
}

void itoa(int32_t num, uint8_t *buf){
    static uint8_t cnt;
    
    if(num /10){
        itoa(num /10, buf);
    }
    else{
        cnt = 0;
    }
    *(buf+cnt) = num %10 +'0';
    cnt++;
}

//show a Number in dec
//x, y: position
// num: Number
//ziku: typeface
//   n: Size
void OLED_ShowNum(uint8_t x, uint8_t y, int32_t num, uint32_t ziku, uint8_t n, uint8_t len){
	uint8_t temp[9] = "        ";
	
    if(num < 0){
        temp[0] = '-';
        itoa(-num, temp +1);
    }
    else if(num == 0){
        temp[0] = '0';
    }
    else if(num > 0){
        itoa(num, temp);
    }
    
    temp[len] = 0;
	OLED_SS(x, y, temp, ziku, n);
}

void OLED_println(uint8_t *str){
    static uint8_t line = 0, OLED_MOVDOWN[3] = {0x00,0xD3,0x00};
    uint8_t temp[17] = {0}, cnt = 0;
    
    do{
        cnt = 0;
        if(*str == '\r')
            str = str+2;
        while(cnt <= 16 && *str != '\r' && *str != 0){
            temp[cnt++] = *(str++);
        }
        temp[cnt] = 0;
        OLED_MOVDOWN[2] = (line+1) * 16;
        MY_I2C_Send(OLED_ADDR, (uint32_t)OLED_MOVDOWN, 3);
        OLED_ClnDRAM(0, line * 2, 127, line * 2 + 1);
        OLED_WriteRAM(0, line * 16, 128, 16);
        OLED_SS(0, line * 16, temp, (uint32_t)&zimo, 1);
        line = (line + 1) % 4;
    }while(*str != 0);
}

void OLED_ResetOffset(void){
    MY_I2C_Send(OLED_ADDR, (uint32_t)"\x00\xD3\x00", 3);
}
