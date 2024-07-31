/*
 * File: LCD16x2.h
 * Driver Name: [[ LCD16x2 Display (GPIO 4-Bit Mode) ]]
 * SW Layer:   ECUAL
 * Created on: Jun 28, 2020
 * Ver: 1.1
 * Author:     Khaled Magdy
 * -------------------------------------------
 * For More Information, Tutorials, etc.
 * Visit Website: www.DeepBlueMbedded.com
 *
 */

#ifndef LCD16X2_H_
#define LCD16X2_H_

#include "stm32wbxx_hal.h"

//-----[ Prototypes For All Functions ]-----

void LCD_Init();                  // Initialize The LCD For 4-Bit Interface
void LCD_Clear();                 // Clear The LCD Display
void LCD_SL();                    // Shift The Entire Display To The Left
void LCD_SR();                    // Shift The Entire Display To The Right
void LCD_CMD(unsigned char);      // Send Command To LCD
void LCD_DATA(unsigned char);     // Send 4-Bit Data To LCD
void LCD_Set_Cursor(unsigned char, unsigned char);  // Set Cursor Position
void LCD_Write_Char(char);        // Write Character To LCD At Current Position
void LCD_Write_String(char*);     // Write A String To LCD
void LCD_Home();                  // Returns display to original position
void LCD_Set_Auto_Shift(int);        // Configures display to automatically shift with cursor.
#endif /* LCD16X2_H_ */
