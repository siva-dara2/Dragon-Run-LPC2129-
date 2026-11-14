#include<LPC21XX.H>
#include<stdlib.h>
#include"lcd.h"
#include<string.h>

#define ROAD_LEN 20
#define MIN_GAP 4
#define EXTRA_GAP 5
#define SW_PS (IOPIN0&(1<<10))

void GAME_START(void);
void CREATE_SYMBOL(void);
void DISPLAY_ROAD(void);
void DISPLAY_SUN(void);
void GAME_OVER(int);
void SET_ROAD(void);
void DISPLAY_SCORE(unsigned int);
void CLEAR_MSG(void);
void DISPLAY_MSG(void);
signed char NEW_OBSTACLE(void);
int SWITCH_PRESSED(void);
unsigned char comp_lut[] = {
    // 0: DRAGON (unchanged)
    0x02,0x07,0x05,0x1F,0x15,0x1F,0x0A,0x0A,

    // 1: ROAD (unchanged)
    0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,

    // 2: SMALL CACTUS (unchanged)
    0x01,0x05,0x05,0x05,0x07,0x04,0x04,0x1F,

    // 3: SUN LEFT HALF
    0x01,0x09,0x03,0x17,0x17,0x03,0x09,0x01,

    // 4: SUN RIGHT HALF
    0x10,0x12,0x18,0x1D,0x1D,0x18,0x12,0x10,

    // 5: CLOUD LEFT HALF
    0x00,0x00,0x06,0x09,0x10,0x10,0x1F,0x00,

    // 6: CLOUD RIGHT HALF
    0x00,0x00,0x0C,0x12,0x01,0x01,0x1F,0x00,

    // 7: UNUSED
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
unsigned char *msg[]={
	"", "WING ATTACK!", "", "FIRE", "FLY HIGH", "GO go GO",
	"", "HEHEE", "UNSTOPABLE", ""  
};
signed char road[ROAD_LEN];
char dragon=1;
unsigned int score=0;
char msgval=2;

int main(){
	LCD_INIT();
	CREATE_SYMBOL();
	memset(road,0,ROAD_LEN);
	GAME_START();
	DISPLAY_SUN();
	DISPLAY_ROAD();
	T0TCR=0X01;	  				//TO ENABLE THE T0TC AND T0PC
	srand(T0TC);				//TO OPEN A RANDOM PAGE TO GET RANDOM NUMBERS
	while(1){
	int i=0;
	if(score==9999){	//IF THE SCORE REACHES ITS MAXIMUM VALUE THAN 
	GAME_OVER(score);	//TO DISPLAY THE SCORE AND CLOSE THE GAME
	break;
	}
	DISPLAY_SCORE(score);
	SET_ROAD();			//CONTINOUSLY UPDATING THE ARRAY VALUES WITH THE ROAD AND OBSTACLES
	
	if(dragon+road[0]==0){	//TO CHECK WEATHER THE UPDATED OBSTACLE VALUES ARE COLLIDED WITH DRAGON
	GAME_OVER(score);		//THAN CALL GAME_OVER FUNCTION AND DISPLAY THE SCORE
	break;
	}
	DISPLAY_ROAD();		//TO DISPLAY THE CHARACTER AND ROAD	
	if(dragon==2)dragon=1;
	for(;i<25;i++)
	{
		delay_ms(20);
		if(SWITCH_PRESSED()){
		dragon=2;
		}
	}
	score++;
	} 
	
}

void GAME_START(void){
	int i=3;
	LCD_COMMAND(0XC4);		//TO SELECT SECOND ROW FIFTH COLUMN
	LCD_STR("GAME STARTS"); //TO DISPLAY THE STRING "GAME STARTS"
	while(i){
	LCD_COMMAND(0X9C);      //TO SELECT THIRD ROW NINTH COLUMN
	LCD_INT(i--);			//TO DISPLAY THE VALUE 3 2 1
	delay_ms(1000);          
	}
	LCD_COMMAND(0X01);      //TO CLEAR THE DISPLAY
}

void CREATE_SYMBOL(void){
	unsigned char i;
	LCD_COMMAND(0X40);		  //TO SELECT THE CGRAM STARTING ADDRESS
	for(i=0;i<56;i++){		  
	LCD_DATA(comp_lut[i]);	  //TO WRITE INTO THE CGRAM MEMORY BYTE BY BYTE
	}
}

void DISPLAY_ROAD(void){
	unsigned char i=0xD4;       //TO SELECT FOUTH ROW FIRST COLUMN
	LCD_COMMAND(i);
	if(dragon==1){		        //IF THE DRAGON VALUE IS 1 THAN IN FIRST COLUMN PRINT THE DRAGON
	LCD_DATA(0);
	}else{
	LCD_DATA(road[0]==0? 1: 2); //IF NOT PRINT THE OBSTACLE ON THE FIRST COLUMN
	}
	for(i=1;i<ROAD_LEN;i++){
	LCD_DATA(road[i]==0? 1: 2);	//PRINT REMAINING ALL THE ROAD OR OBSTACLES
	}
	LCD_COMMAND(0X94);
	if(dragon==2){
	LCD_DATA(0);//IF THE SWITCH IS PRESSED DISPLAY THE DRAGON ON THE THIRD ROW FIRST COLUMN
	DISPLAY_MSG();
	msgval=2;
	}else{
	LCD_DATA(' ');
	if(msgval==0)CLEAR_MSG();
	if(msgval>0)msgval--;
	}
}

void DISPLAY_MSG(void){
	LCD_COMMAND(0XC1);
	LCD_STR(msg[score%10]);
}

void CLEAR_MSG(void){
	unsigned char i;
	LCD_COMMAND(0XC1);
	for(i=0;i<18;i++)LCD_DATA(' ');
}

void GAME_OVER(int score){
	delay_ms(250);
	LCD_COMMAND(0X01);	 //TO CLEAR THE DISPLAY
	LCD_COMMAND(0XC4);   //TO SELECT THE SECOND ROW FOURTH COLUMN
	LCD_STR("GAME OVER");//TO DISPLAY GAME OVER STRING ON LCD
	LCD_COMMAND(0X9B);	 //TO SELECT THE THIRD ROW EIGHTH COLUMN
	LCD_INT(score);		 //TO DISPLAY THE SCORE ON LCD
	delay_ms(1000);	
}

void SET_ROAD(){
	signed char i=0;
	for(i=0;i<ROAD_LEN-1;i++){
	road[i]=road[i+1];	             //TO LEFT SHIFT THE VALUES OF THE ARRAY OF COLUMN VALUES
	}
	road[ROAD_LEN-1]=NEW_OBSTACLE(); //TO GENERATE A NEW OBSTACLE
}

signed char NEW_OBSTACLE(void){
	static signed char gap=MIN_GAP;  //TO ASSIGN MIN GAP TO GAP AND 
	if(gap>0){
	gap--;
	return 0;				         //UNTIL GAP GETS 0 ALWAYS RETURN 0
	}
	gap=MIN_GAP+(rand()%EXTRA_GAP);	 //AFTER GAP BECOMES 0 GENERATE A NEW GAP 
	return -1;						 //SO NOW RETURN -1 AS A OBSTACLE
}

int SWITCH_PRESSED(){
	if(SW_PS==0){
		return 1;
	}else{
		return 0;
	}
}

void DISPLAY_SUN(void){
	
	LCD_COMMAND(0X80);
	LCD_DATA(3);
	LCD_DATA(4);

	LCD_COMMAND(0X8C);
	LCD_DATA(5);
	LCD_DATA(6);
}

void DISPLAY_SCORE(unsigned int n){
	
	LCD_COMMAND(0X90);
	LCD_INT(n);
}










