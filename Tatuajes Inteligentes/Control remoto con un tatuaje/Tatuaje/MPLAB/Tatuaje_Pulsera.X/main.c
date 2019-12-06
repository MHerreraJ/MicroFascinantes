/************************************************************
 * main.c                           Created on: 06/08/2019  *
 *                                                          *
 *      Universidad Nacional Autonoma de Mexico             *
 *              Instituto de Ingeniería                     *
 *          Gobierno de la Ciudad de México                 *
 *                  Microfascinantes                        *
 *                                                          *
 *  Desarrollador: Manuel Andres Herrera Juarez.            *
 *                                                          *
 ************************************************************/

#include "Macros.h"
#include "CapacitiveSensor.h"
#include "TouchSlider.h"

#define IR_LOGIC_ONE (TCB1_CTRLB &= 0xEF)
#define IR_LOGIC_ZERO (TCB1_CTRLB |= 0x10)

#define IR_TIME_NEGATIVE_US     500
#define IR_TIME_LOW_OFFSET_US   1500
#define IR_TIME_LOW_FACTOR      200
#define IR_TIME_WAIT_NEXT_MS    55   

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

void initCPU_ATtiny1614();
void initTimerPWMLED();
void changeDutyPWM(uint8_t duty);
void sendIRBrightness(int db);
void rxUSART_Int(char by, const char* str, int size);
void sliderEvent_Int(uint8_t event, int speed);

int updateBrigth(int db);

//Inicializa CPU Attiny
//Establece frecuencia Reloj=5MHz
//Inicializa el contador de milisegundos
void initCPU_ATtiny1614(){
    CPU_CCP = 0xD8;
    CLKCTRL_MCLKCTRLB = 0x03; //Preescaler Disable
    initMillis();
}

//Inicializa timer PWM para emisor Infrarrojo
//Portadora de 38KHz
void initTimerPWMLED(){
    cli();
    PORTA.DIR |= 0x08; //PA3 as Output
    PORTA.OUT &= ~0x08;
    TCB1.CTRLB = 0x07; //Enable Output, Timer as PWM 

    TCB1.CCMPL = 0x41; //Set frequncy 38 KHz 
    TCB1.CCMPH = 0x20; //Set Duty = 50%

    TCB1.CNT = 0x0000; //Restart Timer Count
    TCB1.CTRLA = 0x03; //No Synchronize, F_TIMER = F_CPU/2
    sei();
    
}

/* Envia la información por infrarrojo
    Protocolo de envio:
    Para cambio en brillo positivo:

      ↓IDLE                 ↓IDLE
       _______              _______
              |             |
              |             |
              ↓_____________↑
  
              < --- dt --- >
    
    Para cambio en brillo negativo

      ↓IDLE   <--1000 us-->            ↓IDLE
       _______       _____              _______
              |     |     ↓            |
              |     |     |            |
              ↓_____↑     |____________↑
  
              ↑NEGATIVE IND↑
                          < --- dt --- >
    Infromacion recibida = r
    Duracion impulso = dt = 200*(r + 1) + 1500 [us]
    Incremento en brillo = |db| = (100/255)*r
        
*/
void sendIRBrightness(int db){    
    unsigned int lowTime_us = 0;
    unsigned int lowTime_ms = 0;
    
    lowTime_ms = (abs(db)+1)*IR_TIME_LOW_FACTOR + IR_TIME_LOW_OFFSET_US;    
    lowTime_us = lowTime_ms %1000;
    lowTime_ms = (lowTime_ms - lowTime_us) / 1000;
    
    if(db < 0){
        IR_LOGIC_ZERO;
        _delay_us(IR_TIME_NEGATIVE_US);
        IR_LOGIC_ONE;
        _delay_us(IR_TIME_NEGATIVE_US);
    }
    IR_LOGIC_ZERO;
    _delay_ms(lowTime_ms);
    _delay_us(lowTime_us);
    IR_LOGIC_ONE;
    _delay_ms(IR_TIME_WAIT_NEXT_MS);
    
}

/* Envia el brillo de acuerdo al tatuaje
 */
int updateBrigth(int db){
    sendIRBrightness(db);
    return 1;
}


/*Calculo del incremento en el brillo segun speed
 * y el tipo de evento en el sensor touch(Tatuaje)
 */
void sliderEvent_Int(uint8_t event, int speed){
    int db = (70-speed)/3;
    if(event == SLIDER_EVENT_LEFT){
        updateBrigth(-db);
    }else{
        updateBrigth(db);
    }
}

/*Función principal
 */
int main(void){
    initCPU_ATtiny1614();
    initTimerPWMLED();
    
    
    TouchSlider* slider;
    //Inicia una touchSlider
    slider = newTouchSlider(4, IO_PB0, 8, 0.15, IO_PA6, IO_PA5, IO_PA7, IO_PA4);
    attachSliderEvent(slider, sliderEvent_Int);
    
    //Checa permanentemente por eventos en el slider
    while(1){        
        checkSliderForEvent(slider);
        _delay_ms(2);
    }
    
    return 0;
}

