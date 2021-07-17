
#define  F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#define KEY_PRT 	PORTC
#define KEY_DDR		DDRC
#define KEY_PIN		PINC


char keyfind();

unsigned char keypad[4][4] = {	{'1','4','7','*'},
								{'2','5','8','0'},
								{'3','6','9','#'},
								{'A','B','C','D'}};


unsigned char colloc, rowloc;


char keyfind()
{
	while(1)
	{
		KEY_DDR = 0xF0;
		KEY_PRT = 0xFF;

		do
		{
			KEY_PRT &= 0x0F;
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
		}while(colloc != 0x0F);
		
		do
		{
			do
			{
				_delay_ms(20);						//20ms key debounce time
				colloc = (KEY_PIN & 0x0F);
			}while(colloc == 0x0F);
			
			_delay_ms (40);							//20 ms key debounce time
			colloc = (KEY_PIN & 0x0F);
		}while(colloc == 0x0F);

		KEY_PRT = 0xEF;
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 0;
			break;
		}

		KEY_PRT = 0xDF;
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 1;
			break;
		}
		
		KEY_PRT = 0xBF;
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 2;
			break;
		}

		KEY_PRT = 0x7F;
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 3;
			break;
		}
	}

	
	if(colloc == 0x0E)
	return(keypad[rowloc][0]);
	
	else if(colloc == 0x0D)
	return(keypad[rowloc][1]);
	
	else if(colloc == 0x0B)
	return(keypad[rowloc][2]);
	
	else
	return(keypad[rowloc][3]);
}


