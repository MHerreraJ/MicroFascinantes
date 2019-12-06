/************************************************************
 * NFCTextRecord.cpp               Created on: 04/08/2019   *
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


//Inicializa un objeto del tipo NFCTextRecord
NFCTextRecord::NFCTextRecord() {
  cardBaudRate = PN532_MIFARE_ISO14443A;
  uid = (uint8_t*)calloc(7, sizeof(uint8_t));
  key = (uint8_t*)calloc(6, sizeof(uint8_t));
  buffer = (uint8_t*)calloc(156, sizeof(uint8_t));
  uidLenght = 0;
  for (int i = 0; i < 6; i++)  key[i] = 0xFF;
}


//Inicia comunicación con módulo PN532
void NFCTextRecord::init(uint8_t conn) {
  NFC  = (Adafruit_PN532*)malloc(sizeof(Adafruit_PN532));
  *NFC = (conn == NFCTXT_RECORD_USE_SPI)? Adafruit_PN532(10) : Adafruit_PN532(2, 3);
  NFC->begin();
  NFC->setPassiveActivationRetries(0xFF);
  NFC->SAMConfig();
}

//Retorna verdadero si detecta un tatuaje NFC en el campo, y si es un chip válido
boolean NFCTextRecord::isNFCAvailable() {
  if (!(NFC->readPassiveTargetID(cardBaudRate, uid, &uidLenght, 100)))  return false;
  if (!(NFC->mifareclassic_AuthenticateBlock(uid, uidLenght, 4, 0, key))) return false;
  return true;
}

//Checa continuamnene por 1.5 s si hay un tatuaje válido en el sensor
boolean NFCTextRecord::assertNFCAvailable(){
  if(++assertField == 4){
    while(!isNFCAvailable()){
      if(++assertField == 9){
        return false;
      }
      delay(300);
    }
    assertField = 0;
  }
  return true;     
}

//Método que arma bloque de datos a partir de la información a escribir o leer.
//*out = Apuntador al arreglo de 4 bytes donde se almacenara el paquete armado
//bloc = Número de bloque a armar
//*txData = Arreglo de datos que contiene la información a escribir.
//*len = Tamaño del arreglo de datos.
boolean NFCTextRecord::getBlock(uint8_t* out, uint8_t block, const uint8_t* txData, uint8_t len) {
  static uint8_t block5[] = {0x34, 0x03, 0x00, 0x92};
  static uint8_t block6[] = {0x04, 0x00, 0x77, 0x38};
  static uint8_t block7[] = {0x2F, 0x35, 0x00, 0x00};

  if (block < 5) return false;

  switch (block) {
    case 5: {
        block5[2] = len + 44 - 37;
        memcpy(out, block5, 4);
      } break;
    case 6: {
        block6[1] = len-37;
        memcpy(out, block6, 4);
      } break;
    case 7: {
        block7[2] = txData[0];
        block7[3] = txData[1];
        memcpy(out, block7, 4);
      } break;
    default:
      memcpy(out, &txData[(block - 8) * 4 + 2], 4);
      break;
  }
  return true;
}

//Lee bloque de datos
boolean NFCTextRecord::readBlock(uint8_t block, uint8_t *out) {
  return NFC->mifareclassic_ReadDataBlock(block, out);
}

//Escribe bloque de datos
boolean NFCTextRecord::writeBlock(uint8_t block, const uint8_t* data) {
  return NFC->mifareclassic_WriteDataBlock(block, data);
}

//Escribe información al tatuaje
//*data = cadena de texto a escribir en el tatuaje.
//*len = tamaño de la cadena de texto, si len=-1, autocalcula el tamaño
int NFCTextRecord::writeData(const char* data, int len) {
  static uint8_t task1[] = {0x54, 0x0F, 0x13}; //Trama 1
  static uint8_t task2[] = "android.com:pkgcom.wakdev.nfctasks"; //Trama2
  uint8_t txBuffer[16];
  uint8_t assertBuffer[16];

  if (!isNFCAvailable()) return 0;
  
  len = (uint8_t)(len == -1 ? strlen(data) : len);
  if (len > 92)   len = 92;
  assertField = 0;

  memcpy(buffer, data, len);
  memcpy(buffer+len, task1, 3);
  memcpy(buffer+len+3, task2, 34);
  len+=37;
  buffer[len] = 0xFE;
  for (uint8_t i = len + 1; i < len + 16; i++) buffer[i] = 0x00;
  
  uint8_t dataSend = 0;
  uint8_t wrBlock = 5;
  uint8_t i;
  while (dataSend < len + 11) {
      if(!assertNFCAvailable()) return 0; 
      getBlock(txBuffer, wrBlock, buffer, len);
      
      if (!writeBlock(wrBlock, txBuffer)) return 0;
      
      if(wrBlock < 8){
        for(i = 0; i<6; i++){
          if(readBlock(wrBlock, assertBuffer)){
            if(strncmp(assertBuffer, txBuffer, 4) == 0) break;
            writeBlock(wrBlock, txBuffer);
            delay(10); 
          }  
        }
        if(i==6) return 0;
      }
      
      wrBlock += 1;
      dataSend += 4;
  }

  return 1;
}

/*
 * Método que intenta leer datos de un tatuaje inteligente
 * *rxData = Apuntador al arreglo de datos donde se guardará la información leida
 * retorna el tamaño de la cadena leida si fue correcta, o un numero negativo si hubo errores de lectura
 */
int NFCTextRecord::readData(char* rxData) {
  uint8_t rxBuffer[16];
  uint8_t rxCmp[4];
  uint8_t len = 0;
  assertField = 0;
  
  if (!isNFCAvailable()) return -1;
  if (!readBlock(5, rxBuffer)) return -1;

  if (rxBuffer[2] < 44-37) return -1;
  len = rxBuffer[2]+ 37 - 44;
  getBlock(rxCmp, 5, NULL, len);

  if (strncmp(rxCmp, rxBuffer, 4) != 0) return -2;
  if (!readBlock(6, rxBuffer)) return -1;
  getBlock(rxCmp, 6, NULL, len);

  if (strncmp(rxCmp, rxBuffer, 4) != 0)    return -2;

  uint8_t dataRead = 0;
  uint8_t rdBlock = 7;

  while (dataRead < len + 2) {
    if(!assertNFCAvailable()) return -1; 
    
    if (!readBlock(rdBlock, rxBuffer)) return -1;

    if (rdBlock == 7) {
      memcpy(buffer, rxBuffer + 2, 14);
    } else {
      memcpy(buffer + dataRead - 2, rxBuffer, 16);
    }
    rdBlock+=4;
    dataRead += 16;
  }

  memcpy(rxData, buffer, len);
  rxData[len-37] = '\0';

  return len-37;
}
