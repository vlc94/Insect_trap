#pragma once

#include <Wire.h>
#include "lcd.h"

//--------------------------------------------
//  RTC device address
//--------------------------------------------
#define RTC_ADDR 0x68

//--------------------------------------------
// Function to convert decimal to bcd
//--------------------------------------------
int decToBcd(int value)
{
  return ((value / 10 * 16) + (value % 10));
}

//--------------------------------------------
// Function to convert bcd to decimal
//--------------------------------------------
int bcdToDec(int value)
{
  return ((value / 16 * 10) + (value % 16));
}

//--------------------------------------------
// Function to set RTC  hour & min
//--------------------------------------------
void configRTC(int hr, int min)
{
        Wire.beginTransmission(RTC_ADDR);
        Wire.write(0);
        Wire.write(0);
        Wire.write(decToBcd(min));
        Wire.write(decToBcd(hr));
        Wire.endTransmission();
} 

void displayRTCTime()
{
    int tempHour=0;
    int tempMin=0;
    Wire.beginTransmission(0x68);
    Wire.write(0);
    Wire.endTransmission();
    Wire.requestFrom(0x68,7);
    tempMin = bcdToDec(Wire.read());
    tempMin = bcdToDec(Wire.read());
    tempHour  = bcdToDec(Wire.read());
    Wire.endTransmission(); 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Time:");
    lcd.setCursor(0,1);
    lcdDisplayDigit(0,1,tempHour);
    lcd.print(':');
    lcdDisplayDigit(3,1,tempMin);
}
