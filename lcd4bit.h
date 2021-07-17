#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

#define _CONCAT(a,b) a##b
 #define PORT(x) _CONCAT(PORT,x)
 #define PIN(x) _CONCAT(PIN,x)
 #define DDR(x) _CONCAT(DDR,x)
 
#define LCD_DATA D			//Port PB3-PB6 are connected to D4-D7(pin 11 - 14)
#define LCD_DATA_POS 4

#define LCD_E B 			//Enable/strobe signal
#define LCD_E_POS	PB2		//Position of enable in above port

#define LCD_RS B			//RS SIGNAL
#define LCD_RS_POS PB0

#define LCD_RW B			//RW SIGNAL
#define LCD_RW_POS PB1

#define LS_BLINK 0B00000001
#define LS_ULINE 0B00000010
#define LS_NONE	 0B00000000

#define LCD_DATA_PORT 	PORT(LCD_DATA)
#define LCD_E_PORT 		PORT(LCD_E)
#define LCD_RS_PORT 	PORT(LCD_RS)
#define LCD_RW_PORT 	PORT(LCD_RW)

#define LCD_DATA_DDR 	DDR(LCD_DATA)
#define LCD_E_DDR 		DDR(LCD_E)
#define LCD_RS_DDR 		DDR(LCD_RS)
#define LCD_RW_DDR 		DDR(LCD_RW)

#define LCD_DATA_PIN	PIN(LCD_DATA)

#define SET_E() (LCD_E_PORT|=(1<<LCD_E_POS))
#define SET_RS() (LCD_RS_PORT|=(1<<LCD_RS_POS))
#define SET_RW() (LCD_RW_PORT|=(1<<LCD_RW_POS))

#define CLEAR_E() (LCD_E_PORT&=(~(1<<LCD_E_POS)))
#define CLEAR_RS() (LCD_RS_PORT&=(~(1<<LCD_RS_POS)))
#define CLEAR_RW() (LCD_RW_PORT&=(~(1<<LCD_RW_POS)))

#define LCDCmd(c) (LCDByte(c,0))
#define LCDData(d) (LCDByte(d,1))

#define LCDClear() LCDCmd(0b00000001)

unsigned char __cgram[]=
{
0x0C, 0x12, 0x12, 0x0C, 0x00, 0x00, 0x00, 0x00, //Char0
0x00, 0x04, 0x02, 0x1f, 0x02, 0x04, 0x00, 0x00, //Char1
0x0A, 0x15, 0x11, 0x0A, 0x04, 0x00, 0x00, 0x00, //Char2
0x03, 0x05, 0x09, 0x0B, 0x0B, 0x18, 0x18, 0x00, //Char3
0x1F, 0x0E, 0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, //Char4
0x01, 0x01, 0x05, 0x05, 0x15, 0x15, 0x15, 0x00, //Char5
0x0E, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, //Char6


};

void LCDByte(uint8_t c,uint8_t isdata)
{
//Sends a byte to the LCD in 4bit mode
//cmd=0 for data
//cmd=1 for command


//NOTE: THIS FUNCTION RETURS ONLY WHEN LCD HAS PROCESSED THE COMMAND

uint8_t hn,ln;			//Nibbles
uint8_t temp;

hn=c>>4;
ln=(c & 0x0F);

if(isdata==0)
	CLEAR_RS();
else
	SET_RS();

_delay_us(0.500);		//tAS

SET_E();

//Send high nibble

temp=(LCD_DATA_PORT & (~(0X0F<<LCD_DATA_POS)))|((hn<<LCD_DATA_POS));
LCD_DATA_PORT=temp;

_delay_us(1);			//tEH

//Now data lines are stable pull E low for transmission

CLEAR_E();

_delay_us(1);

//Send the lower nibble
SET_E();

temp=(LCD_DATA_PORT & (~(0X0F<<LCD_DATA_POS)))|((ln<<LCD_DATA_POS));

LCD_DATA_PORT=temp;

_delay_us(1);			//tEH

//SEND

CLEAR_E();

_delay_us(1);			//tEL

LCDBusyLoop();
}

void LCDBusyLoop()
{
	//This function waits till lcd is BUSY

	uint8_t busy,status=0x00,temp;

	//Change Port to input type because we are reading data
	LCD_DATA_DDR&=(~(0x0f<<LCD_DATA_POS));

	//change LCD mode
	SET_RW();		//Read mode
	CLEAR_RS();		//Read status

	//Let the RW/RS lines stabilize

	_delay_us(0.5);		//tAS

	
	do
	{

		SET_E();

		//Wait tDA for data to become available
		_delay_us(0.5);

		status=(LCD_DATA_PIN>>LCD_DATA_POS);
		status=status<<4;

		_delay_us(0.5);

		//Pull E low
		CLEAR_E();
		_delay_us(1);	//tEL

		SET_E();
		_delay_us(0.5);

		temp=(LCD_DATA_PIN>>LCD_DATA_POS);
		temp&=0x0F;

		status=status|temp;

		busy=status & 0b10000000;

		_delay_us(0.5);
		CLEAR_E();
		_delay_us(1);	//tEL
	}while(busy);

	CLEAR_RW();		//write mode
	//Change Port to output
	LCD_DATA_DDR|=(0x0F<<LCD_DATA_POS);

}

void LCDInit(uint8_t style)
{
	/*****************************************************************
	
	This function Initializes the lcd module
	must be called before calling lcd related functions

	Arguments:
	style = LS_BLINK,LS_ULINE(can be "OR"ed for combination)
	LS_BLINK :The cursor is blinking type
	LS_ULINE :Cursor is "underline" type else "block" type

	*****************************************************************/
	
	//After power on Wait for LCD to Initialize
	_delay_ms(30);
	
	//Set IO Ports
	LCD_DATA_DDR|=(0x0F<<LCD_DATA_POS);
	LCD_E_DDR|=(1<<LCD_E_POS);
	LCD_RS_DDR|=(1<<LCD_RS_POS);
	LCD_RW_DDR|=(1<<LCD_RW_POS);

	LCD_DATA_PORT&=(~(0x0F<<LCD_DATA_POS));
	CLEAR_E();
	CLEAR_RW();
	CLEAR_RS();

	//Set 4-bit mode
	_delay_us(0.3);	//tAS

	SET_E();
	LCD_DATA_PORT|=((0b00000010)<<LCD_DATA_POS); //[B] To transfer 0b00100000 i was using LCD_DATA_PORT|=0b00100000
	_delay_us(1);
	CLEAR_E();
	_delay_us(1);
	
	//Wait for LCD to execute the Functionset Command
	LCDBusyLoop();                                    //[B] Forgot this delay

	//Now the LCD is in 4-bit mode

	LCDCmd(0b00001100|style);	//Display On
	LCDCmd(0b00101000);			//function set 4-bit,2 line 5x7 dot format

	/* Custom Char */
	LCDCmd(0b01000000);

	uint8_t __i;
	for(__i=0;__i<sizeof(__cgram);__i++)
		LCDData(__cgram[__i]);
	
	LCDGotoXY(0,0);

}
void LCDWriteString(const char *msg)
{
	/*****************************************************************
	
	This function Writes a given string to lcd at the current cursor
	location.

	Arguments:
	msg: a null terminated string to print

	Their are 8 custom char in the LCD they can be defined using
	"LCD Custom Character Builder" PC Software. 

	You can print custom character using the % symbol. For example
	to print custom char number 0 (which is a degree symbol), you 
	need to write

		LCDWriteString("Temp is 30%0C");
		                          ^^
								  |----> %0 will be replaced by
								  		custom char 0.

	So it will be printed like.
		
		Temp is 30°C
		
	In the same way you can insert any syblom numbered 0-7 	


	*****************************************************************/
 while(*msg!='\0')
 {
 	//Custom Char Support
	if(*msg=='%')
	{
		msg++;
		int8_t cc=*msg-'0';

		if(cc>=0 && cc<=7)
		{
			LCDData(cc);
		}
		else
		{
			LCDData('%');
			LCDData(*msg);
		}
	}
	else
	{
		LCDData(*msg);
	}
	msg++;
 }
}

void LCDWriteInt(int val,unsigned int field_length)
{
	/***************************************************************
	This function writes a integer type value to LCD module

	Arguments:
	1)int val	: Value to print

	2)unsigned int field_length :total length of field in which the value is printed
	must be between 1-5 if it is -1 the field length is no of digits in the val

	****************************************************************/

	char str[5]={0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
	str[i]=val%10;
	val=val/10;
	i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
		j=5-field_length;

	if(val<0) LCDData('-');
	for(i=j;i<5;i++)
	{
	LCDData(48+str[i]);
	}
}
void LCDGotoXY(uint8_t x,uint8_t y)
{
 	if(x>=20) return;

	//16x2 LCD type
	
	switch(y)
	{
		case 0:
			break;
		case 1:
			x|=0b01000000;
			break;
		case 2:
			x+=0x14;
			break;
		case 3:
			x+=0x54;
			break;
	}

	x|=0b10000000;
  	LCDCmd(x);
}

/***************************************************
	M A C R O S
***************************************************/

#define LCDHome() LCDCmd(0b00000010);

#define LCDWriteStringXY(x,y,msg) {\
 LCDGotoXY(x,y);\
 LCDWriteString(msg);\
}

#define LCDWriteIntXY(x,y,val,fl) {\
 LCDGotoXY(x,y);\
 LCDWriteInt(val,fl);\
}
/***************************************************/