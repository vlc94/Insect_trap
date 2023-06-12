#pragma once

#include <LiquidCrystal.h>
//--------------------------------------------
// Assign GPIO
//--------------------------------------------
const int fan      = 9;
const int uvLed    = 10;
const int whiteLed = 11;
const int ws2812B  = A2;
//--------------------------------------------
//  Input Button GPIO
//--------------------------------------------
const int setButton  = A1;
const int upButton   = A0;
const int downButton = 13;
const int okButton   = 12;
const int confirmButton = A3;
//--------------------------------------------
//  Lcd GPIO
//--------------------------------------------
const int lcd_rs = 8;
const int lcd_e  = 7;
const int lcd_d4 = 6;
const int lcd_d5 = 5;
const int lcd_d6 = 4;
const int lcd_d7 = 3;
const int lcd_bl = 2; 

void gpioConfig()
{
    //--------------------------------------------
    // Configure GPIO mode as output/input
    //--------------------------------------------
    pinMode(fan, OUTPUT);
    pinMode(uvLed, OUTPUT);
    pinMode(whiteLed, OUTPUT);
    pinMode(ws2812B, OUTPUT);
    pinMode(setButton,INPUT);
    pinMode(upButton,INPUT);
    pinMode(downButton,INPUT);
    pinMode(okButton,INPUT);
    pinMode(confirmButton, INPUT);  
    pinMode(lcd_bl,OUTPUT);
}