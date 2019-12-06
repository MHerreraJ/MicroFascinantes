/************************************************************
 * TouchSlider.c                    Created on: 14/08/2019  *
 *                                                          *
 *      Universidad Nacional Autonoma de Mexico             *
 *              Instituto de Ingeniería                     *
 *          Gobierno de la Ciudad de México                 *
 *                  Microfascinantes                        *
 *                                                          *
 *  Desarrollador: Manuel Andres Herrera Juarez.            *
 *                                                          *
 ************************************************************/

#include "TouchSlider.h"


/*  Funcion que retorna el primer bit menos 
    significativo que este en '1' en un 
    entero sin signo    */
static int getLSB1(unsigned int n){
    int index = 0;
    if(n == 0)  return -1;
    while(n != 0){
        if((n&0x01) == 0x01)    return index;
        n >>= 1;
        index++;
    }
    return -1;
}

/*  Funcion que retorna el primer bit más 
    significativo que este en '1' en un 
    entero sin signo    */
static int getMSB1(unsigned int n){
    int index = 0;
    int count = 0;
    if(n == 0)  return -1;
    while(n != 0){
        if((n&0x01) == 0x01)    index = count;        
        n >>= 1;
        count++;
    }
    return index;
}

/*  Retorna un apuntador a una nueva estructura del tipo
    TouchSlider.
    sliderSize = entero que especifica cuantos botones tiene el slider
    sendPin = Pin del uC común a todos los botones
    samples = Numero de muestras a tomar por botón
    thdPercent = Umbral de detección
    ... = Pines del tatuaje empezando por el brimer boton hasta el ultimo   
 */
TouchSlider* newTouchSlider(uint8_t sliderSize, uint8_t sendPin, uint8_t samples, float thdPercent, ...){
    TouchSlider* s = (TouchSlider*)malloc(sizeof(TouchSlider));
    va_list arg;
    va_start(arg, thdPercent);
    
    s->numSensors = sliderSize;
    s->inputs = (TouchSensor*)calloc(sliderSize, sizeof(TouchSensor));
    s->touchRead = 0x00;
    s->lastTouchRead = 0;
    
    uint8_t i;
    uint8_t readPin;
    for(i=0; i<sliderSize; i++){
        readPin = va_arg(arg, int);
        s->inputs[i] = *newTouchSensor(sendPin, (uint8_t)readPin);
    }
    va_end(arg);
    
    s->event = SLIDER_EVENT_NONE;
    s->speed = 0;    
    if(thdPercent < 0.0) thdPercent = 0.0;
    s->thdPercent = thdPercent;
    s->thd = 0;
    s->samples = samples;
    s->touchEvent = NULL;
    s->counter2 = 0;
    s->latency = 40;
    
    calibrateSlider(s);
    
    return s;
}

/*Apuntador a la función de ejecución cuando se detecte un evento touch*/
void attachSliderEvent(TouchSlider* s, void (*touchEvent)(uint8_t, int)){
    s->touchEvent = touchEvent;
}

/*Calibra el Slider*/
void calibrateSlider(TouchSlider* s){
    long val = 0;
    uint8_t i,j;
    for(i=0; i<70; i++){
        for(j=0; j<s->numSensors; j++){
            val += touchSensor(&(s->inputs[j]), s->samples);
        }
    }
    val = (long)(s->thdPercent*(float)val/(float)s->numSensors);
    s->thd = val;
}

/*Lee todos los botones del Slider
  Si las lecturas capacitivas están dentro del rango,
  asinga un '1' al boton correspondiente, sino '0'
 */
void readSliderTouchInputs(TouchSlider* s){
    uint8_t i;
    uint16_t touchMask = 0;
    long currentRead;
    for(i=0; i<s->numSensors; i++){
        currentRead = touchSensor(&(s->inputs[i]), s->samples);
        if(currentRead > s->thd){
            touchMask |= 1<<i;
        }
    }
    s->touchRead = touchMask;
}


/*  Checa si hay un evento en el slider
    puede ser: Desliz hacia la izquierda,
    Desliz a la derecha o ninguno.  */
void checkSliderForEvent(TouchSlider* s){
    readSliderTouchInputs(s);
    if(s->lastTouchRead == 0x00){
        if(s->touchRead != 0x00){
            s->speed = 0;
            s->counter2 = 0;
            s->event = SLIDER_EVENT_NONE;
            s->lastTouchRead = s->touchRead;
        }
    }else{
        s->speed++;
        if(s->touchRead != 0x00){
            if(getLSB1(s->touchRead) < getLSB1(s->lastTouchRead)){//Izquierda

                s->event = SLIDER_EVENT_LEFT;
                if(s->touchEvent != NULL){
                    (*s->touchEvent)(s->event, s->speed);
                }
                s->speed = 0;
                    
                s->counter2 = 0;
            }else if(getMSB1(s->touchRead) > getMSB1(s->lastTouchRead)){ //Derecha
                s->event = SLIDER_EVENT_RIGHT;
                if(s->touchEvent != NULL){
                    (*s->touchEvent)(s->event, s->speed);
                }
                s->speed = 0;             
            }else{
                goto CHECK_COUNTER2;
            }
            s->lastTouchRead = s->touchRead;
        }else{
            goto CHECK_COUNTER2;
        }
    }
    
    return;
    
    CHECK_COUNTER2:
    s->counter2++;
    if(s->counter2 > s->latency){
        s->speed = 0;
        s->lastTouchRead = 0;
        s->event = SLIDER_EVENT_NONE;
        s->counter2 = 0;
    }
    return;
}
