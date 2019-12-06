/************************************************************
 * TouchSlider.h                    Created on: 14/08/2019  *
 *                                                          *
 *      Universidad Nacional Autonoma de Mexico             *
 *              Instituto de Ingeniería                     *
 *          Gobierno de la Ciudad de México                 *
 *                  Microfascinantes                        *
 *                                                          *
 *  Desarrollador: Manuel Andres Herrera Juarez.            *
 *                                                          *
 ************************************************************/

#ifndef TOUCHSLIDER_H
#define	TOUCHSLIDER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define SLIDER_EVENT_NONE    0x00
#define SLIDER_EVENT_LEFT    0x01
#define SLIDER_EVENT_RIGHT   0x02
    
#include "Macros.h"
#include "CapacitiveSensor.h"


    /*  Estructura que almacena los datos necesarios
        para reconocer un slider touch(Tatuaje). Dicha
        estructura utiliza 'CapacitiveSensor' como apoyo
        para detectar 'toques' al tatuaje.
     */
   
    typedef struct TouchSlider{
        TouchSensor* inputs;
        uint8_t numSensors;
        uint8_t samples;
        
        long thd;
        float thdPercent;        
        
        uint16_t touchRead;
        uint16_t lastTouchRead;        
        
        uint8_t event;
        int speed;
        
        int counter2;
        int latency;
        
        void (*touchEvent)(uint8_t, int);
    }TouchSlider;

    TouchSlider* newTouchSlider(uint8_t sliderSize, uint8_t sendPin, uint8_t samples, float thdPercent, ...);
    
    void attachSliderEvent(TouchSlider* s, void (*touchEvent)(uint8_t, int));
    void checkSliderForEvent(TouchSlider* s);
    void calibrateSlider(TouchSlider* s);
    void readSliderTouchInputs(TouchSlider* s);
    
#ifdef	__cplusplus
}
#endif

#endif	/* TOUCHSLIDER_H */

