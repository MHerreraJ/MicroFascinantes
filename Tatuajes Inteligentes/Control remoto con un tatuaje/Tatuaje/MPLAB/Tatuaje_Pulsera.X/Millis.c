/************************************************************
 * Millis.h                         Created on: 14/08/2019  *
 *                                                          *
 *      Universidad Nacional Autonoma de Mexico             *
 *              Instituto de Ingeniería                     *
 *          Gobierno de la Ciudad de México                 *
 *                  Microfascinantes                        *
 *                                                          *
 *  Desarrollador: Manuel Andres Herrera Juarez.            *
 *                                                          *
 ************************************************************/
#include "Millis.h"

static volatile unsigned long msTimer = 0;

void initMillis(){
    //Timer A para millis(). Incremento variable cada 1ms
    cli();
    //Usar preescalador 
    TCA0.SINGLE.CTRLA &= 0xFE; //Disable Timer A
    TCA0.SINGLE.CTRLESET = 0x0C; //Force hard reset
    TCA0.SINGLE.CTRLESET = 0x01;
    TCA0.SINGLE.CTRLA &= 0xF1; //Clear Clock Select   
    TCA0.SINGLE.CTRLA |= 0x08; //Set F_TIMER = F_CPU/16
    TCA0.SINGLE.PER = (F_CPU/1000)/16; //Set Period to 1ms  
    TCA0.SINGLE.INTCTRL = 0x01; //Enable overflow Interrupt    
    TCA0.SINGLE.CTRLA |= 0x01; //Enable Timer A        
    sei();
}

ISR(TCA0_OVF_vect){
    msTimer++;
    TCA0.SINGLE.INTFLAGS |= 0x01; //Clear Flag
}

unsigned long millis(){
    long r = 0;
    cli();
    r = msTimer;
    sei();
    return r;
}