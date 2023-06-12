#pragma once

//--------------------------------------------
// Timer Initalisation
// Timer interrupt
// Timer : Timer 1 
// Prescalar :
// Mode :
//--------------------------------------------
void timer1Init()
{
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    OCR1A = 15624;
    TCCR1B|= (1 << CS10) | (1 << CS12) ;
    interrupts();
}