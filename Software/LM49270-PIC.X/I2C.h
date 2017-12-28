/* 
 * File:        I2C.h
 * Author:      Jackson Snowden
 * Comments:    Inter-Integrated Circuit function definitions
 */

#ifndef _I2C_H_
#define	_I2C_H_

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

#endif	/* XC_HEADER_TEMPLATE_H */ 

void I2C_Master_Init(const unsigned long clk);

void I2C_Master_Wait();

void I2C_Master_Start();

void I2C_Master_Repeated_Start();

void I2C_Master_Stop();

void I2C_Master_Write(unsigned char data);

void I2C_Master_Ack();

void I2C_Master_nAck();

unsigned char I2C_Master_Read(char ack);

#endif  /* _I2C_H_ */

