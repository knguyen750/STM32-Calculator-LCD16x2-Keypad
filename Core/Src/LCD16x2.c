/*
 * File: LCD16x2.c
 * Driver Name: [[ LCD16x2 Display (GPIO 4-Bit Mode) ]]
 * SW Layer:   ECUAL
 * Created on: Jun 28, 2020
 * Author:     Khaled Magdy
 * -------------------------------------------
 * For More Information, Tutorials, etc.
 * Visit Website: www.DeepBlueMbedded.com
 *
 */

#include "LCD16x2.h"
#include "DWT_Delay.h"
#include "main.h"
#include "gpio.h"


//-----[ Alphanumeric LCD 16x2 Routines ]-----

void LCD_DATA(unsigned char data)
{
    HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, ((data>>3)&0x01)); // LCD Pin D7
    HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, ((data>>2)&0x01)); // LCD Pin D6
    HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, ((data>>1)&0x01)); // LCD Pin D5
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, ((data>>0)&0x01)); // LCD Pin D4
}
void LCD_CMD(unsigned char a_CMD)
{
    // Select Command Register
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET); // LCD Register Select
    // Move The Command Data To LCD
    LCD_DATA(a_CMD);
    // Send The EN Clock Signal
    HAL_GPIO_WritePin(LCD_E_GPIO_Port,  LCD_E_Pin,  GPIO_PIN_SET); // Enable rising edge
    DWT_Delay_us(50); // Pulse width time (450 ns min.)
    HAL_GPIO_WritePin(LCD_E_GPIO_Port,  LCD_E_Pin,  GPIO_PIN_RESET); // Enable falling edge
    DWT_Delay_us(50); // Address hold time (20 ns min.) and data hold time (10 ns min.)
}
void LCD_Clear()
{
    LCD_CMD(0x0);
    LCD_CMD(0x1);
    DWT_Delay_us(10);
}
void LCD_Set_Cursor(unsigned char r, unsigned char c)
{
    unsigned char Temp,Low4,High4;
    if(r == 1)
    {
      Temp  = 0x80 + c - 1; //0x80 is used to move the cursor
      High4 = Temp >> 4;
      Low4  = Temp & 0x0F;
      LCD_CMD(High4);
      LCD_CMD(Low4);
    }
    if(r == 2)
    {
      Temp  = 0xC0 + c - 1;
      High4 = Temp >> 4;
      Low4  = Temp & 0x0F;
      LCD_CMD(High4);
      LCD_CMD(Low4);
    }
    DWT_Delay_ms(2);
}
void LCD_Init()
{
	// Initialize The DWT Delay Function Which Will Be Used Later On
    DWT_Delay_Init();
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_RESET); // LCD Pin D7
    HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_RESET); // LCD Pin D6
    HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, GPIO_PIN_RESET); // LCD Pin D5
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_RESET); // LCD Pin D4

	// The Init. Procedure As Described In The Datasheet
    LCD_DATA(0x00);
    DWT_Delay_ms(150);
    LCD_CMD(0x03);
    DWT_Delay_ms(5);
    LCD_CMD(0x03);
    DWT_Delay_us(150); 
    LCD_CMD(0x03); // Function set
    
    // Begin waiting time between instr. is longer than instr. exec time
    LCD_CMD(0x02); 
    
    // Function set 8-bit mode 
    LCD_CMD(0x02); 
    LCD_CMD(0x08); 

    // Not sure how below fits into the initialization procedure in datasheet
    // but it works

    // Display on/off control: specify two display lines, blinking cursor
    LCD_CMD(0x00); 
    LCD_CMD(0x0F); 
    
    // Entry mode set: increment, display shift
    LCD_CMD(0x00);
    LCD_CMD(0x06); 
    
    LCD_CMD(0x00); 
    LCD_CMD(0x01); // Clear Display
}
void LCD_Write_Char(char data)
{
   char low4,high4;
   low4  = data & 0x0F;
   high4 = data & 0xF0;

   HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin, GPIO_PIN_SET);

   LCD_DATA(high4>>4);
   HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
   DWT_Delay_us(50);
   HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
   DWT_Delay_us(50);

   LCD_DATA(low4);
   HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
   DWT_Delay_us(50);
   HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
   DWT_Delay_us(50);
}
void LCD_Write_String(char *str)
{
    int i;
    for(i=0;str[i]!='\0';i++)
       LCD_Write_Char(str[i]);
}
void LCD_SL()
{
    LCD_CMD(0x01);
    LCD_CMD(0x08);
    DWT_Delay_us(50);
}
void LCD_SR()
{
    LCD_CMD(0x01);
    LCD_CMD(0x0C);
    DWT_Delay_us(50);
}
void LCD_Home()
{
    LCD_CMD(0x00);
    LCD_CMD(0x02);
    DWT_Delay_ms(2);
}

void LCD_Set_Auto_Shift(int set_on)
{
    if (set_on == 1) {
        LCD_CMD(0x00);
        LCD_CMD(0x07);
        DWT_Delay_us(50);
    }
    else {
        LCD_CMD(0x00);
        LCD_CMD(0x06);
        DWT_Delay_us(50);
    } 
}