/************************************************************
 * NFC_PN532.ino                   Created on: 04/08/2019   *
 *                                                          *
 *      Universidad Nacional Autonoma de Mexico             *
 *              Instituto de Ingeniería                     *
 *          Gobierno de la Ciudad de México                 *
 *                  Microfascinantes                        *
 *                Tatuajes Inteligentes                     *
 *                                                          *
 *  Desarrollador: Manuel Andres Herrera Juarez.            *
 *                                                          *
 ************************************************************/
 
#include "NFCTextRecord.h"

#define NFC_CMD_ACK   'A'
#define NFC_CMD_WRITE 'W'
#define NFC_CMD_READ  'R'
#define NFC_CMD_NONE  'N'
#define NFC_CMD_STOP  'S'
#define NFC_CMD_INIT  'I'
#define NFC_CMD_READY 'R'
#define NFC_CMD_WAIT  'W'
#define NFC_CMD_ERROR 'E'
#define NFC_CMD_TIMEOUT 'T'

NFCTextRecord Nfc;

//Estructura manejadora del puerto Serial
struct{
  char data[150];
  char* pData;
  uint8_t count;  
  uint8_t full;
}UART;


//Estructura manejadora de los comandos
struct{
  uint8_t cmd;
  uint8_t cmdInt;
  char buffer[140];  
  int len;
}CMD;

/*Envia un mensaje al software maestro(Python)
 puede ser un acuse de recibido, un aviso de acción ejecutada
 o un mensaje de error*/
void sendAcknowledge(uint8_t cmd, uint8_t param, uint8_t nl=1){
  static uint8_t ackMsg[] = {0x00, 0x00, 0x0C, 0xFD, 'u', 'F', 'T', 'I', '\r', '\n'};
  ackMsg[0] = cmd;
  ackMsg[1] = param;
  Serial.write(ackMsg, nl?10:8);
}

/*
 * Decodifica la orden a ejecutar proveniente del puerto serial
 */
void decodeMessage(){
  CMD.cmdInt = 0;
  switch(toUpperCase(UART.data[0])){
    case NFC_CMD_WRITE:{ //Escribe datos al tatuaje
      CMD.cmd = NFC_CMD_WRITE;
      CMD.len = strlen(UART.data + 1);      
      memcpy(CMD.buffer, UART.data+1, CMD.len);      
    }break;
    case NFC_CMD_READ:{//Lee datos del tatuaje
      CMD.cmd = NFC_CMD_READ;
      CMD.len = 0;
    }break;
    case NFC_CMD_STOP:{//Deten cualquier operación pendiente
      CMD.cmd = NFC_CMD_NONE;
    }break;
    case NFC_CMD_INIT:{//Inicializa el firmware
      assertConnection();
      CMD.cmd = NFC_CMD_NONE;
    }break;
    default:{//Descarta comando
      CMD.cmd = NFC_CMD_NONE;
    }break;
  }    
}

/*
 * Función que verifica que este firmware sea compatible
 * con el software manejador(Python) a traves de un envio de claves
 */
void assertConnection(){
  CMD.cmd = NFC_CMD_NONE;
  CMD.cmdInt = 0;
  UART.pData = UART.data;
  UART.count = 0;
  UART.full = 0;
  
  long lastTime = millis();
  uint8_t assertKey[] = {'I', 'W', 0x0C, 0xFD, 'u', 'F', 'T', 'I'};

  //Ejecuta 
  while(1){
    serialEvent();
    if(UART.full){
      UART.full = 0;
      //Si el software manejador envia clave correcta, rompe el loop
      if(strncmp(UART.data, assertKey, 8) == 0) break;
    }
    if(millis() - lastTime > 500){
      //Envia un acuse de inicialización cada 500ms al software manejador
      sendAcknowledge(NFC_CMD_INIT, NFC_CMD_WAIT);    
      lastTime = millis();
    }
  }
  //Envia acuse de inicialización exitosa
  sendAcknowledge(NFC_CMD_INIT, NFC_CMD_READY);

  //Inicializa objeto del tipo NFCTextRecord
  //Si Conexión por I2C -> Nfc.init(NFCTXT_RECORD_USE_I2C);
  //Si Conexión por SPI -> Nfc.init(NFCTXT_RECORD_USE_SPI);
  Nfc.init(NFCTXT_RECORD_USE_SPI);
}
 
void setup(void) {
  //Inicializa puerto serial a 115200 bauds
  Serial.begin(115200);

  //Verifica compatibilidad con el software manejador(Python)
  assertConnection();    
}

void loop(void){
  //Si se recibió una cadena de texto por el puerto serial
  if(UART.full){
    //Decodifica comando
    decodeMessage();

    //Envia acuse de recibido
    sendAcknowledge(NFC_CMD_ACK, CMD.cmd);

    //Limpia bandera
    UART.full = 0;
  }

  //Si no hay comandos a ejecutar, regresa al principio de este método
  if(CMD.cmd == NFC_CMD_NONE) return;

  //Si el comando a ejecutar marca error por más de 15 veces,
  //libera comando y manda acuse de error por tiempo
  if(CMD.cmdInt++ >= 15){
    CMD.cmd = NFC_CMD_NONE;
    sendAcknowledge(NFC_CMD_ACK, NFC_CMD_TIMEOUT);
    return;    
  }

  //Espera 100ms
  delay(100);

  //Si el comando es escribir
  if(CMD.cmd == NFC_CMD_WRITE){
    //Trata de escribir la información, sino retorna al principio de loop()
    if(!Nfc.writeData(CMD.buffer, CMD.len)) return;

    //Si escribio correctamente, manda un acuse de acción completada
    sendAcknowledge(CMD.cmd, 0);
  }else if(CMD.cmd == NFC_CMD_READ){
    //Trata de leer la información
    CMD.len = Nfc.readData(CMD.buffer);

    //Si retorna un numero menor a 0, hay error
    if(CMD.len < 0){
      //Si retorna -2, el tatuaje se puede leer, pero no tiene la trama
      //correcta, avisa al software maestro con un acuse de error por datos
      //y libera acción
      if(CMD.len == -2){
        sendAcknowledge(CMD.cmd, NFC_CMD_ERROR);
        CMD.cmd = NFC_CMD_NONE;
      }
      return;
    }
    //Manda acuse de lectura correcta, con la información leida
    sendAcknowledge(CMD.cmd, 0, 0);
    Serial.write(CMD.buffer, CMD.len);
    Serial.println();
  }

  //Acción completada, libera comando
  CMD.cmd = NFC_CMD_NONE;
}

/*
 * Función de interrupción por byte recibido al puerto serial
 */
void serialEvent(){
  static char rxData;
  while(Serial.available()){
    rxData = Serial.read();
    if(!UART.full){   
      //Detecta un fin de linea   
      if(rxData == '\r' || rxData == '\n'){
        if(rxData == '\n'){
          UART.full = 1;     
          *UART.pData = '\0';
          UART.pData = UART.data;                       
        }
      }else{
        *UART.pData++ = rxData;
        UART.count++;
      }
    }           
  }
}
