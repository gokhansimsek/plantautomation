#include <stdio.h>
#include <inttypes.h>
#include <wiringPi.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//Define GPIO to LCD mapping
#define LCD_RS    4
#define LCD_E     8
#define LCD_D4   31
#define LCD_D5   11
#define LCD_D6   10
#define LCD_D7    6

//Define some device constants
#define LCD_WIDTH    20
#define LCD_CHR      HIGH
#define LCD_CMD      LOW
 
#define LCD_LINE_1   (0x80)   //LCD RAM address for the 1st line
#define LCD_LINE_2   (0xC0)   //LCD RAM address for the 2nd line
#define LCD_LINE_3   (0x94)   //LCD RAM address for the 3rd line
#define LCD_LINE_4   (0xD4)   //LCD RAM address for the 4th line
 
//Timing constants
#define E_PULSE   0.0005
#define E_DELAY   0.0005

void lcd_toggle_enable()
{
  //Toggle enable
  delay(E_DELAY);
  digitalWrite(LCD_E, HIGH);
  delay(E_PULSE);
  digitalWrite(LCD_E, LOW);
  delay(E_DELAY);
}

void lcd_byte(int bits, int mode)
{
  //Send byte to data pins
  //bits = data
  //mode = True  for character
  //       False for command
 
  digitalWrite(LCD_RS, mode); //Register select
 
  //High bits
  digitalWrite(LCD_D4, LOW);
  digitalWrite(LCD_D5, LOW);
  digitalWrite(LCD_D6, LOW);
  digitalWrite(LCD_D7, LOW);
  
  if (bits&0x10 == 0x10)
  {
    digitalWrite(LCD_D4, HIGH);
  }
  if (bits&0x20 == 0x20)
  {
    digitalWrite(LCD_D5, HIGH);
  }  
  if (bits&0x40 == 0x40)
  {
    digitalWrite(LCD_D6, HIGH);
  }
  if (bits&0x80 == 0x80)
  {
    digitalWrite(LCD_D7, HIGH);
  }
  
  //Toggle 'Enable' pin
  lcd_toggle_enable();
 
  //Low bits
  digitalWrite(LCD_D4, LOW);
  digitalWrite(LCD_D5, LOW);
  digitalWrite(LCD_D6, LOW);
  digitalWrite(LCD_D7, LOW);

  if (bits&0x01 == 0x01)
  {
    digitalWrite(LCD_D4, HIGH);
  }
  if (bits&0x02 == 0x02)
  {
    digitalWrite(LCD_D5, HIGH);
  }
  if (bits&0x04==0x04)
  {
    digitalWrite(LCD_D6, HIGH);
  }
  if (bits&0x08 == 0x08)
  {
    digitalWrite(LCD_D7, HIGH);
  }
  
  //Toggle 'Enable' pin
  lcd_toggle_enable();
}

void lcd_init()
{
  //Initialise display
  lcd_byte(0x33,LCD_CMD); //110011 Initialise
  lcd_byte(0x32,LCD_CMD); //110010 Initialise
  lcd_byte(0x06,LCD_CMD); //000110 Cursor move direction
  lcd_byte(0x0C,LCD_CMD); //001100 Display On,Cursor Off, Blink Off
  lcd_byte(0x28,LCD_CMD); //101000 Data length, number of lines, font size
  lcd_byte(0x01,LCD_CMD); //000001 Clear display
  delay(E_DELAY);
}

void lcd_string(char *message,int line)
{ 
	int index;
	
	lcd_byte(line, LCD_CMD);

	for (index=0; index < LCD_WIDTH;index++)
	{	
		lcd_byte((int)message[index],LCD_CHR);
	}
}

void main()
{
	pinMode(LCD_E, OUTPUT );     //Enable
	pinMode(LCD_RS, OUTPUT );    //Register Select
	pinMode(LCD_D4, OUTPUT );    //Data PIN 4
	pinMode(LCD_D5, OUTPUT );    //Data PIN 5
	pinMode(LCD_D6, OUTPUT );    //Data PIN 6
	pinMode(LCD_D7, OUTPUT );    //Data PIN 7

	//Initialise display
	lcd_init();
	
	while(1)
	{
		lcd_string("--------------------",LCD_LINE_1);
		lcd_string("Gokhan Simsek",LCD_LINE_2);
		lcd_string("Vestel",LCD_LINE_3);
		lcd_string("--------------------",LCD_LINE_4);
    }
    return(0);
}
