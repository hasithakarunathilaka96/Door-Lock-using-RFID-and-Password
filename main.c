#include <avr/io.h>
#include <util/delay.h>
#include "keypad.h"
#include <lcd4bit.h>
#define F_CPU 2000000UL

unsigned char epw[4];														// enter passsword
unsigned char cpw[] = {'0','0','0','0'};									// coreect passsword
unsigned char admincpw[] = {'1','2','3','4'};								// admin coreect passsword
unsigned char user1cpw[] = {'1','1','1','1'};								// user01 coreect passsword
unsigned char user2cpw[] = {'2','2','2','2'};								// user01 coreect passsword
unsigned char newusercpw[4];												// newuser coreect passsword
unsigned char rpw[4];														// reset passsword
unsigned char pw;
unsigned char i = 0;
unsigned char card_num = 0;
unsigned char data[10];
unsigned char c_admin[10] = {'3','0','0','0','9','2','A','8','7','A'}; 	// Key for card 0009594386
unsigned char c_user1[10] = {'3','0','0','0','9','2','6','7','1','8'}; 	// Key for card 0009594648
unsigned char c_user2[10] = {'3','0','0','0','9','2','7','2','1','B'};	// Key for card 0009594467
unsigned char new_user[10];

void USART_INIT(void);
unsigned char USART_RECEIVE(void);
void card_check(void);

void key(void);
void password(void);
void reset_password_check1(void);
void reset_password_check2(void);
void reset_password_check(void);

void admin(void);
void user1(void);
void user2(void);
void newuser(void);

void admin_reset(void);
void admin_reset_password_check(void);
void admin_check_password(void);
void user1_reset_password_check(void);
void user2_reset_password_check(void);
void newuser_reset_password_check(void);
void enroll_new_card(void);

int main(void){
	
	DDRA=0xff;
	DDRD=0xff;

	LCDInit(0);
	LCDClear();
	LCDWriteStringXY(4,0,"Welcome");
	LCDWriteStringXY(1,1,"Embedded World");
	_delay_ms(1000);
	
	while(1){
	
		i=0;
		LCDClear();
		LCDWriteStringXY(0,0," Show Your Card ");
		
		card_check();
		
		if(card_num==1){
			PORTA |= (1<<7);
			PORTD |= (1<<4);
			_delay_ms(100);
			PORTA &= ~(1<<7);
			PORTD &= ~(1<<4);
			admin();
		}
		else if(card_num==2){;
			PORTA |= (1<<7);
			PORTD |= (1<<5);
			_delay_ms(100);
			PORTA &= ~(1<<7);
			PORTD &= ~(1<<5);
			user1();
		}
		else if(card_num==3){
			PORTA |= (1<<7);
			PORTD |= (1<<6);
			_delay_ms(100);
			PORTA &= ~(1<<7);
			PORTD &= ~(1<<6);
			user2();
		}
		else if(card_num==4){
			PORTA |= (1<<7);
			PORTD |= (1<<7);
			_delay_ms(100);
			PORTA &= ~(1<<7);
			PORTD &= ~(1<<7);
			newuser();
		}
	}
}

///// Detect the RFID card /////
void card_check(void){

	card_num=0;
	unsigned char check = 0; 											//Used to check if head byte is received
	USART_INIT();
	while(1){
		check = USART_RECEIVE();
		if(check == 2){													//check if head byte is received
			for(unsigned char j=0;j<10;j++){
				data[j] = USART_RECEIVE();
			}
			if(data[6]==c_admin[6] && data[7]==c_admin[7] && data[8]==c_admin[8] && data[9]==c_admin[9]){
				card_num=1;
				break;
			}
			if(data[6]==c_user1[6] && data[7]==c_user1[7] && data[8]==c_user1[8] && data[9]==c_user1[9]){
				card_num=2;
				break;
			}
			if(data[6]==c_user2[6] && data[7]==c_user2[7] && data[8]==c_user2[8] && data[9]==c_user2[9]){
				card_num=3;
				break;
			}
			if(data[6]==new_user[6] && data[7]==new_user[7] && data[8]==new_user[8] && data[9]==new_user[9]){
				card_num=4;
				break;
			}
			if(data[9]!=0 ){
				break;
			}
		}
	}
}

///// Initialized the USART/////
void USART_INIT(void){
	UCSRB |= (1<<RXEN)|(1<<TXEN);								// Enable Transmit pin, Receive pin (TXEN=1, RXEN=1)
	UCSRC |= (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);				// select character size(8 bit)
	UBRRL = 12;													// Set baudrate (For 2MHz 9600bps)
}

///// Receiving data using USART /////
unsigned char USART_RECEIVE(void){					
	while(!(UCSRA&(1<<RXC)));									// Wait until data received
	return(UDR);												// return received msg
}

///// Identify the pressed key /////
void key(void){
	pw = keyfind();
}

///// Set password to array /////
void password(void){
	LCDClear();
	LCDWriteStringXY(0,0,"Enter Password ");
	
	while(1){
		key();
		if(i < 4 && pw != 'A' && pw != 'B' && pw != 'D' && pw != '*' && pw != '#' && pw != 'C'){
			LCDWriteStringXY(i,1,"*");
			epw[i] = pw;
			i++;
		}else if(pw == 'C'){
			i--;
			LCDWriteStringXY(i,1," ");;
			if(i>10){i=0;}
		}else if(pw == 'A' && i==4){
		break;
		}
	}
}

///// Check reset password 01 /////
void reset_password_check1(void){

	LCDClear();
	LCDWriteStringXY(0,0,"Enter New");
	LCDWriteStringXY(0,1,"Password ");
	_delay_ms(1000);
	
	LCDClear();
	LCDWriteStringXY(0,0,"New Password");
	
	i=0;

	while(1){
		key();
		
		if(i < 4 && pw != 'A' && pw != 'B' && pw != 'D' && pw != '*' && pw != '#' && pw != 'C'){
			LCDWriteStringXY(i,1,"*");
			rpw[i] = pw;
			i++;
		}else if(pw == 'C'){
			i--;
			LCDWriteStringXY(i,1," ");
			if(i>10){i=0;}
		}else if (pw == 'A' && i==4){
		i=0;
		reset_password_check2();
		break;
		} 
	}
} 

///// Check re enter reset password 2 /////
void reset_password_check2(void){

	LCDClear();
	LCDWriteStringXY(0,0,"Re-Enter New");
	LCDWriteStringXY(0,1,"Password ");
	_delay_ms(1000);
	LCDClear();
	LCDWriteStringXY(0,0,"New Password");

	while(1){

		key();
		
		if(i < 4 && pw != 'A' && pw != 'B' && pw != 'D' && pw != '*' && pw != '#' && pw != 'C'){
			LCDWriteStringXY(i,1,"*");
			epw[i] = pw;
			i++;
		}else if(pw == 'C'){
			i--;
			LCDWriteStringXY(i,1," ");
			if(i>10){i=0;}
		}else if (pw == 'A' && i==4){
		break;
		}
	}
}

///// Set reset password /////
void reset_password_check(void){

	reset_password_check1();
	
	if(epw[0] == rpw[0] && epw[1] == rpw[1] && epw[2] == rpw[2] && epw[3] == rpw[3] && pw == 'A'){
		unsigned j=0;
		for(j=0;j<4;j++){
			cpw[j] = rpw[j];
		}
		LCDClear();
		LCDWriteStringXY(0,0,"Password Reset");
		LCDWriteStringXY(0,1,"Done ");
		_delay_ms(1000);
	}
	else if(pw == 'A'){
		LCDClear();
		LCDWriteStringXY(0,0,"Wrong New");
		LCDWriteStringXY(0,1,"ReEnter Password ");
		_delay_ms(1500);
		LCDClear();
		LCDWriteStringXY(0,0,"ReEnter Password ");
		i=0;
		reset_password_check();;
	}

}

///// Select Admin card functions /////
void admin(void){
	LCDClear();
	LCDWriteStringXY(0,0,"Press 1");
	LCDWriteStringXY(0,1,"Open Door");
	_delay_ms(1000);
	LCDClear();
	LCDWriteStringXY(0,0,"Press 2");
	LCDWriteStringXY(0,1,"Reset Passwords");
	_delay_ms(1000);
	LCDWriteStringXY(0,0,"Press 3");
	LCDWriteStringXY(0,1,"Enroll New Card");
	_delay_ms(1000);
	LCDClear();
	LCDWriteStringXY(0,0,"Enter");
	
	while(1){
		key();
		if(pw == '1'){
			LCDClear();
			LCDWriteStringXY(0,0,"Open Door ");
			_delay_ms(1000);
			admin_check_password();
			break;
		}else if(pw == '2'){
			LCDClear();
			LCDWriteStringXY(0,0,"Reset Passwords ");
			_delay_ms(1000);
			admin_reset();
			break;
		}else if (pw == '3'){
			LCDClear();
			LCDWriteStringXY(0,0,"Enroll New Card ");
			_delay_ms(1000);
			enroll_new_card();
			PORTA |= (1<<7);
			_delay_ms(100);
			PORTA &= ~(1<<7);
			break;
		}
	}
}

//// Check user 1 password /////
void user1(void){

	LCDClear();
	
	while(i<4){
		password();
	}
	
	while(1){
		if(epw[0] == user1cpw[0] && epw[1] == user1cpw[1] && epw[2] == user1cpw[2] && epw[3] == user1cpw[3]){
			LCDClear();
			LCDWriteStringXY(0,0,"Door Opened ");
			PORTA|= (1<<0);
			PORTA|= (1<<3);
			_delay_ms(4000);
			PORTA|= (1<<0);
			PORTA|= (1<<3);
			break;
		}else {
			LCDClear();
			LCDWriteStringXY(0,0,"Wrong Password ");
			LCDWriteStringXY(0,1,"ReEnter Password ");
			_delay_ms(1000);
			LCDClear();
			LCDWriteStringXY(0,0,"ReEnter Password ");
			i=0;
			user1();	
		}
	}
}

//// Check user 2 password /////
void user2(void){

	LCDClear();
	
	while(i<4){
		password();
	}
	
	while(1){
		if(epw[0] == user2cpw[0] && epw[1] == user2cpw[1] && epw[2] == user2cpw[2] && epw[3] == user2cpw[3]){
			LCDClear();
			LCDWriteStringXY(0,0,"Door Opened ");
			PORTA|= (1<<0);
			PORTA|= (1<<4);
			_delay_ms(4000);
			PORTA|= (1<<0);
			PORTA|= (1<<4);
			break;
		}else {
			LCDClear();
			LCDWriteStringXY(0,0,"Wrong Password ");
			LCDWriteStringXY(0,1,"ReEnter Password ");
			_delay_ms(1000);
			LCDClear();
			LCDWriteStringXY(0,0,"ReEnter Password ");
			i=0;
			user2();	
		}
	}
}

//// Check newuser password /////
void newuser(void){

	LCDClear();
	
	while(i<4){
		password();
	}
	
	while(1){
		if(epw[0] == newusercpw[0] && epw[1] == newusercpw[1] && epw[2] == newusercpw[2] && epw[3] == newusercpw[3]){
			LCDClear();
			LCDWriteStringXY(0,0,"Door Opened ");
			PORTA|= (1<<0);
			PORTA|= (1<<5);
			_delay_ms(4000);
			PORTA|= (1<<0);
			PORTA|= (1<<5);
			break;
		}else {
			LCDClear();
			LCDWriteStringXY(0,0,"Wrong Password ");
			LCDWriteStringXY(0,1,"ReEnter Password ");
			_delay_ms(1000);
			LCDClear();
			LCDWriteStringXY(0,0,"ReEnter Password ");
			i=0;
			newuser();	
		}
	}
}

///// Select user for reset password /////
void admin_reset(void){
	LCDClear();
	LCDWriteStringXY(0,0,"Press 1 Reset");
	LCDWriteStringXY(0,1,"Admin Password");
	_delay_ms(1000);
	LCDClear();
	LCDWriteStringXY(0,0,"Press 2 Reset");
	LCDWriteStringXY(0,1,"User01 Password");
	_delay_ms(1000);
	LCDClear();
	LCDWriteStringXY(0,0,"Press 3 Reset");
	LCDWriteStringXY(0,1,"User02 Password");
	_delay_ms(1000);
	if(newusercpw[3]!=0){
		LCDWriteStringXY(0,0,"Press 4 Reset");
		LCDWriteStringXY(0,1,"New user Password");
		_delay_ms(1000);
	}
	
	LCDClear();
	LCDWriteStringXY(0,0,"Enter");
	
	while(1){
		key();
		if(pw == '1'){
			LCDClear();
			LCDWriteStringXY(0,0,"Admin Password");
			_delay_ms(1000);
			admin_reset_password_check();
			break;
		}else if(pw == '2'){
			LCDClear();
			LCDWriteStringXY(0,0,"User1 Password");
			_delay_ms(1000);
			user1_reset_password_check();
			break;
		}else if (pw == '3'){
			LCDClear();
			LCDWriteStringXY(0,0,"User2 Password");
			_delay_ms(1000);
			user2_reset_password_check();
			break;
		}else if (pw == '4'){
			LCDClear();
			LCDWriteStringXY(0,0,"Newuser Password");
			_delay_ms(1000);
			newuser_reset_password_check();
			break;
		} 
	}
}

///// Check admin password /////
void admin_check_password(void){

	LCDClear();
	
	while(i<4){
		password();
	}
	while(1){
		if(epw[0] == admincpw[0] && epw[1] == admincpw[1] && epw[2] == admincpw[2] && epw[3] == admincpw[3] ){
			LCDClear();
			LCDWriteStringXY(0,0,"Door Opened ");
			PORTA|= (1<<0);
			PORTA|= (1<<6);
			_delay_ms(4000);
			PORTA|= (1<<0);
			PORTA|= (1<<6);
			break;
		}else {
			LCDClear();
			LCDWriteStringXY(0,0,"Wrong Password ");
			LCDWriteStringXY(0,1,"ReEnter Password ");
			_delay_ms(1000);
			LCDClear();
			LCDWriteStringXY(0,0,"ReEnter Password ");
			i=0;
			admin_check_password();	
		}
	}
}

///// Set reset password for admin /////
void admin_reset_password_check(void){

	reset_password_check1();
	
	if(epw[0] == rpw[0] && epw[1] == rpw[1] && epw[2] == rpw[2] && epw[3] == rpw[3] ){
		unsigned j=0;
		for(j=0;j<4;j++){
			admincpw[j] = rpw[j];
		}
		LCDClear();
		LCDWriteStringXY(0,0,"Password Reset");
		LCDWriteStringXY(0,1,"Done ");
		_delay_ms(1000);
	}
	else {
		LCDClear();
		LCDWriteStringXY(0,0,"Wrong New");
		LCDWriteStringXY(0,1,"ReEnter Password ");
		_delay_ms(1000);
		LCDClear();
		LCDWriteStringXY(0,0,"ReEnter Password ");
		i=0;
		admin_reset_password_check();
	}
}

///// Set reset password for user1 /////
void user1_reset_password_check(void){

	reset_password_check1();
	
	if(epw[0] == rpw[0] && epw[1] == rpw[1] && epw[2] == rpw[2] && epw[3] == rpw[3] ){
		unsigned j=0;
		for(j=0;j<4;j++){
			user1cpw[j] = rpw[j];
		}
		LCDClear();
		LCDWriteStringXY(0,0,"Password Reset");
		LCDWriteStringXY(0,1,"Done ");
		_delay_ms(1000);	
	}
	else {
		LCDClear();
		LCDWriteStringXY(0,0,"Wrong New");
		LCDWriteStringXY(0,1,"ReEnter Password ");
		_delay_ms(1000);
		LCDClear();
		LCDWriteStringXY(0,0,"ReEnter Password ");
		i=0;
		user1_reset_password_check();
	}
}

///// Set reset password for user2 /////
void user2_reset_password_check(void){

	reset_password_check1();
	
	if(epw[0] == rpw[0] && epw[1] == rpw[1] && epw[2] == rpw[2] && epw[3] == rpw[3] ){
		unsigned j=0;
		for(j=0;j<4;j++){
			user2cpw[j] = rpw[j];
		}
		LCDClear();
		LCDWriteStringXY(0,0,"Password Reset");
		LCDWriteStringXY(0,1,"Done ");;
		_delay_ms(1000);
	}
	else {
		LCDClear();
		LCDWriteStringXY(0,0,"Wrong New");
		LCDWriteStringXY(0,1,"ReEnter Password ");
		_delay_ms(1000);
		LCDClear();
		LCDWriteStringXY(0,0,"ReEnter Password ");
		i=0;
		 user2_reset_password_check();
	}
}

///// Set reset password for newuser /////
void newuser_reset_password_check(void){

	reset_password_check1();
	
	if(epw[0] == rpw[0] && epw[1] == rpw[1] && epw[2] == rpw[2] && epw[3] == rpw[3] ){
		unsigned j=0;
		for(j=0;j<4;j++){
			newusercpw[j] = rpw[j];
		}
		LCDClear();
		LCDWriteStringXY(0,0,"Password Reset");
		LCDWriteStringXY(0,1,"Done ");
		_delay_ms(1000);	
	}
	else {
		LCDClear();
		LCDWriteStringXY(0,0,"Wrong New");
		LCDWriteStringXY(0,1,"ReEnter Password ");
		_delay_ms(1000);
		LCDClear();
		LCDWriteStringXY(0,0,"ReEnter Password ");
		i=0;
		newuser_reset_password_check();
	}
}

///// Enroll new card/////
void enroll_new_card(void){
	LCDClear();
	LCDWriteStringXY(0,0,"Show Card ");
	_delay_ms(1000);
	while(1){
		card_check();
		if(card_num!=1 && card_num!=2 && card_num!=3 && card_num!=4){
			for(unsigned char j=0;j<10;j++){
				new_user[j] = data[j];
				PORTA |= (1<<7);
				_delay_ms(100);
				PORTA &= ~(1<<7);
			}
			
			LCDClear();
			LCDWriteStringXY(0,0,"Enter New");
			LCDWriteStringXY(0,1,"Password ");
			_delay_ms(1000);
			
			i=0;
			password();
			
			for(unsigned char j=0;j<4;j++){
				newusercpw[j] = epw[j];
			}
			
			LCDClear();
			LCDWriteStringXY(0,0,"Password Set");
			_delay_ms(1000);
			break;
		}
	}
}
