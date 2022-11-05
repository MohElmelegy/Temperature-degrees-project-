/*
 * DH22.c
 *
 * Created: 12/30/2020 2:49:53 AM
 *  Author: MOhElmelegy
 */ 

#define F_CPU  8000000
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "DIO_Interface.h"
#include "LCD_Interface.h"
#include "UART_Interface.h"
#include "KEYPAD_Interface.h"
#include <avr/io.h>
#include <avr/delay.h>
#define clear 0b00000001
#define DHT11_PIN 8
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;

void Request()				/* Microcontroller send start pulse/request */
{
	DDRA |= (1<<DHT11_PIN);
	PORTA &= ~(1<<DHT11_PIN);	/* set to low pin */
	_delay_ms(20);			/* wait for 20ms */
	PORTA |= (1<<DHT11_PIN);	/* set to high pin */
}


void Response()				/* receive response from DHT11 */
{
	
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
	
}

uint8_t Receive_data()			/* receive data */
{
	for (int q=0; q<8; q++)
	{
		while((PIND & (1<<DHT11_PIN)) == 0);  /* check received bit 0 or 1 */
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))/* if high pulse is greater than 30ms */
		c = (c<<1)|(0x01);	/* then its logic HIGH */
		else			/* otherwise its logic LOW */
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}




int main(void)

{
	char data[5];
	LCD_VidInit();
	UART_Init();
	LCD_VidSendCommand(clear);
	_delay_ms(2);
	LCD_VidSetPosition(0,0);
	LCD_VidWriteString("Humidity =");
	LCD_VidSetPosition(1,0);
	LCD_VidWriteString("Temp =");
	_delay_ms(3000);
    while(1)
    {
		Request();		/* send start pulse */
		
		Response();		/* receive response */
		
		
		I_RH=Receive_data();	/* store first eight bit in I_RH */
		D_RH=Receive_data();	/* store next eight bit in D_RH */
		I_Temp=Receive_data();	/* store next eight bit in I_Temp */
		D_Temp=Receive_data();	/* store next eight bit in D_Temp */
		CheckSum=Receive_data();/* store next eight bit in CheckSum */
		
		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			LCD_VidSendCommand(clear);
			_delay_ms(2);
			LCD_VidSetPosition(0,0);
			LCD_VidWriteString("Error");
			_delay_ms(3000);
		}
         
		 else
		 {
			 itoa(I_RH,data,10);
			 LCD_VidSetPosition(11,0);
			 UART_SendData(data);
			 LCD_VidWriteNumber(data);
			 LCD_VidSendCommand(".");
			 _delay_ms(3000);
			 
			 itoa(D_RH,data,10);
			 LCD_VidSendCommand(data);
			 UART_SendData(data);
			 LCD_VidSendCommand("%");
			 
			 itoa(I_Temp,data,10);
			 LCD_VidSetPosition(6,1);
			 LCD_VidSendCommand(data);
			 UART_SendData(data);
			 LCD_VidSendCommand(".");
			 
			 itoa(D_Temp,data,10);
			 LCD_VidSendCommand(data);
			 UART_SendData(data);
			 LCD_VidSendCommand("C ");
			 
			 itoa(CheckSum,data,10);
			 LCD_VidSendCommand(data);
			 UART_SendData(data);
			 LCD_VidSendCommand(" ");



			 
			 
			 
		 }			 
		 _delay_ms(10);
    }
}