#pragma once

#include "pins.h"

int8_t currentButtonState[4];           //Array to current button state
//int8_t lastButtonState[4]={1,1,1,1};    //Array to last button state
int8_t lastButtonState[4]={0,0,0,0};    //Array to last button state
//--------------------------------------------
// Function to read button state
// Current configuration : When button is pressed
// the input goes low , when this is detected
// function returns as true else false
// True: button press detected
// False : No button press detected
// Uses state change of gpio to avoid debounce
//--------------------------------------------
bool buttonPressed(const int button)
{
        bool state=false;
        currentButtonState[button] = digitalRead(button);
        if( (currentButtonState[button] == HIGH) && (lastButtonState[button] == LOW))
        {
            state =  true;
        }
        lastButtonState[button] = currentButtonState[button];
        return state;
}

int inc(int param, int8_t range)
{
   // int temp=0;
    if(param >= range)
    {
        param = 0;
    }
    else
    {
        param++;
    }
    return param;
} 

int dec(int param, int8_t range)
{
   // int temp=0;
    if(param <= 0)
    {
        param = range;
    }
    else
    {
        param--;
    }
    return param;
} 