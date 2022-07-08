#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "My.h" // 함수가 많아 내가 따로 만든 헤더
#include "time.h" //시간 관련 함수를 새로 분류
#include <string.h>
#define CLI() cli()
#define SEI() sei()

const int statLoc = 11;
const int hourLoc = 1;
const int minLoc = 4;
const int secLoc = 7;

//해외 국가들과 서울의 시간 차
int others[8] = {0, 11, 16, 00, 17, 18, 1, 21};

void setStatus(char *s);
void putTime(int y, Time* t);
void putDate(int y, Time* t);

int login();
void title();
void dualTime();
void alarm();
void timer();
void stopwatch();

Time now;
Time now2;

int alEnabled = 0;
Time alTime;

int tEnabled = 0;
Time tTime;

int sEnabled = 0;
int sTime = 0;
int sRecord[4];
int sCnt;

int mode = 0;

//인터럽트
ISR(TIMER1_COMPA_vect)
{
	modSec(&now, 1);
	
	if(mode == 2)
	{
		modSec(&now2, 1);
	}
	
	if(alEnabled == 1)
	{
		if(alTime.hour == now.hour && alTime.min == now.min && alTime.sec == now.sec)			
			for(int cnt = 0; cnt < 10; cnt++)
			{
				soundLong(659, 2000); // mi
				_delay_ms(400);
			}
					
	}
	
	if(tEnabled == 1)
	{
		modSec(&tTime, -1);
		
		if(tTime.hour == 0 && tTime.min == 0 && tTime.sec == 0)
		{

			tEnabled = 0; 
			
			for(int cnt = 0; cnt < 10; cnt++)
			{
				soundLong(659, 2000); // mi
				_delay_ms(400);
			}
		}
		
	}
	
	if(sEnabled == 1)
	{
		sTime++;
	}

}


int main()
{
	//Init
	port_init();
	timer1_init();
	devices_init();
	lcdInit();
	_delay_ms(50);
	MCUCR=0x80;
	    
	now.year = 2022;
	now.month = 6;
	now.date = 1;
	
	now.hour = 0;
	now.min = 0;
	now.sec = 0;
	
	
	while(!login()) 
	{
		wrongSound();
		_delay_ms(50); // 로그인 실패 시 다시 로그인 해야함.
	}
	
	while(1)
	{
		soundLong(523, 1000); 
		title();			   // 메인 시간
		soundLong(523, 1000);
		dualTime();			   // 세계시간
		soundLong(523, 1000);
		alarm();			   // 알람
		soundLong(523, 1000);
		timer();			   // 타이머
		soundLong(523, 1000);
		stopwatch();		   // 스돕워치
	}
	
	lcd_puts(2, "End of Program");
	_delay_ms(50);

	return 0;
}


int login()
{

	//우선 EEPROM의 데이터를 읽어들인다.
	//0 : 초기화 유뮤, 1~4 : 로그인 문자열
	
    char loginable;
    char con[5];
	
	loginable = eeprom_read_byte(0);
	con[0] = eeprom_read_byte(1);
	con[1] = eeprom_read_byte(2);
	con[2] = eeprom_read_byte(3);
	con[3] = eeprom_read_byte(4);
	con[4] = '\0';
	
	

	//만약, 0번째 바이트가 1이라면, 그대로 로그인 가능, 만약 0번째 바이트가 0이라면, 사용할 비번 입력
	if(loginable == '1')
	{
		lcd_puts(1, "Login Please!");
		lcd_puts(2, "____");		
	}
	else
	{
		lcd_puts(1, "Put your PW!");
		lcd_puts(2, "____");
	}
	
	
	
	//PW입력 단계
	char str[5]; str[4] = '\0';
	for(int i = 0; i < 4; i++)
	{
		char temp = NULL;
		while((temp = input()) == NULL);
		
		str[i] = temp;
		
		//비밀번호 입력에 따른 문자열 변경
		char star[5]; star[4] = '\0';
		for(int j = 0; j < 4; j++)
			star[j] = (i < j) ? '_' : '*';
			
		
		lcd_puts(2, star);
		
	}
	
	_delay_ms(50);
	


	//로그인 가능한 상태라면, 입력 문자열과 EEPROM 문자열을 대조하여 맞다면 1반환, 아니라면 0 반환.
	//비밀번호 초기화가 필요한 상황이었다면, 입력 받은 문자열을 EEPROM에 저장하고 1반환.
	if(loginable == '1')
	{
		if(!strcmp(con, str) )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{	
		lcdClear();
		_delay_ms(50);
		
		eeprom_write_byte(0x0000, '1');
		eeprom_write_byte(0x0001, str[0]);
		eeprom_write_byte(0x0002, str[1]);
		eeprom_write_byte(0x0003, str[2]);
		eeprom_write_byte(0x0004, str[3]);
		
		_delay_ms(50);
		
		return 1;
		
	}
}

void title()
{
	
	mode = 1;
	lcdClear();
	_delay_ms(50);
	
	
	while(1)
	{
		char key = input();
		
		if(key == '0') return;
		else if(key == '7')
		{
			setStatus("Year  ");
		
			Time temp;
			
			timecpy(&temp, &now);
			
			int selected = 0;
			while(1)
			{
				char data = input();
				
				if(data == '6')
				{
					selected ++;
					if(5 < selected) selected = 0;

					switch(selected)
					{
						case 0 : setStatus("Year  "); break;
						
						case 1 : setStatus("Month "); break;
						
						case 2 : setStatus("Date  "); break;
						
						case 3 : setStatus("Hour  "); break;
						
						case 4 : setStatus("Minute"); break;
						
						case 5 : setStatus("Second"); break;
					
					}
				}
				
				if(data == '5')
				{
					switch(selected)
					{
						case 0 : modYear(&temp, 1); break;
						
						case 1 : modMonth(&temp, 1); break;
						
						case 2 : modDate(&temp, 1); break;
						
						case 3 : modHour(&temp, 1); break;
						
						case 4 : modMin(&temp, 1); break;
						
						case 5 : modSec(&temp, 1); break;
					
					}
				
				}
				
				if(data == '4')
				{
					switch(selected)
					{
						case 0 : modYear(&temp, -1); break;
						
						case 1 : modMonth(&temp, -1); break;
						
						case 2 : modDate(&temp, -1); break;
						
						case 3 : modHour(&temp, -1); break;
						
						case 4 : modMin(&temp, -1); break;
						
						case 5 : modSec(&temp, -1); break;
					
					}
				
				}
				
				putDate(1, &temp);
				putTime(2, &temp);
				if(data == '7')
				{
					timecpy(&now, &temp);
					break;
				}
			
			}
			
		}

		setStatus("Seoul");
		putDate(1, &now);
		putTime(2, &now);

	}

	return;
}


void dualTime()
{
	mode = 2;
	lcdClear();
	_delay_ms(50);
	
	lcd_gotoxy(statLoc, 1);
	putString("Seoul ");
	
	int selected = 0;
	while(1)
	{
		if(selected == 0)
		{
			lcd_puts(2, "             ");
		}
		else
		{
			putTime(2, &now2);
		}
		putTime(1, &now);
		char data = input();
		
		switch(data)
		{
			case '7' : selected = 1; setStatus("NY");break;
			
			case '6' : selected = 2; setStatus("LONDON");break;
			
			case '5' : selected = 3; setStatus("TOKYO");break;
			
			case '4' : selected = 4; setStatus("GERMAN");break;
			
			case '3' : selected = 5; setStatus("MOSCOW");break;
			
			case '2' : selected = 6; setStatus("SIDNEY");break;
			
			case '1' : selected = 7; setStatus("CHI");break;
			
			case '0' : return;
			
			//default : continue;
		
		}
		
		timecpy(&now2, &now);
		modHour(&now2, others[selected]);
		
	}
	
	
}

void alarm()
{
	mode = 3;
	
	lcdClear();
	_delay_ms(50);

	while(1)
	{
		char key = input();
		
		if(key == '0') return;
		else if(key == '7')
		{
			setStatus("Hour  ");
		
			Time temp;
			
			timecpy(&temp, &alTime);
			
			int selected = 0;
			while(1)
			{
				char data = input();
				
				if(data == '6')
				{
					selected ++;
					if(3 < selected) selected = 0;

					switch(selected)
					{
						
						case 0 : setStatus("Hour  "); break;
						
						case 1 : setStatus("Minute"); break;
						
						case 2 : setStatus("Second"); break;
						
						case 3 : setStatus("ON/OFF"); break;
					
					}
				}
				
				if(data == '5')
				{
					switch(selected)
					{
						case 0 : modHour(&temp, 1); break;
						
						case 1 : modMin(&temp, 1); break;
						
						case 2 : modSec(&temp, 1); break;
					
						case 3 : alEnabled = (alEnabled == 0) ? 1 : 0; break;
					
					}
				
				}
				
				if(data == '4')
				{
					switch(selected)
					{
						case 0 : modHour(&temp, -1); break;
						
						case 1 : modMin(&temp, -1); break;
						
						case 2 : modSec(&temp, -1); break;
						
						case 3 : alEnabled = (alEnabled == 0) ? 1 : 0; break;
					
					}
				
				}
				
				putTime(1, &temp);
				if(alEnabled == 0)lcd_puts(2, "OFF");
				else lcd_puts(2, "ON ");
				if(data == '7')
				{
					timecpy(&alTime, &temp);
					break;
				}
			
			}
			
		}

		putTime(1, &alTime);
		if(alEnabled == 0)lcd_puts(2, "OFF");
		else lcd_puts(2, "ON ");
		setStatus("Alarm");

	}
}

void timer()
{
	mode = 4;
	
	lcdClear();
	_delay_ms(50);

	int selected = 0;
	while(1)
	{
		lcd_gotoxy(1, 2);
		if(tEnabled == 0) putString("Stopped");
		else putString("Running");
			
		char key = input();
		
		if(key == '0') return;
		
		if(key == '7')
		{
			if(tEnabled == 0 && !(tTime.hour == 0 && tTime.min == 0 && tTime.sec == 0)) tEnabled = 1;
			else tEnabled = 0;
			
			lcd_gotoxy(1, 2);
			if(tEnabled == 0) putString("Stopped");
			else 
			{
				soundLong(523, 1500); //do
				putString("Running");
				lcd_gotoxy(statLoc, 1);
				putString("       ");
			}
			
		}
		else if(tEnabled == 0)
		{
			
			if(key == '6')
			{
				
				selected++;
				if(3 < selected) selected = 0;
				
				lcd_gotoxy(statLoc, 1);
				switch(selected)
				{
					case 0 : putString("      "); break;
					case 1 : putString("Hour  "); break;
					case 2 : putString("Min   "); break;
					case 3 : putString("Sec   "); break;
					
				}
				
			}
			else if(key == '5')
			{
				switch(selected)
				{
					case 1 : modHour(&tTime, 1); break;
					case 2 : modMin(&tTime, 1); break;
					case 3 : modSec(&tTime, 1); break;
					
				}
			}
			else if(key == '4')
			{
				switch(selected)
				{
					case 1 : modHour(&tTime, -1); break;
					case 2 : modMin(&tTime, -1); break;
					case 3 : modSec(&tTime, -1); break;
					
				}
			}
			
		}
		
		putTime(1, &tTime);
		setStatus("Timer");

	}
}


void stopwatch()
{
	mode = 5;
	lcdClear();
	_delay_ms(50);

	int selected = 0;
	while(1)
	{
			
		char key = input();
		
		if(key == '0') return;
		
		if(key == '7')
		{
			soundLong(523, 1500); //do
			sEnabled = (sEnabled == 1) ? 0 : 1;
			
			if(selected == 1)
			{
				lcdClear();
				_delay_ms(50);
				selected = 0;
			}
			
			lcd_gotoxy(1, 2);
			if(sEnabled == 0) 
			{
				putString("Stopped");
			
			}
			else 
			{
				putString("Running");
				
			}
			
		}
		else if(key == '6')
		{
			if(selected == 1)
			{
				lcdClear();
				_delay_ms(50);
				selected = 0;
			}
			
			//기록 남기기
			if(sEnabled == 1)
			{
				if(sCnt < 4)
				{
					soundLong(523, 500); //do
					sRecord[sCnt] = sTime;
					sCnt++;
				}
				else
				{
					wrongSound();
				}

			}
			//리셋하기
			else
			{
				soundLong(523, 1500); //do
				memset(sRecord, 0, sizeof(sRecord));
				sCnt=0;
				sTime = 0;
				
			}
			
		}
		else if(key == '5')
		{
			lcdClear();
			_delay_ms(50);
			
			selected = (selected == 0) ? 1 : 0;

		}

		if(selected == 0)
		{
			lcd_gotoxy(statLoc, 1);
			putString("      ");
			lcd_gotoxy(statLoc, 1);
			for(int i = 0; i < sCnt; i++) lcd_putn1(i+1);
			
			lcd_gotoxy(1, 1);
			lcd_putn2(sTime / 60);
			lcd_putch('.');
			lcd_putn2(sTime % 60);
			
			lcd_gotoxy(1, 2);
			if(sEnabled == 0) putString("Stopped");
			else putString("Running");
			
			setStatus("sWatch");
		}
		else
		{
			lcd_gotoxy(1, 1);
			putString("1:");
			if(sRecord[0] != 0) 
			{
				lcd_putn2(sRecord[0] / 60);
				lcd_putch('.');
				lcd_putn2(sRecord[0] % 60);				
			}
			
			lcd_gotoxy(9, 1);
			putString("2:");
			if(sRecord[1] != 0) 
			{
				lcd_putn2(sRecord[1] / 60);
				lcd_putch('.');
				lcd_putn2(sRecord[1] % 60);				
			}
			
			lcd_gotoxy(1, 2);
			putString("3:");
			if(sRecord[2] != 0) 
			{
				lcd_putn2(sRecord[2] / 60);
				lcd_putch('.');
				lcd_putn2(sRecord[2] % 60);				
			}
			
			lcd_gotoxy(9, 2);
			putString("4:");
			if(sRecord[3] != 0) 
			{
				lcd_putn2(sRecord[3] / 60);
				lcd_putch('.');
				lcd_putn2(sRecord[3] % 60);				
			}
			
		}
		
		setLED(sCnt);

	}
	
	
}

// 우측 하단 상태 바꿔줌
void setStatus(char *s)
{
	lcd_gotoxy(statLoc, 2);
	putString("      ");
	lcd_gotoxy(statLoc, 2);
	putString(s);
}


//해당 줄에 시간 뿌려주기 (시 : 분 : 초)
void putTime(int y, Time* t)
{
		
	lcd_gotoxy(hourLoc, y);
	lcd_putn2(t->hour);
	lcd_putch(':');
		
	lcd_gotoxy(minLoc, y);
	lcd_putn2(t->min);
	lcd_putch(':');
		
	lcd_gotoxy(secLoc, y);
	lcd_putn2(t->sec);
}

void putDate(int y, Time* t)
{
	lcd_gotoxy(1, y);
	lcd_putn4(t->year);
	lcd_putch('.');

	lcd_putn2(t->month);
	lcd_putch('.');
		
	lcd_putn2(t->date);

}
