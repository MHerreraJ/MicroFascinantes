/************************************************************
 * NFCTextRecord.h                 Created on: 04/08/2019   *
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
 
#ifndef __NFCWRITER_H__
#define __NFCWRITER_W__

#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define NFCTXT_RECORD_USE_SPI   0
#define NFCTXT_RECORD_USE_I2C   1

class NFCTextRecord{
    private:
        uint8_t cardBaudRate;
        uint8_t* uid;
        uint8_t uidLenght;
        uint8_t* key;
        uint8_t* buffer;
        uint8_t assertField;
        Adafruit_PN532* NFC;

        boolean isNFCAvailable();
        boolean assertNFCAvailable();
        boolean getBlock(uint8_t* out, uint8_t block, const uint8_t* txData, uint8_t len); //Write Mode

        boolean readBlock(uint8_t block, uint8_t *out);
        boolean writeBlock(uint8_t block, const uint8_t* data);


    public:
        NFCTextRecord();
        void init(uint8_t conn);
        int writeData(const char* data, int len=-1);
        int readData(char* rxData);

};

#endif
