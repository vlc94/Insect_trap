
#pragma once

#include "pins.h"
#include <LiquidCrystal.h>


//--------------------------------------------
// Lcd pins
//--------------------------------------------
LiquidCrystal lcd(lcd_rs, lcd_e, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

//--------------------------------------------
// Define no. of lcd rows & columns
//--------------------------------------------
void setupLcd()
{
    lcd.begin(16, 2);
}

void lcdDisplayDigit(uint8_t col, uint8_t row, int val)
{
    
    lcd.setCursor(col,row);
    if(val < 10)
    {
        lcd.print(0);
        lcd.print(val);
    }
    else
    {
        lcd.print(val);
    }
}

/*
    col -> lcd column
    row -> lcd row
    phl -> Msg to be displayed
    val -> value to displayed
*/
void lcdDisplayValue(uint8_t col, uint8_t row, String phl ,int8_t val)
{
        lcd.clear();
        lcd.setCursor(col,row);
        lcd.print(phl);
        lcd.setCursor(0,row+1);
        lcd.print(val);
}