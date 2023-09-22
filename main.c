/*
 * GccApplication6.c
 *
 * Created: 9/7/2023 10:51:54 AM
 * Author : hp
 */ 
#include "STD_TYPES.h"
#include"BIT_MATH.h"
#include"DIO_interface.h"
#include"TIMER_test_interface.h"
#include "KPD_interface.h"
#include "Seven_Segments.h"
#include"CLCD_interface.h"
#include"GIE.h"
#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile u8 SecCount=0;
volatile u8 MinutesCount=0;
volatile u8 HoursCount=0;
volatile u8 alarm_MinutesCount=100,alarm_HoursCount=100,alarm_SecCount=100;
u8 value,first_digit,second_digit;
int num(u32 x);
void Start_Buzzer();
void Stop_Buzzer();
void Alarm_Mode();
void Real_Clock();
void Display_7SEG();

ISR(TIMER0_OVF_vect)
{

	static u16 c=0;
	c++;
	if(c==3907){
		TCNT0=192;
		SecCount++;
		c=0;
	}
}

int main()
{
	
	//LCD
	DIO_u8SetPortDir(DIO_PORTA, DIO_PORT_OUT);
	DIO_u8SetPortDir(DIO_PORTC, 0xff);
	CLCD_voidInit();

	//kpd
	DIO_u8SetPortDir(DIO_PORTD, 0b00001111);
	DIO_u8SetPortValue(DIO_PORTD, 0xff);

	//7SEG
	DIO_u8SetPinDir(DIO_PORTB,DIO_PIN0,DIO_PIN_OUT);
	DIO_u8SetPinDir(DIO_PORTB,DIO_PIN1,DIO_PIN_OUT);
	DIO_u8SetPinDir(DIO_PORTB,DIO_PIN2,DIO_PIN_OUT);
	DIO_u8SetPinDir(DIO_PORTB,DIO_PIN3,DIO_PIN_OUT);
	DIO_u8SetPinDir(DIO_PORTB,DIO_PIN4,DIO_PIN_OUT);
	DIO_u8SetPinDir(DIO_PORTB,DIO_PIN5,DIO_PIN_OUT);
	DIO_u8SetPinDir(DIO_PORTB,DIO_PIN6,DIO_PIN_OUT);

	CLCD_voidSendString(" Press 1 to adjust clock ");
	CLCD_voidGoToXY(1,0);
	CLCD_voidSendString("Press 2 to adjust alarm");
	TIMER0_voidOfInit();
	//232+3OVF
	TIMER0_voidPreLoadValue(192);
	//250+4OVF
	//TIMER0_voidPreLoadValue(6);
	GIE_voidEnable();
	while (1)
	{
		value=KPD_u8GetPressedKey();
		if (value!=KPD_NO_PRESSED_KEY)
		{
			if(value==9)        //Clock
			{

				Real_Clock();
				
			}
			else if(value==10)  //alarm
			{

				Alarm_Mode();

			}
			else if(value==14)
			{
				Stop_Buzzer();
			}
			else
			{
				CLCD_voidSendCommand(0x01);
				CLCD_voidSendString(" wrong choice");
				_delay_ms(1000);
				CLCD_voidSendCommand(0x01);

				if((alarm_SecCount!=100||alarm_MinutesCount!=100||alarm_HoursCount!=100)&&(SecCount!=0||MinutesCount!=0||HoursCount!=0))
				{
					CLCD_voidSendString(" Press 1 to adjust clock ");
					CLCD_voidGoToXY(1,0);
					CLCD_voidSendString("Press 2 to adjust alarm  ");
					CLCD_voidWriteNumber(alarm_HoursCount);
					CLCD_voidSendString(":");
					CLCD_voidWriteNumber(alarm_MinutesCount);
					CLCD_voidSendString(":");
					CLCD_voidWriteNumber(alarm_SecCount);

				}
				else
				{
					CLCD_voidSendString(" Press 1 to adjust clock ");
					CLCD_voidGoToXY(1,0);
					CLCD_voidSendString("Press 2 to adjust alarm");

				}

			}

		}
		//-----------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------

		Display_7SEG();

		if (SecCount==60)
		{
			SecCount=0;
			MinutesCount++;
		}
		if (MinutesCount==60)
		{
			MinutesCount=0;
			HoursCount++;
		}
		if (HoursCount==24)
		{
			HoursCount=0;
		}

		Start_Buzzer();


	}
	

}

int num(u32 x)
{
	u32 Number;
	if (x == 1)
	{
		Number = 7;
	}
	else if (x == 2)
	{
		Number = 8;
	}
	else if (x == 3)
	{
		Number = 9;
	}
	else if (x == 5)
	{
		Number = 4;
	}
	else if (x == 6)
	{
		Number = 5;
	}
	else if (x == 7)
	{
		Number = 6;
	}
	else if (x == 9)
	{
		Number = 1;
	}
	else if (x == 10)
	{
		Number=2;
	}
	else if (x == 11)
	{
		Number=3;
	}
	else if (x == 14)
	{
		Number = 0;
	}
	return Number;
}

void Start_Buzzer()
{
	if( HoursCount==alarm_HoursCount && MinutesCount==alarm_MinutesCount && SecCount==alarm_SecCount )
	{
		CLCD_voidSendCommand(0x01);
		CLCD_voidSendString(" Press 1 to adjust clock ");
		CLCD_voidGoToXY(1,0);
		CLCD_voidSendString("Press 0 to Close alarm");
		DIO_u8SetPinDir(DIO_PORTB,DIO_PIN7,DIO_PIN_OUT);
		SET_BIT(PORTB,7);
	}
}

void Stop_Buzzer()
{
	//Stop Buzzer
	CLCD_voidSendCommand(0x01);
	CLCD_voidSendString(" Press 1 to adjust clock ");
	CLCD_voidGoToXY(1,0);
	CLCD_voidSendString("Press 2 to adjust alarm");
	DIO_u8SetPinDir(DIO_PORTB,DIO_PIN7,DIO_PIN_IN);
	CLEAR_BIT(PORTB,7);
}


void Alarm_Mode()
{
	CLCD_voidSendCommand(0x01);
	CLCD_voidSendString(" ");
	CLCD_voidGoToXY(1,0);
	CLCD_voidSendString(" hours=--");
	CLCD_voidGoToXY(1,7);
	_delay_ms(500);
	do
	{
		first_digit=KPD_u8GetPressedKey();

	} while (first_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(first_digit));
	first_digit=num(first_digit);
	_delay_ms(400);
	do
	{
		second_digit=KPD_u8GetPressedKey();

	} while (second_digit==KPD_NO_PRESSED_KEY);

	CLCD_voidWriteNumber(num(second_digit));
	second_digit=num(second_digit);
	_delay_ms(300);
	alarm_HoursCount=(second_digit)+10*(first_digit);

	CLCD_voidSendCommand(0x01);
	CLCD_voidSendString(" ");//
	CLCD_voidGoToXY(1,0);
	CLCD_voidSendString(" minutes=--");
	CLCD_voidGoToXY(1,9);
	do
	{
		first_digit=KPD_u8GetPressedKey();

	} while (first_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(first_digit));
	first_digit=num(first_digit);
	_delay_ms(400);
	do
	{
		second_digit=KPD_u8GetPressedKey();

	} while (second_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(second_digit));
	second_digit=num(second_digit);
	_delay_ms(300);
	alarm_MinutesCount=(second_digit)+10*(first_digit);
	if (alarm_MinutesCount>60)
	{
		alarm_MinutesCount=alarm_MinutesCount-60;
		alarm_HoursCount++;
		if(alarm_HoursCount>=24)
		alarm_HoursCount=0;
	}
	CLCD_voidSendCommand(0x01);
	CLCD_voidSendString(" ");//
	CLCD_voidGoToXY(1,0);
	CLCD_voidSendString(" seconds=--");
	CLCD_voidGoToXY(1,9);

	do
	{
		first_digit=KPD_u8GetPressedKey();

	} while (first_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(first_digit));
	first_digit=num(first_digit);
	_delay_ms(400);
	do
	{
		second_digit=KPD_u8GetPressedKey();

	} while (second_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(second_digit));
	second_digit=num(second_digit);
	_delay_ms(300);
	alarm_SecCount=(second_digit)+10*(first_digit);
	if (alarm_SecCount>60)
	{
		alarm_SecCount=alarm_SecCount-60;
		alarm_MinutesCount++;
	}
	CLCD_voidSendCommand(0x01);
	CLCD_voidSendString(" Press 1 to adjust clock ");
	CLCD_voidGoToXY(1,0);
	CLCD_voidSendString("Press 2 to adjust alarm  ");
	CLCD_voidWriteNumber(alarm_HoursCount);
	CLCD_voidSendString(":");
	CLCD_voidWriteNumber(alarm_MinutesCount);
	CLCD_voidSendString(":");
	CLCD_voidWriteNumber(alarm_SecCount);
}


void Real_Clock()
{
	CLCD_voidSendCommand(0x01);
	CLCD_voidSendString(" hours=--");
	CLCD_voidGoToXY(0,7);
	_delay_ms(500);
	do
	{
		first_digit=KPD_u8GetPressedKey();

	} while (first_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(first_digit));
	first_digit=num(first_digit);
	_delay_ms(400);
	do
	{
		second_digit=KPD_u8GetPressedKey();

	} while (second_digit==KPD_NO_PRESSED_KEY);

	CLCD_voidWriteNumber(num(second_digit));
	second_digit=num(second_digit);
	_delay_ms(300);
	HoursCount=(second_digit)+10*(first_digit);

	CLCD_voidSendCommand(0x01);

	CLCD_voidSendString(" minutes=--");
	CLCD_voidGoToXY(0,9);
	do
	{
		first_digit=KPD_u8GetPressedKey();

	} while (first_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(first_digit));
	first_digit=num(first_digit);
	_delay_ms(400);
	do
	{
		second_digit=KPD_u8GetPressedKey();

	} while (second_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(second_digit));
	second_digit=num(second_digit);
	_delay_ms(300);
	MinutesCount=(second_digit)+10*(first_digit);
	if (MinutesCount>60)
	{
		MinutesCount=MinutesCount-60;
		HoursCount++;
		if(HoursCount>=24)
		HoursCount=0;
	}
	CLCD_voidSendCommand(0x01);

	CLCD_voidSendString(" seconds=--");
	CLCD_voidGoToXY(0,9);

	do
	{
		first_digit=KPD_u8GetPressedKey();

	} while (first_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(first_digit));
	first_digit=num(first_digit);
	_delay_ms(400);
	do
	{
		second_digit=KPD_u8GetPressedKey();

	} while (second_digit==KPD_NO_PRESSED_KEY);
	CLCD_voidWriteNumber(num(second_digit));
	second_digit=num(second_digit);
	_delay_ms(300);
	SecCount=(second_digit)+10*(first_digit);
	if (SecCount>60)
	{
		SecCount=SecCount-60;
		MinutesCount++;
	}
	if((alarm_SecCount!=100||alarm_MinutesCount!=100||alarm_HoursCount!=100)&&(SecCount!=0||MinutesCount!=0||HoursCount!=0))
	{
		CLCD_voidSendCommand(0x01);
		CLCD_voidSendString(" Press 1 to adjust clock ");
		CLCD_voidGoToXY(1,0);
		CLCD_voidSendString("Press 2 to adjust alarm  ");
		CLCD_voidWriteNumber(alarm_HoursCount);
		CLCD_voidSendString(":");
		CLCD_voidWriteNumber(alarm_MinutesCount);
		CLCD_voidSendString(":");
		CLCD_voidWriteNumber(alarm_SecCount);

	}
	else
	{
		CLCD_voidSendCommand(0x01);
		CLCD_voidSendString(" Press 1 to adjust clock ");
		CLCD_voidGoToXY(1,0);
		CLCD_voidSendString("Press 2 to adjust alarm");

	}
}

void Display_7SEG()
{
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN3,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN4,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN5,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN6,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN7,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN2,DIO_LOW);
	WriteSegment(SecCount%10);
	_delay_ms(5);

	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN2,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN4,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN5,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN6,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN7,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN3,DIO_LOW);
	WriteSegment(SecCount/10);
	_delay_ms(5);

	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN2,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN3,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN5,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN6,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN7,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN4,DIO_LOW);
	WriteSegment(MinutesCount%10);
	_delay_ms(5);

	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN2,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN3,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN4,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN6,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN7,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN5,DIO_LOW);
	WriteSegment(MinutesCount/10);
	_delay_ms(5);

	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN2,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN3,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN4,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN5,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN7,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN6,DIO_LOW);
	WriteSegment(HoursCount%10);
	_delay_ms(5);

	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN2,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN3,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN4,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN5,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN6,DIO_HIGH);
	DIO_u8SetPinValue(DIO_PORTC,DIO_PIN7,DIO_LOW);
	WriteSegment(HoursCount/10);
	_delay_ms(5);
}


