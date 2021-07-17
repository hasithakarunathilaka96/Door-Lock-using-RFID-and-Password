#include <avr/io.h>
#include <util/delay.h>
#include "keypad.h"
#include "LCD16x2_4bit.h"

unsigned char epw[4];								// enter passsword
unsigned char cpw[] = {'0','0','0','0'};			// coreect passsword
unsigned char admincpw[] = {'1','2','3','4'};		// admin coreect passsword
unsigned char user1cpw[] = {'1','1','1','1'};		// user01 coreect passsword
unsigned char user2cpw[] = {'2','2','2','2'};		//  user01 coreect passsword
unsigned char rpw[4];								// reset passsword
unsigned char pw;
unsigned int i = 0;

void key();
void password();
void check_status();
void reset_password_check1();
void reset_password_check2();
void reset_password_check();

void admin();
void user1();
void user2();

void admin_reset();
void admin_reset_password_check();
void admin_check_password();
void user1_reset_password_check();
void user2_reset_password_check();

int main(void){

	LCD_Init();
	LCD_String_xy(1,4,"Welcome");
		LCD_String_xy(2,1,"Embedded World");
	_delay_ms(3000);
	
	while(1){
		i=0;
		LCD_Init();
		LCD_String_xy(1,0," Show Your Card ");
	
		key();
		
		if(pw != " " && pw == 'A'){
			check_status();
		}else if(pw != " " && pw == 'B'){
			reset_password_check();
		}else if(pw != " " && pw == 'D'){
			user1();
		}else if(pw != " " && pw == 'C'){
			admin();
		}else if(pw != " " && pw == '#'){
			user2();
		}
	}
	
}

void key(){
	pw = keyfind();
}

void password(){

	LCD_String_xy(1,0,"Enter Password ");
	key();
	
	if(pw != "" && i < 4 && pw != 'A' && pw != 'B' && pw != 'D' && pw != '*' && pw != '#' && pw != 'C'){
		LCD_String_xy(2,i,"*");
		epw[i] = pw;
		i++;
	}else if(pw != " " && pw == 'C'){
		i--;
		LCD_String_xy(2,i," ");
		if(i>10){i=0;}
	}
}

void check_status(){

	LCD_Init();
	
	while(i<4){
		password();
	}
	while(1){
		key();
		if(pw != " " && epw[0] == cpw[0] && epw[1] == cpw[1] && epw[2] == cpw[2] && epw[3] == cpw[3] && pw == 'A'){
			LCD_Init();
			LCD_String_xy(1,0,"Door Opened ");
			DDRB=0xff;
			PORTB=0x01;
			_delay_ms(5000);
			PORTB=0x00;
			break;
		}else if(pw != " " && pw == 'A'){
			LCD_Init();
			LCD_String_xy(1,0,"Wrong Password ");
			LCD_String_xy(2,0,"ReEnter Password ");
			_delay_ms(1000);
			LCD_Init();
			LCD_String_xy(1,0,"ReEnter Password ");
			i=0;
			check_status();	
		}
	}
}

void reset_password_check1(){

	LCD_Init();
	LCD_String_xy(1,0,"Enter New");
	LCD_String_xy(2,0,"Password ");
	_delay_ms(1000);
	LCD_Init();
	LCD_String_xy(1,0,"New Password");
	
	i=0;

	while(1){
		key();
		
		if(pw != "" && i < 4 && pw != 'A' && pw != 'B' && pw != 'D' && pw != '*' && pw != '#' && pw != 'C'){
			LCD_String_xy(2,i,"*");
			rpw[i] = pw;
			i++;
		}else if(pw != " " && pw == 'C'){
			i--;
			LCD_String_xy(2,i," ");
			if(i>10){i=0;}
		}else if (pw != " " && pw == 'A' && i==4){
		i=0;
		reset_password_check2();
		break;
		} 
	}
} 

void reset_password_check2(){

	LCD_Init();
	LCD_String_xy(1,0,"Re-Enter New");
	LCD_String_xy(2,0,"Password ");
	_delay_ms(1000);
	LCD_Init();
	LCD_String_xy(1,0,"New Password");

	while(1){

		key();
		
		if(pw != "" && i < 4 && pw != 'A' && pw != 'B' && pw != 'D' && pw != '*' && pw != '#' && pw != 'C'){
			LCD_String_xy(2,i,"*");
			epw[i] = pw;
			i++;
		}else if(pw != " " && pw == 'C'){
			i--;
			LCD_String_xy(2,i," ");
			if(i>10){i=0;}
		}else if (pw != " " && pw == 'A' && i==4){
		break;
		}
	}
}

void reset_password_check(){

	reset_password_check1();
	
	if(pw != " " && epw[0] == rpw[0] && epw[1] == rpw[1] && epw[2] == rpw[2] && epw[3] == rpw[3] && pw == 'A'){
		unsigned j=0;
		for(j=0;j<4;j++){
			cpw[j] = rpw[j];
		}
		LCD_Init();
		LCD_String_xy(1,0,"Password Reset");
		LCD_String_xy(2,0,"Done ");
		DDRB=0xff;
		PORTB=0x01;
		_delay_ms(3000);
		PORTB=0x00;
	}
	else if(pw != " " && pw == 'A'){
		LCD_Init();
		LCD_String_xy(1,0,"Wrong New");
		LCD_String_xy(2,0,"ReEnter Password ");
		_delay_ms(3000);
		LCD_Init();
		LCD_String_xy(1,0,"ReEnter Password ");
		i=0;
		reset_password_check();;
	}

}

void admin(){
	LCD_Init();
	LCD_String_xy(1,0,"Press 1");
	LCD_String_xy(2,0,"Open Door");
	_delay_ms(3000);
	LCD_Init();
	LCD_String_xy(1,0,"Press 2");
	LCD_String_xy(2,0,"Reset Passwords");
	_delay_ms(3000);
	LCD_Init();
	LCD_String_xy(1,0,"Press 3");
	LCD_String_xy(2,0,"Lock Door");
	_delay_ms(3000);
	LCD_Init();
	LCD_String_xy(1,0,"Enter");
	
	while(1){
		key();
		if(pw != "" &&  pw == '1'){
			LCD_Init();
			LCD_String_xy(1,0,"Admin11 ");
			_delay_ms(3000);
			admin_check_password();
			break;
		}else if(pw != " " && pw == '2'){
			LCD_Init();
			LCD_String_xy(1,0,"Admin22 ");
			_delay_ms(3000);
			admin_reset();
			break;
		}else if (pw != " " && pw == '3'){
			LCD_Init();
			LCD_String_xy(1,0,"Admin33 ");
			_delay_ms(3000);
			
			break;
		} 
	}
}

void user1(){

	LCD_Init();
	
	while(i<4){
		password();
	}
	while(1){
		key();
		if(pw != " " && epw[0] == user1cpw[0] && epw[1] == user1cpw[1] && epw[2] == user1cpw[2] && epw[3] == user1cpw[3]){
			LCD_Init();
			LCD_String_xy(1,0,"Door Opened ");
			DDRB=0xff;
			PORTB=0x01;
			_delay_ms(5000);
			PORTB=0x00;
			break;
		}else {
			LCD_Init();
			LCD_String_xy(1,0,"Wrong Password ");
			LCD_String_xy(2,0,"ReEnter Password ");
			_delay_ms(1000);
			LCD_Init();
			LCD_String_xy(1,0,"ReEnter Password ");
			i=0;
			user1();	
		}
	}
}

void user2(){

	LCD_Init();
	
	while(i<4){
		password();
	}
	while(1){
		key();
		if(pw != " " && epw[0] == user2cpw[0] && epw[1] == user2cpw[1] && epw[2] == user2cpw[2] && epw[3] == user2cpw[3]){
			LCD_Init();
			LCD_String_xy(1,0,"Door Opened ");
			DDRB=0xff;
			PORTB=0x01;
			_delay_ms(5000);
			PORTB=0x00;
			break;
		}else {
			LCD_Init();
			LCD_String_xy(1,0,"Wrong Password ");
			LCD_String_xy(2,0,"ReEnter Password ");
			_delay_ms(1000);
			LCD_Init();
			LCD_String_xy(1,0,"ReEnter Password ");
			i=0;
			user2();	
		}
	}
}


void admin_reset(){
	LCD_Init();
	LCD_String_xy(1,0,"Press 1 Reset");
	LCD_String_xy(2,0,"Admin Password");
	_delay_ms(3000);
	LCD_Init();
	LCD_String_xy(1,0,"Press 2 Reset");
	LCD_String_xy(2,0,"User01 Password");
	_delay_ms(3000);
	LCD_Init();
	LCD_String_xy(1,0,"Press 3 Reset");
	LCD_String_xy(2,0,"User02 Password");
	_delay_ms(3000);
	LCD_Init();
	LCD_String_xy(1,0,"Enter");
	
	while(1){
		key();
		if(pw != "" &&  pw == '1'){
			LCD_Init();
			LCD_String_xy(1,0,"Admin Password");
			_delay_ms(3000);
			admin_reset_password_check();
			break;
		}else if(pw != " " && pw == '2'){
			LCD_Init();
			LCD_String_xy(1,0,"User1 Password");
			_delay_ms(3000);
			user1_reset_password_check();
			break;
		}else if (pw != " " && pw == '3'){
			LCD_Init();
			LCD_String_xy(1,0,"User2 Password");
			_delay_ms(3000);
			user2_reset_password_check();
			break;
		} 
	}
}

void admin_check_password(){

	LCD_Init();
	
	while(i<4){
		password();
	}
	while(1){
		key();
		if(pw != " " && epw[0] == admincpw[0] && epw[1] == admincpw[1] && epw[2] == admincpw[2] && epw[3] == admincpw[3] ){
			LCD_Init();
			LCD_String_xy(1,0,"Door Opened ");
			DDRB=0xff;
			PORTB=0x01;
			_delay_ms(5000);
			PORTB=0x00;
			break;
		}else {
			LCD_Init();
			LCD_String_xy(1,0,"Wrong Password ");
			LCD_String_xy(2,0,"ReEnter Password ");
			_delay_ms(1000);
			LCD_Init();
			LCD_String_xy(1,0,"ReEnter Password ");
			i=0;
			admin_check_password();	
		}
	}
}

void admin_reset_password_check(){

	reset_password_check1();
	
	if(pw != " " && epw[0] == rpw[0] && epw[1] == rpw[1] && epw[2] == rpw[2] && epw[3] == rpw[3] ){
		unsigned j=0;
		for(j=0;j<4;j++){
			admincpw[j] = rpw[j];
		}
		LCD_Init();
		LCD_String_xy(1,0,"Password Reset");
		LCD_String_xy(2,0,"Done ");
		DDRB=0xff;
		PORTB=0x01;
		_delay_ms(3000);
		PORTB=0x00;	
	}
	else {
		LCD_Init();
		LCD_String_xy(1,0,"Wrong New");
		LCD_String_xy(2,0,"ReEnter Password ");
		_delay_ms(3000);
		LCD_Init();
		LCD_String_xy(1,0,"ReEnter Password ");
		i=0;
		reset_password_check();
	}
}

void user1_reset_password_check(){

	reset_password_check1();
	
	if(pw != " " && epw[0] == rpw[0] && epw[1] == rpw[1] && epw[2] == rpw[2] && epw[3] == rpw[3] ){
		unsigned j=0;
		for(j=0;j<4;j++){
			user1cpw[j] = rpw[j];
		}
		LCD_Init();
		LCD_String_xy(1,0,"Password Reset");
		LCD_String_xy(2,0,"Done ");
		DDRB=0xff;
		PORTB=0x01;
		_delay_ms(3000);
		PORTB=0x00;	
	}
	else {
		LCD_Init();
		LCD_String_xy(1,0,"Wrong New");
		LCD_String_xy(2,0,"ReEnter Password ");
		_delay_ms(3000);
		LCD_Init();
		LCD_String_xy(1,0,"ReEnter Password ");
		i=0;
		reset_password_check();
	}
}


void user2_reset_password_check(){

	reset_password_check1();
	
	if(pw != " " && epw[0] == rpw[0] && epw[1] == rpw[1] && epw[2] == rpw[2] && epw[3] == rpw[3] ){
		unsigned j=0;
		for(j=0;j<4;j++){
			user2cpw[j] = rpw[j];
		}
		LCD_Init();
		LCD_String_xy(1,0,"Password Reset");
		LCD_String_xy(2,0,"Done ");
		DDRB=0xff;
		PORTB=0x01;
		_delay_ms(3000);
		PORTB=0x00;	
	}
	else {
		LCD_Init();
		LCD_String_xy(1,0,"Wrong New");
		LCD_String_xy(2,0,"ReEnter Password ");
		_delay_ms(3000);
		LCD_Init();
		LCD_String_xy(1,0,"ReEnter Password ");
		i=0;
		reset_password_check();
	}
}
