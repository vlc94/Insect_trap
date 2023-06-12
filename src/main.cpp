/*
    Author: Vishnu
    Project: RTC based led & fan control
*/
/*
    Libraries used :
    EEPROM.h (Official Arduino)               -> Internal EEPROM read & write 
    LiquidCrystal.h (Official Arduino)        -> For lcd
    Wire.h (Official Arduino)                 -> Two wire protocol
    Adafruit_NeoPixel.h (Adafruit Industries) -> To control WS2812B led's

    Include files(Custom) : 
    button.h    -> button related functions 
    lcd.h       -> Lcd functions
    pins.h      -> Gpio mapping & configuration
    rtc.h       -> Real time clock functions
    timer.h     -> timer configurations
*/
//--------------------------------------------
// Define firmware version
//--------------------------------------------
#define FW_VERSION "V0.1"

#include <Arduino.h>
#include <stdio.h>
#include <EEPROM.h>
/*
    Neopixel library by adafruit
    For more refer to adafruit website
*/
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "lcd.h"
#include "pins.h"
#include "timer.h"
#include "rtc.h"
#include "button.h"

//--------------------------------------------
// Global variables
//--------------------------------------------
#define ON 1
#define OFF 0

int8_t page = 0;
bool ONTime = false;
bool OFFTime = false;
bool setRTCParam = false;

int onTimeHour;
int onTimeMinute;
int offTimeHour;
int offTimeMinute;

int8_t RTCTimeHour;
int8_t RTCTimeMinute;

//for pheripherals
int cycles;
int8_t _uv;
int8_t _wLed;
char _rgb;
int8_t _fan;

static int8_t flag = 0;
int8_t subPageCount = 0;
int8_t pageFlag = 0;

//--------------------------------------------
// Declare NeoPixel strip object
//--------------------------------------------
#define LED_COUNT 3  
Adafruit_NeoPixel pixel(LED_COUNT, ws2812B, NEO_GRB + NEO_KHZ800);


//--------------------------------------------
// Function prototypes
//--------------------------------------------
void mainMenu();
void subMenu();
void incParam();
void decParam();
void readDataFromEEPROM();

void setup() 
{
  // put your setup code here, to run once:
    gpioConfig(); //Congiure gpio
    //By default turn on LCD backlight
    digitalWrite(lcd_bl,HIGH);
    //Lcd initialisation
    setupLcd();
    //Display welcome message
    lcd.setCursor(8,0);     //Set cursor position ex 4th column, 0-1st line
    lcd.print("Welcome");   
    //Display firmware version
    lcd.setCursor(6,1);
    lcd.print(FW_VERSION);  
    // I2C Initialisation
    Wire.begin();
    while (1);
    
    // Display RTC Time
    displayRTCTime();
    delay(2000); 
    //Read parameters
    readDataFromEEPROM();
    delay(2000);
    if(cycles == 0)
    {
        //By default turn off all pheripherals
        digitalWrite(fan, LOW);
        digitalWrite(uvLed, LOW);
        digitalWrite(whiteLed, LOW);
        //Initialize pixel object 
        pixel.begin();  
        // Turn off all RGB leds off      
        pixel.setPixelColor(0,0,0,0);
        pixel.setPixelColor(1,0,0,0);
        pixel.setPixelColor(2,0,0,0);
        pixel.show(); 
    }
}

void loop() 
{
  // put your main code here, to run repeatedly:
    if(flag == 60 && cycles > 0 )
    {   
        flag = 0;
        int tempMin, tempHour;
        Wire.begin();
        Wire.beginTransmission(0x68);
        Wire.write(1);
        Wire.endTransmission();
        Wire.requestFrom(0x68,7);
        tempMin = bcdToDec(Wire.read());
        tempHour = bcdToDec(Wire.read());
        Wire.endTransmission(); 

        if(tempMin == onTimeMinute && tempHour == onTimeHour)
        {
            digitalWrite(fan, HIGH);  //fan
            digitalWrite(uvLed, HIGH); //UV led
            digitalWrite(whiteLed, HIGH); // white led
            if(_rgb == 'R')
            {
                pixel.setPixelColor(0,255,0,0);
                pixel.setPixelColor(1,255,0,0);
                pixel.setPixelColor(2,255,0,0);
            }
            else if(_rgb == 'G')
            {
                pixel.setPixelColor(0,0,255,0);
                pixel.setPixelColor(1,0,255,0);
                pixel.setPixelColor(2,0,255,0);
            }
            else 
            {
                pixel.setPixelColor(0,0,0,255);
                pixel.setPixelColor(1,0,0,255);
                pixel.setPixelColor(2,0,0,255);
            }
            pixel.show();
            
        }
        if(tempMin == offTimeMinute && tempHour == offTimeHour)
        {
            digitalWrite(fan, LOW);  //fan
            digitalWrite(uvLed, LOW); //UV led
            digitalWrite(whiteLed, LOW); // white led
            pixel.setPixelColor(0,0,0,0);
            pixel.setPixelColor(1,0,0,0);
            pixel.setPixelColor(2,0,0,0);
            pixel.show();
            cycles -- ;
        }
    }
    else
    {
        if(buttonPressed(setButton) == true )
        {
            mainMenu();
        }
        else if(buttonPressed(okButton) == true)
        {
            subMenu();
        }
        else if(buttonPressed(upButton) == true)
        {
            incParam();
        }
        else if(buttonPressed(downButton) == true)
        {
            decParam();
        }
        else if(buttonPressed(confirmButton) ==  true)
        {
            if(setRTCParam == true)
            {
                /*
                Save RTC time entered by user
                */
            /*  Wire.beginTransmission(RTC_ADDR);
                Wire.write(0);
                Wire.write(0);
                Wire.write(decToBcd(RTCTimeMinute));
                Wire.write(decToBcd(RTCTimeHour));
                Wire.endTransmission(); */
                configRTC(RTCTimeHour, RTCTimeMinute);
                setRTCParam = false;   
            }
            if(ONTime == true || OFFTime == true)
            {
                EEPROM.write(0, onTimeHour);
                EEPROM.write(1, onTimeMinute);
                EEPROM.write(2, offTimeHour);
                EEPROM.write(3, offTimeMinute);
                EEPROM.write(4, cycles);  
                //
                setRTCParam = false;
                ONTime = false;
                OFFTime = false;
                subPageCount = 0;
                page = 0;
                flag = 0;
                pageFlag = 0;
                //
                // Start timer
                timer1Init();
            }
            /*
                Save ON time & OFF time parameters
            */
            /*  EEPROM.write(5, _uv);
            EEPROM.write(6, _wLed);
            EEPROM.write(6, _rgb); 
            EEPROM.write(7, _fan); */

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Parameters saved");
            delay(1000);
            readDataFromEEPROM();   
            ONTime = false;
            OFFTime = false;       
        }
    }
}

//--------------------------------------------
// Interrupt ISR routine
//--------------------------------------------
ISR(TIMER1_COMPA_vect)
{
    flag++; //Increment value of variable everytime when isr is executed
}

void mainMenu()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    switch(page)
    {
        case 0: lcd.clear();
                lcd.print("Set ON time:");
                lcd.setCursor(0,1);
                ONTime = true;
                OFFTime = false;
                setRTCParam = false;
                subPageCount = 0;
        break;

        case 1: lcd.clear();
                lcd.print("Set OFF time:");
                lcd.setCursor(0,1);
                ONTime = false;
                OFFTime = true;
                setRTCParam = false;
                subPageCount = 0;
        break;

        case 2: lcd.print("Set clock:");
                lcd.setCursor(0,1);
                OFFTime = false;
                ONTime = false;
                setRTCParam = true;
                subPageCount = 0;
                    //add funtion to set time & parameters
        break;
    } 
    if(page >=2)
    {
        page = 0;
    }
    else
    {
        page++;
    }
}
void subMenu()
{
    if(ONTime == true && OFFTime == false)
    {
        subPageCount++;
        switch (subPageCount)
        {
            case 1  :   lcd.setCursor(0,1);
                        lcd.print("                ");
                        lcdDisplayDigit(0,1,onTimeHour);
                        lcd.print(':');
                        lcdDisplayDigit(3,1,onTimeMinute);
                        lcd.setCursor(15,1);
                        lcd.print('H');
                        pageFlag = 1;
            break;

            case 2  :   lcdDisplayDigit(3,1,onTimeMinute);
                        lcd.setCursor(15,1);
                        lcd.print('M');
                        pageFlag = 2;
            break;

            case 3  :   lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Set UV light:");
                        lcd.setCursor(0,1);
                        if(_uv == ON)
                        {
                        lcd.print("ON");
                        }
                        else
                        {
                        lcd.print("OFF");
                        } 
                        pageFlag = 3;
            break;

            case 4  :   lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Set RGB:"); 
                        lcd.setCursor(0,1);
                        lcd.print(_rgb);
                        pageFlag = 4;;
            break;

            case 5  :   lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Set White light:");
                        lcd.setCursor(0,1);
                        if(_wLed == ON)
                        {
                            lcd.print("ON");
                        }
                        else
                        {
                            lcd.print("OFF");
                        }  
                        pageFlag = 5;
            break;

            case 6  :   lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Set fan:");
                        lcd.setCursor(0,1);
                        if(_fan == ON)
                        {
                            lcd.print("ON");
                        }
                        else
                        {
                            lcd.print("OFF");
                        } 
                        pageFlag = 6;
            break;

            case 7  :   lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Set cycles:");
                        lcd.setCursor(0,1);
                        lcd.print(cycles);
                        pageFlag = 7;
                        subPageCount = 0;
            break; 
        }
    }
    else if(ONTime == false && OFFTime == true)
    {
        subPageCount++;
        if(subPageCount == 1)
        {
            lcd.setCursor(0,1);
            lcd.print("                "); 
            lcdDisplayDigit(0,1,offTimeHour);
            lcd.print(':');
            lcdDisplayDigit(3,1,offTimeMinute);
            lcd.setCursor(15,1);
            lcd.print('H');
            pageFlag = 8;
        }
        else if(subPageCount == 2)
        {
            lcdDisplayDigit(3,1,offTimeMinute);
            lcd.setCursor(15,1);
            lcd.print('M');
            //pageFlag = 4;
            subPageCount = 0;
            pageFlag = 9;
        }
    } 
    else if(setRTCParam == true && OFFTime == false && ONTime == false)
    {
      // write function to read RTC time
        subPageCount++;
        if(subPageCount == 1)
        {
            lcd.setCursor(0,1);
            lcd.print("                "); 
            lcdDisplayDigit(0,1,RTCTimeHour);
            lcd.print(':');
            lcdDisplayDigit(3,1,RTCTimeMinute);
            lcd.setCursor(15,1);
            lcd.print('H');
            pageFlag = 10;
        }
        else if(subPageCount == 2)
        {
            lcdDisplayDigit(3,1,RTCTimeMinute);
            lcd.setCursor(15,1);
            lcd.print('M');
            subPageCount = 0;
            pageFlag = 11; 
        }
    }
} 
//--------------------------------------------
// Increment parameter
//--------------------------------------------
void incParam()
{
    switch(pageFlag)
    {
        case 1  :   onTimeHour = inc(onTimeHour,23);
                    lcdDisplayDigit(0,1,onTimeHour);
        break;

        case 2  :   onTimeMinute = inc(onTimeMinute,59);
                    lcdDisplayDigit(3,1,onTimeMinute);
        break;

        case 3  :   lcd.setCursor(0,1);
                    lcd.print("    ");
                    lcd.setCursor(0,1);
                    if(_uv == ON)
                    {
                        _uv = OFF;
                        lcd.print("OFF");
                    }
                    else
                    {
                        _uv = ON;
                        lcd.print("ON");
                    }
        break;

        case 4: lcd.setCursor(0,1);
                if(_rgb == 'R')
                {
                    _rgb = 'G';
                    lcd.print(_rgb);
                }
                else if(_rgb == 'G')
                {
                    _rgb = 'B';
                    lcd.print(_rgb);
                }
                else
                {
                    _rgb = 'R';
                    lcd.print(_rgb);
                }
        break;

        case 5  :   lcd.setCursor(0,1);
                    lcd.print("                 ");
                    lcd.setCursor(0,1);if(_wLed == ON)
                    {
                    _wLed = OFF;
                    lcd.print("OFF");
                    }
                    else
                    {
                    _wLed = ON;
                    lcd.print("ON");
                    }
        break;

        case 6  :   lcd.setCursor(0,1);
                    lcd.print("                 ");
                    lcd.setCursor(0,1); 
                    if(_fan == ON)
                    {
                    _fan = OFF;
                    lcd.print("OFF");
                    }
                    else
                    {
                    _fan = ON;
                    lcd.print("ON");
                    }
        break;

        case 7  :   cycles =inc(cycles,10);
                    lcd.setCursor(0,1);
                    lcd.print("                 ");
                    lcd.setCursor(0,1); 
                    lcd.print(cycles);
        break;

        case 8  :   offTimeHour = inc(offTimeHour,23);
                    lcdDisplayDigit(0,1,offTimeHour);
        break;

        case 9  :   offTimeMinute = inc(offTimeMinute,59);
                    lcdDisplayDigit(3,1,offTimeMinute);
        break;

        case 10  :  RTCTimeHour = inc(RTCTimeHour,23);
                    lcdDisplayDigit(0,1,RTCTimeHour);
        break;

        case 11  :  RTCTimeMinute = inc(RTCTimeMinute,59);
                    lcdDisplayDigit(3,1,RTCTimeMinute);
        break;
    }
}
//--------------------------------------------
// Decrement parameter
//--------------------------------------------
void decParam()
{
    switch (pageFlag)
    {
        case 1  :   onTimeHour = dec(onTimeHour,23);
                    lcdDisplayDigit(0,1,onTimeHour);
        break;

        case 2  :   onTimeMinute = dec(onTimeMinute,59);
                    lcdDisplayDigit(3,1,onTimeMinute);
        break;

        case 7  :   cycles =dec(cycles,10);
                    lcd.setCursor(0,1);
                    lcd.print("                 ");
                    lcd.setCursor(0,1); 
                    lcd.print(cycles);
        break;
        case 8  :   offTimeHour = dec(offTimeHour,23);
                    lcdDisplayDigit(0,1,offTimeHour);
        break;

        case 9  :   offTimeMinute = dec(offTimeMinute,59);
                    lcdDisplayDigit(3,1,offTimeMinute);
        break;

        case 10  :  RTCTimeHour = dec(RTCTimeHour,23);
                    lcdDisplayDigit(0,1,RTCTimeHour);
        break;

        case 11  :  RTCTimeMinute = dec(RTCTimeMinute,59);
                    lcdDisplayDigit(3,1,RTCTimeMinute);
        break; 
    }
}
//--------------------------------------------
// Function to read data from EEPROM
//--------------------------------------------
void readDataFromEEPROM()
{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ON ");
        //Display saved On & Off Time 
        lcdDisplayDigit(3,0,EEPROM.read(0));
        lcd.print(':');
        lcdDisplayDigit(6,0, EEPROM.read(1));

        lcd.setCursor(0,1);
        lcd.print("OFF ");
        
        lcdDisplayDigit(4,1,EEPROM.read(2));
        lcd.print(':');
        lcdDisplayDigit(7,1,EEPROM.read(3));
}
