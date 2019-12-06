/*  Libreria adaptada para trabajar con el Attiny1614
 *  Libreria original: 
 * 
 *  CapacitiveSense.h v.04 - Capacitive Sensing Library for 'duino / Wiring
    https://github.com/PaulStoffregen/CapacitiveSensor
    http://www.pjrc.com/teensy/td_libs_CapacitiveSensor.html
    http://playground.arduino.cc/Main/CapacitiveSensor
    Copyright (c) 2009 Paul Bagder  All right reserved.
 */

#ifndef CAPACITIVESENSOR_H
#define	CAPACITIVESENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "Macros.h"
#include "Millis.h"
    
#define DIRECT_MODE_INPUT(base, mask) ((base->DIR) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask) ((base->DIR) |= (mask))
#define DIRECT_WRITE_LOW(base, mask) ((base->OUT) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask) ((base->OUT) |= (mask))
#define DIRECT_READ(base, mask) (((base->IN) & (mask)) ? 1:0)
    
    typedef struct TouchSensor{
        int error;
        unsigned long  leastTotal;
        unsigned int   loopTimingFactor;
        unsigned long  CS_Timeout_Millis;
        unsigned long  CS_AutocaL_Millis;
        unsigned long  lastCal;
        unsigned long  total;
        uint8_t sBit;
        PORT_t* sReg;
        uint8_t rBit;
        PORT_t* rReg;
    }TouchSensor;

    TouchSensor* newTouchSensor(uint8_t txPin, uint8_t rxPin);
    long touchSensorRaw(TouchSensor* s, uint8_t samples);
    long touchSensor(TouchSensor* s, uint8_t samples);
    void setTSTimeoutMillis(TouchSensor* s, unsigned long timeout_millis);
    void resetTSAutoCal(TouchSensor* s);
    void setTSAutoCalMillis(TouchSensor* s, unsigned long autoCal_millis);
    long int senseOneCycle(TouchSensor* s);
    
#ifdef	__cplusplus
}
#endif

#endif	/* CAPACITIVESENSOR_H */

