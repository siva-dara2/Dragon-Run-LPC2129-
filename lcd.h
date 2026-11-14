#ifndef lcd_h
#define lcd_h
#include<lpc21xx.h>
#include"timers.h"
#define LCD_D 0xff
#define RS 1<<8
#define E 1<<9

void LCD_INIT(void);
void LCD_STR(char *p);
void LCD_COMMAND(unsigned char);
void LCD_DATA(unsigned char);
void LCD_INT(int);
void LCD_FLOAT(float);

void LCD_INIT(void)
{
    PINSEL0 &= ~0x000FFFFF;

    IODIR0 |= 0x000003FF;

    IOCLR0 = 0x000003FF;

    delay_ms(30);

    LCD_COMMAND(0x38);  // 8-bit, 2-line, 5x8 font
    delay_ms(5);

    LCD_COMMAND(0x0C);  // Display ON, cursor OFF
    delay_ms(5);

    LCD_COMMAND(0x06);  // Increment cursor
    delay_ms(5);

    LCD_COMMAND(0x01);  // Clear display
    delay_ms(5);
}

void LCD_COMMAND(unsigned char cmd){
	IOCLR0 = LCD_D;
	IOSET0 = cmd;
	IOCLR0 = RS;
	IOSET0 = E;
	delay_ms(2);
	IOCLR0 = E;
}

void LCD_DATA(unsigned char data){
	IOCLR0 = LCD_D;
	IOSET0 = data;
	IOSET0 = RS;
	IOSET0 = E;
	delay_ms(2);
	IOCLR0 = E;
}

void LCD_INT(int n){
	int i=0;
	int arr[15];
	if(n==0){
		LCD_DATA('0');
	}else if(n<0){
		LCD_DATA('-');
		n=(~n)+1;
	}
	while(n){
		arr[i++]=n%10;
		n/=10;
	}
	i--;
	while(i>=0){
		LCD_DATA(arr[i]+'0');
		i--;
	}
}

void LCD_STR(char *p){
	while(*p){
		if(*p=='\n'){
			LCD_COMMAND(0xC0);
			p++;
		}
		LCD_DATA(*p++);
	}
}

void LCD_FLOAT(float fun){

	LCD_INT((int)fun);
	LCD_DATA('.');
	fun=fun-(int)fun;
	fun*=10;
	while(fun){
	LCD_INT((int)fun);
	fun=fun-(int)fun;
	fun*=10;
	}
}
#endif
