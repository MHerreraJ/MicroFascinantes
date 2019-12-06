
/************************************************************
 * Control_Potencia_Shield_IR.ino  Created on: 04/08/2019   *
 *                                                          *
 *      Universidad Nacional Autonoma de Mexico             *
 *              Instituto de Ingeniería                     *
 *          Gobierno de la Ciudad de México                 *
 *                  Microfascinantes                        *
 *                                                          *
 *  Desarrollador: Manuel Andres Herrera Juarez.            *
 *                                                          *
 ************************************************************/

 //Pines shield de Potencia
#define TRIAC_ENABLE 3
#define ZERO_DETECTOR 4
#define IR_RECEIVER 7

//Estados FSM receptor infrarrojo
#define IR_IDLE 0
#define IR_FIRST_RISING 1
#define IR_SECOND_RISING 2
#define IR_NEGATIVE 2
#define IR_ERROR 3

//Brillo del foco
int bright = 0;

//Control de fase
int tDelay = 0;

//Bandera detector de cruce por cero
bool detectZero = false;


//Interrupcion PCINT2 Detecta cruce por cero 
//y datos del infrarrojo provenientes del tatuaje
ISR(PCINT2_vect){
  static unsigned int lastTime = 0;
  static uint8_t irFsm = IR_IDLE;
  static int lastIRState = -1;

  unsigned int currentTime = micros();
  unsigned int dt = currentTime - lastTime;


  //Detecta cruce por cero
  if((PIND & 0x10) == 0x10){
    if(!detectZero){
      detectZero = true;
    }
  }

/*  Cambio detectado en receptor infrarrojo
    Protocolo de recepción:
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
  if((PIND & 0x80) == 0x80){ //RISING_EDGE
    if(lastIRState != 1){     
      if(irFsm == IR_FIRST_RISING){        
        if(dt > 400 && dt < 600){ //500 us delay
          irFsm = IR_SECOND_RISING;
          lastTime = currentTime;
        }else if(dt > 1200 && dt <9500){ //1500 us offset
          updateBrightness(dt, 1);
          irFsm = IR_IDLE;
        }else{
          irFsm = IR_IDLE;
        }
      }else if(irFsm == IR_NEGATIVE){
        updateBrightness(dt, -1);
        irFsm = IR_IDLE;
      }else if(irFsm == IR_ERROR){
        irFsm = IR_IDLE;
      }     
      lastIRState = 1;
    }
  }else{ //FALLING_EDGE
    if(lastIRState != 0){
      if(irFsm == IR_IDLE){
        lastTime = currentTime;
        irFsm = IR_FIRST_RISING;
      }else if(irFsm == IR_SECOND_RISING){
        if(dt > 400 && dt <600){ //500 us delay
          lastTime = currentTime;
          irFsm = IR_NEGATIVE;
        }else{
          irFsm = IR_ERROR;
        }
      }else{
        irFsm = IR_ERROR;
      }
      
      lastIRState = 0;
    }
  }
  
}

char txBuffer[30];
void updateBrightness(unsigned int dt, int sign){
  int db = 0;
  //Calcula brillo a partir de 
  //dt = 200*(r + 1) + 1500 [us]
  //db = (100/255)*r
  db = sign*((dt-1500)/200 - 1);
  db = (int)(100.0*(float(db))/255.0);

  //Utiliza db como auxiliar para aumentar el brillo global del foco
  //Trunca los valores si no esta de 0% a 100%
  db += bright;
  if(db < 0) db = 0;
  if(db > 100) db = 100;

  //Actualiza el brillo global del foco
  bright = db;

  //Calcula el tiempo de retraso en la fase a partir del brillo
  tDelay = (int)(((340.0*(23.0-float(bright*bright)/500.0)))); 
}

void setup() {
  //Establece los pines del shield como entrada/salida
  pinMode(ZERO_DETECTOR, INPUT);
  pinMode(IR_RECEIVER, INPUT);
  pinMode(TRIAC_ENABLE, OUTPUT);

  //Deshabilita el IC que controla el foco. 
  //Fase inicial de 8000 us (Foco apagado)
  digitalWrite(TRIAC_ENABLE,0);
  tDelay = 8000;
  
  //ZERO DETECTOR = PCINT20
  PCIFR |= 0x04; //Clear pending interrupts
  PCICR |= 0x04; //Enable PCIE2 Interrupt (PCINT23..16)
  PCMSK2 = 0x10 | 0x80; //Mask PCINT20, PCINT23

}

void loop() {
  //Si se detecta el cruce por cero:
  if(detectZero){
    if(bright != 0){
      //Si el brillo es diferente de 0, deja
      //apagado el TRIAC por un tiempo tDelay
      //y dejalo encendido
      digitalWrite(TRIAC_ENABLE, 0);
      delayMicroseconds(tDelay);
      digitalWrite(TRIAC_ENABLE, 1);
      delayMicroseconds(10);
    }else{
      digitalWrite(TRIAC_ENABLE, 0);
    }    
    //Limpia bandera
    detectZero = 0;
  }
}

