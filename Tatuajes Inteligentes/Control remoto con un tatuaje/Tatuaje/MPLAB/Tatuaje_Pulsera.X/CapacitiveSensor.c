/*  Libreria adaptada para trabajar con el Attiny1614
 *  Libreria original: 
 * 
 *  CapacitiveSense.h v.04 - Capacitive Sensing Library for 'duino / Wiring
    https://github.com/PaulStoffregen/CapacitiveSensor
    http://www.pjrc.com/teensy/td_libs_CapacitiveSensor.html
    http://playground.arduino.cc/Main/CapacitiveSensor
    Copyright (c) 2009 Paul Bagder  All right reserved.
 */

#include "CapacitiveSensor.h"

TouchSensor* newTouchSensor(uint8_t txPin, uint8_t rxPin){
    TouchSensor* s = (TouchSensor*)calloc(1, sizeof(TouchSensor));
    s->error = 1;
    s->loopTimingFactor = 310;
    s->CS_Timeout_Millis = (2000 * (float) s->loopTimingFactor * (float)F_CPU)/16000000;
    s->CS_AutocaL_Millis = 20000;        
    
    s->sBit = GETIO_MASK(txPin);//s->sBit = 1<<txPin;
    s->sReg = GETIO_PORT(txPin);//s->sReg = txReg;
    s->rBit = GETIO_MASK(rxPin);//s->rBit = 1<<rxPin;
    s->rReg = GETIO_PORT(rxPin);//s->rReg = rxReg;
    
    *((uint8_t *)s->sReg + 0x10 + GETIO_PIN(txPin)) = 0x00;
    *((uint8_t *)s->rReg + 0x10 + GETIO_PIN(rxPin)) = 0x00;
    
    DIRECT_MODE_OUTPUT(s->sReg, s->sBit);
    DIRECT_MODE_INPUT(s->rReg, s->rBit);
    DIRECT_WRITE_LOW(s->sReg, s->sBit);
    
    s->leastTotal = 0x0FFFFFFFL;
    s->lastCal = millis();
    return s;
}

long touchSensor(TouchSensor* s, uint8_t samples){
    s->total = 0;
    if(samples == 0) return 0;
    if(s->error < 0) return 0;
    uint8_t i;
    for(i=0; i<samples; i++){
        if(senseOneCycle(s) < 0) return -2;
    }
    
    if(((millis() - (s->lastCal)) > (s->CS_AutocaL_Millis)) && (abs((s->total) - (s->leastTotal)) < (int)(.10*(float)(s->leastTotal)))){
        s->leastTotal =  0x0FFFFFFFL;   
        s->lastCal = millis();
    }
    if((s->total) < (s->leastTotal)) s->leastTotal = s->total;
    return ((s->total) - (s->leastTotal));
}

long touchSensorRaw(TouchSensor* s, uint8_t samples){
    s->total = 0;
    if(samples == 0) return 0;
    if((s->error) < 0) return -1;
    uint8_t i;
    for(i=0; i< samples; i++){
        if(senseOneCycle(s) < 0) return -2;
    }
    return s->total;
}

void resetTSAutoCal(TouchSensor* s){
    s->leastTotal = 0x0FFFFFFFL;
}

void setTSAutoCalMillis(TouchSensor* s, unsigned long autoCal_millis){
    s->CS_AutocaL_Millis = autoCal_millis;
}

void setTSTimeoutMillis(TouchSensor* s, unsigned long timeout_millis){
    s->CS_Timeout_Millis = (timeout_millis * (float)(s->loopTimingFactor) * (float)F_CPU)/16000000;
}

long int senseOneCycle(TouchSensor* s){
    cli();
    DIRECT_WRITE_LOW(s->sReg, s->sBit);
    DIRECT_MODE_INPUT(s->rReg, s->rBit);
    DIRECT_MODE_OUTPUT(s->rReg, s->rBit);
    DIRECT_WRITE_LOW(s->rReg, s->rBit);
    _delay_us(10);
    DIRECT_MODE_INPUT(s->rReg, s->rBit);
    DIRECT_WRITE_HIGH(s->sReg, s->sBit);
    sei();
    
    while(!DIRECT_READ(s->rReg, s->rBit) && ((s->total) < (s->CS_Timeout_Millis))){
        s->total++;
    }
    
    if((s->total) > (s->CS_Timeout_Millis)){
        return -2;
    }
    
    cli();
    DIRECT_WRITE_HIGH(s->rReg, s->rBit);
    DIRECT_MODE_OUTPUT(s->rReg, s->rBit);
    DIRECT_WRITE_HIGH(s->rReg, s->rBit);
    DIRECT_MODE_INPUT(s->rReg, s->rBit);
    DIRECT_WRITE_LOW(s->sReg, s->sBit);
    sei();
    
    while(DIRECT_READ(s->rReg, s->rBit) && ((s->total) < (s->CS_Timeout_Millis))){
        s->total++;
    }
    
    if((s->total) >= (s->CS_Timeout_Millis)){
        return -2;
    }else{
        return 1;
    }
}
