/************************************************************
 * Macros.h                         Created on: 11/08/2019  *
 *                                                          *
 *      Universidad Nacional Autonoma de Mexico             *
 *              Instituto de Ingeniería                     *
 *          Gobierno de la Ciudad de México                 *
 *                  Microfascinantes                        *
 *                                                          *
 *  Desarrollador: Manuel Andres Herrera Juarez.            *
 *                                                          *
 ************************************************************/

#ifndef MACROS_H
#define	MACROS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define ALLINONE_FIRMWARE   1
#define CONTROL_FIRMWARE    2
    
#define TYPEOF_FIRMWARE CONTROL_FIRMWARE
//#define TYPEOF_FIRMWARE  ALLINONE_FIRMWARE
    
#define F_CPU 5000000UL
#define  __DELAY_BACKWARD_COMPATIBLE__

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#define IO_PA0     0x00
#define IO_PA1     0x01
#define IO_PA2     0x02
#define IO_PA3     0x03
#define IO_PA4     0x04
#define IO_PA5     0x05
#define IO_PA6     0x06
#define IO_PA7     0x07
    
#define IO_PB0     0x20
#define IO_PB1     0x21
#define IO_PB2     0x22
#define IO_PB3     0x23
#define IO_PB4     0x24
#define IO_PB5     0x25
#define IO_PB6     0x26
#define IO_PB7     0x27
    
    
#define GETIO_PORT(pin) ((((pin)>>4) == 0x00)? &PORTA : &PORTB)
#define GETIO_MASK(pin) ((uint8_t)(1<<((pin) & 0x07)))
#define GETIO_PIN(pin)  ((uint8_t)((pin) & 0x07))
    
#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_H */

