#define EX_LED (*(volatile unsigned char *)0x8008)
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <math.h>
//#include <time.h>
//#include "lcd.h"

#define CLI() cli()
#define SEI() sei()

#define EX_SS_DATA (*(volatile unsigned char*)0x8002)
#define Ex_SS_SEL (*(volatile unsigned char*)0x8003)

unsigned char segment_data[11] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
0x6d, 0x7d, 0x27, 0x7f, 0x6f, 0};

void delay_ms(int n)
{ 
    volatile int i,j;
        for(i=1;i<n;i++)
         for(j=1;j<600;j++);
}

void port_init(void)
{ 	
	PORTA = 0x00;
	DDRA = 0xff; //주소출력
	PORTB = 0x00;
	DDRB = 0x00;
	PORTC = 0x00; //m103 output only
	DDRC = 0x03; //주소 출력
	PORTD = 0x00;
	DDRD = 0x00;
	PORTE = 0x00;
	DDRE = 0x00;
	PORTF = 0x00;
	DDRF = 0x00;
	PORTG = 0x00;
	DDRG = 0x03; // Write, ale 신호
	
	PORTD = 0xff; // PORTD 초기값 설정
	DDRD = 0xff; // PORTD 모두 출력으로 설정
	PORTB = 0xff;
	DDRB = 0x00; // PORTB 입력으로 설정
	DDRG  = 0x1f;  // buzzle --> PORTG4
	
	Ex_SS_SEL = 0x80;
}

char input()
{
	//사용 시 입력 가능, 블럭 상태는 아님.
	char keydata = PINB & 0xFF;
	PORTD = (keydata << 3);
	switch(keydata)
	{	
      		case 0x01 : _delay_ms(300); return '0'; //1
				
			case 0x02 : _delay_ms(300); return '1'; //2
				
			case 0x04 : _delay_ms(300); return '2'; //4
				
			case 0x08 : _delay_ms(300); return '3'; //8
				
			case 0x10 : _delay_ms(300); return '4'; //16
				
			case 0x20 : _delay_ms(300); return '5'; //32
				
			case 0x40 : _delay_ms(300); return '6'; //64

			case 0x80 : _delay_ms(300); return '7'; //128

			default : _delay_ms(300); return NULL;
	}
		
	
}

void timer1_init(void)
{ // TCNT1 = 3036; //65536-3036=62500
TCCR1A = 0x00;
TCCR1B = 0x0c; // WGM12 = CS12 = 1, prescale=256
OCR1A = 62500;
TIMSK = 0x10; //OCIE1A = 1
// ASSR = 0x00;
}


void devices_init(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XDIV  = 0x00; //xtal divider
 XMCRA = 0x00; //external memory
 port_init();  
 MCUCR = 0x80;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x10; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

int putSegment(int level, int num)
{
	level = level % 4;
	switch(level)
	{
		case 0 : Ex_SS_SEL = 0xfe;
				 break;
		case 1 : Ex_SS_SEL = 0xfd;
				 break;
		case 2 : Ex_SS_SEL = 0xfb;
				 break;
		case 3 : Ex_SS_SEL = 0xf7;
				 break;
		default : return 0;
	
	}
	
		EX_SS_DATA = segment_data[num];
		delay_ms(5);

}


double sound(int freq)
{	//T1HIGHCNT = (0xFFFF-floor(1000000/freq))/0x100;
	//T1LOWCNT = 0xFF00 - (0xFFFF-floor(1000000/freq) );
	double TCNT = (0xFFFF-floor(1000000/(freq/2)));
	return TCNT;
}

void buzzer(double sound)
{
	TCNT1 = sound;
	PORTG = PORTG ^ 0x10;
}

//정확한 길이를 나도 모른다. 그냥 감으로 맞춰야함
void soundLong(int freq,int HowLong)
{
	for(int i = 0; i < HowLong; i++) 
	{
		buzzer(sound(freq));
	}

}

void wrongSound()
{
	soundLong(523, 700); //do
	_delay_ms(30);
	soundLong(523, 700); //do
}

void setLED(int num)
{
	if(num == 0) EX_LED = 0x00;
	else if(num == 1) EX_LED = 0x01;
	else if(num == 2) EX_LED = 0x03;
	else if(num == 3) EX_LED = 0x07;
	else if(num == 4) EX_LED = 0x0F;
}



