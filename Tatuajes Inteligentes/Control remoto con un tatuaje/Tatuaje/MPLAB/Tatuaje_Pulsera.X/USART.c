#include "USART.h"

static struct USART{
    char buffer[31];
    uint8_t size;
    uint8_t index;
    uint8_t mode;
    uint8_t realIndex;
}USART = {{}, 0,0,0,0};

static void (*rxInterrupt)(char,const char*,int) = NULL;
static char stringEnd[16];
static int stringSize = 2;

ISR(USART0_RXC_vect){
    static uint8_t index = 0;
    //Check if new data in RxBuffer
    char rx = USART0_RXDATAH;
    if((rx&0x80) != 0x80) return;
    rx = USART0_RXDATAL;
    if(USART.mode == USART_INT_NONE || rxInterrupt == NULL) return;
    if(USART.mode == USART_INT_CHAR){
        USART.buffer[0] = rx;
        USART.buffer[1] = '\0';
        (*rxInterrupt)(rx, USART.buffer, 1);
        return;
    }
    
    if(USART.index < 30){
        USART.buffer[USART.index++] = rx;
    }
    USART.realIndex++;
    if(rx == stringEnd[index]){        
        index++;
        if(index == stringSize){
            index = 1;
            USART.size = USART.index;
            if(USART.realIndex-stringSize <= 30){
                USART.buffer[USART.realIndex-stringSize] = '\0';
            }else{
                USART.buffer[USART.index] = '\0';
            }
            USART.index = 0;
            USART.realIndex = 0;
            (*rxInterrupt)(rx, USART.buffer, USART.size);
        }
    }else{
        index = 0;
    }
        
}

void initUSART(unsigned long baud, uint8_t conf, void (*rxInt)(char, const char*,int)){
    cli(); //Disable global Interrupts
    if((conf & 0x01) == USART_PIN_ALT){
        PORTMUX_CTRLB |= 0x01; //Set USART0 Alternative Pins (PA1 Tx, PA2 Rx)
        PORTA_OUT |= 0x02; //Set PA1(TxD) High
        PORTA_DIR |= 0x02; //Set PA1 Output
    }else{
        PORTMUX_CTRLB &= 0xFE;
        PORTB_OUT |= 0x04; //Set PB2 (TxD) High
        PORTB_DIR |= 0x04; //Set PB2 Output
    }
    
    USART0_CTRLB &= 0x3F; //Disable Rx & Tx
    USART0_BAUD = (uint16_t)((4*F_CPU)/baud);
    USART0_CTRLC &= 0x3F; //Asynchronous USART
    USART0_CTRLC = (USART0.CTRLC & 0xF8) | 0x03; //8 bit character size
    USART0_CTRLB &= 0xF9; //RXMODE = Normal
    
    USART.size = 0;
    USART.index = 0;
    USART.realIndex = 0;
    USART.mode = conf & 0x06;
    switch(USART.mode){
        case USART_INT_NONE:{
            USART0_CTRLA &= 0x7F;
            break;
        }
        default:{
            rxInterrupt = rxInt;
            USART0_CTRLA |= 0x80;            
            break;
        }
    }
    strncpy(stringEnd, "\r\n", 2);
    stringSize = 2;    
        
    USART0_CTRLB |= 0xC0; //Enbale Rx & Tx        
    sei(); //Enable global Interrupts
}

 int setIntStringUSART(const char* txt, int size){
     if(USART.mode == USART_INT_CUSTOM){
        int sz = size <= 15?size:15;
        strncpy(stringEnd, txt, sz);
        stringSize = sz;
        stringEnd[sz] = '\0';
        return size<=15;
     }
     return 0;
 }

void sendByteUSART(char data){
    //Wait for Tx Buffer Empty
    while((USART0_STATUS & 0x20) != 0x20);
    USART0_TXDATAL = data;
}

void sendDataUSART(const char* data){
    while(*data){
        sendByteUSART(*data);
        data++;
    }
}

void printfUSART(const char* format, ...){
    const char* txt;

    union{
        long l;
        int i;
        double f;
        char* s;
        char c;
        unsigned int ui;
    }typeArg;

    va_list arg;
    va_start(arg, format);
    for(txt = format; *txt != '\0'; txt++){
        while(*txt != '%'){
            if(*txt == '\0'){
                va_end(arg);
                return;
            }
            sendByteUSART(*txt);
            txt++;
        }

        txt++;
        if(*txt == '\0'){
            va_end(arg);
            return;
        }

        switch(*txt){
            case 'c':
                typeArg.c = va_arg(arg, char);
                sendByteUSART(typeArg.c);
            break;
            case 'd':
                typeArg.i = va_arg(arg, int);
                sendDataUSART(decToString(typeArg.i));
            break;
            case 'b':
                typeArg.i = va_arg(arg, int);
                sendDataUSART(binToString(typeArg.i));
            break;
            case 'h':
                typeArg.i = va_arg(arg, int);
                sendDataUSART(hexToString(typeArg.i));
            break;
            case 'f':
                typeArg.f = va_arg(arg, double);
                sendDataUSART(formatDoubleStr(typeArg.f));
            break;
            case 's':
                typeArg.s = va_arg(arg, char*);
                sendDataUSART(typeArg.s);
            break;
            case '$':
                for(typeArg.i=0; typeArg.i<10; typeArg.i++){
                    sendByteUSART('\r');
                    sendByteUSART('\n');
                }
            break;
            case '#':
                sendByteUSART(0x0C);
            break;
            case '%':
                sendByteUSART('%');
            break;

        }
    }
    //Module 3: Closing argument list to necessary clean-up
    va_end(arg);
}