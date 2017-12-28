/* 
 * File:        Config.h
 * Author:      Jackson Snowden
 * Comments:    Base settings for this PIC
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

#endif	/* XC_HEADER_TEMPLATE_H */

#define _XTAL_FREQ  4000000

#define LM_ADDR     0x7C
#define CNT_REG     0x00
#define HP_REG      0x40
#define SP_REG      0x80

// Pin definitions
#define BUTTON_UP       RA5
#define BUTTON_DN       RA4
#define POT             RC3
#define MODE_SELECT     RC5

// Analog input pin channels
#define ADC_IN          7

// Initial volume used at startup
#define INIT_VOL     22

// Headphone volume offset from speaker volume
#define HP_OFFSET    10