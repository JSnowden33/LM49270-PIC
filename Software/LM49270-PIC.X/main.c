/*
 * File:   main.c
 * Author: Jackson Snowden
 */

#include <xc.h>
#include "Config.h"
#include "I2C.h"

signed int vol;
unsigned int SP_vol;
unsigned int HP_vol;

unsigned char PWR_state;
unsigned char mode;

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

void PIC_Init()
{
    // Internal oscillator enabled at 4 MHz
    OSCCON = 0b11101010;
    
    // Enable IOC for RA4 and RA5
    IOCAP = 0b00000000;
    IOCAN = 0b00110000;
    IOCAF = 0x00;
    INTCON = 0b10001000;
    
    // Pull-ups disabled
    nWPUEN = 1;     
    
    // Timer 0 no prescaler
    TMR0CS = 1;
    PSA = 1;
    
    // All I/O are input
    TRISA = 0xFF;
    TRISC = 0xFF;
    
    // RC3 is analog input
    ANSELA = 0b00000000;
    ANSELC = 0b00001000;
    
    // Set VREF to VDD  
    ADCON1 = 0b11010000;    
}

unsigned int Read_ADC(unsigned char channel)
{   
    unsigned int result;

    // Turn on A/D module, set channel to specified channel
    ADCON0 = 0b00000001 | (channel << 2);   

    __delay_ms(5);                          // Wait for sampling cap to charge
    GO_nDONE = 1;                           // Start conversion
    while (GO_nDONE == 1);                  // Wait for conversion
    result = (ADRESH << 8) | ADRESL;        // Right-justified, 10-bit result
    
    return result;
}

void LM_Write(unsigned char reg, char data)
{
    I2C_Master_Start();
    I2C_Master_Write(LM_ADDR << 1);            
    I2C_Master_Write(reg | (data & 0x1F));    
    I2C_Master_Stop();
}

void Set_Volume()
{
    LM_Write(SP_REG, SP_vol);
    LM_Write(HP_REG, HP_vol);
    
    // Power off the amp if it's on and volume is 0
    if ((PWR_state == 1) && (vol == 0))
    {
        LM_Write(CNT_REG, 0x02);
        PWR_state = 0;
    }
    
    // Power on the amp if it's off and volume increases
    if ((PWR_state == 0) && (vol != 0))
    {
        LM_Write(CNT_REG, 0x03);
        PWR_state = 1;
    }
}

void Volume_Up()
{
    if (vol < 31)
        vol++;
    else
        vol = 31;
        
    SP_vol = vol;
    
    if (vol > HP_OFFSET)
        HP_vol = vol - HP_OFFSET;
    else
        HP_vol = 0;
}

void Volume_Down()
{
    if (vol > 0)
        vol--;
    else
        vol = 0;
    
    SP_vol = vol;
        
    if (vol > HP_OFFSET)
        HP_vol = vol - HP_OFFSET;
    else
        HP_vol = 0;
}

void Check_ADC()
{
    // Convert 10-bit ADC reading to a 5-bit value
    unsigned int ADC_val = Read_ADC(ADC_IN) >> 5;
    
    if (ADC_val != vol)
    {
        vol = ADC_val;
        
        SP_vol = vol;
        
        if (vol > HP_OFFSET)
            HP_vol = vol - HP_OFFSET;
        else
            HP_vol = 0;
        
        Set_Volume();
    }
}

void LM_Init()
{
    // Headphone and speaker 3D effect off, charge pump enabled, device power on
    LM_Write(CNT_REG, 0x03);
    PWR_state = 1;
  
    // Get initial volume from potentiometer or from stored value
    if (mode)
    {
        Check_ADC();
    }
    else
    {
        vol = INIT_VOL;
        SP_vol = vol;
        HP_vol = vol - HP_OFFSET;
    }
    
    // Set initial volume
    Set_Volume();
}

void interrupt ISR()
{
    // If BUTTON_UP has been pressed
    if (IOCAF5)
    {
        // Start counting with Timer 0
        unsigned int count = 0;
        TMR0 = 0x00;
        TMR0IF = 0;
        TMR0CS = 0;
        
        // Hang until button has been held for ~50ms
        while ((!BUTTON_UP) && (count < 200))
        {
            if (TMR0IF)
            {
                count++;
                TMR0IF = 0;
            }
        }
        
        // Clear timer
        TMR0CS = 1;
        TMR0IF = 0;
        TMR0 = 0x00;
        
        // Increase volume if button was held for over 50ms
        if (count >= 200)
        {
            Volume_Up();
            Set_Volume();
            count = 0;
        }
        
        // Check if button is still pressed
        if (!BUTTON_UP)
        {
            // Start counting with Timer 0
            count = 0;
            TMR0 = 0x00;
            TMR0IF = 0;
            TMR0CS = 0;
            
            // Increase volume every 250ms as long as button is pressed
            while (!BUTTON_UP)
            {
                if (TMR0IF)
                {
                    count++;
                    TMR0IF = 0;
                }
                
                if (count >= 1000)
                {
                    Volume_Up();
                    Set_Volume();
                    count = 0;
                }
            }
            
            // Clear timer
            TMR0CS = 1;
            TMR0IF = 0;
            TMR0 = 0x00;
        }
        
        IOCAF5 = 0;
    }
    
    // If BUTTON_DN has been pressed
    if (IOCAF4)
    {
        // Start counting with Timer 0
        unsigned int count = 0;
        TMR0 = 0x00;
        TMR0IF = 0;
        TMR0CS = 0;
        
        // Hang until button has been held for ~50ms
        while ((!BUTTON_DN) && (count < 200))
        {
            if (TMR0IF)
            {
                count++;
                TMR0IF = 0;
            }
        }
        
        // Clear timer
        TMR0CS = 1;
        TMR0IF = 0;
        TMR0 = 0x00;
        
        // Increase volume if button was held for over 50ms
        if (count >= 200)
        {
            Volume_Down();
            Set_Volume();
            count = 0;
        }
        
        // Check if button is still pressed
        if (!BUTTON_DN)
        {
            // Start counting with Timer 0
            count = 0;
            TMR0 = 0x00;
            TMR0IF = 0;
            TMR0CS = 0;
            
            // Increase volume every 250ms as long as button is pressed
            while (!BUTTON_DN)
            {
                if (TMR0IF)
                {
                    count++;
                    TMR0IF = 0;
                }
                
                if (count >= 1000)
                {
                    Volume_Down();
                    Set_Volume();
                    count = 0;
                }
            }
            
            // Clear timer
            TMR0CS = 1;
            TMR0IF = 0;
            TMR0 = 0x00;
        }
        
        IOCAF4 = 0;
    }
    
}

void main()
{
    PIC_Init();
    
    // Set mode on startup based on the select pin
    mode = MODE_SELECT;
    
    // Disable interrupts if mode = 1 so buttons don't work
    if (mode)
    {
        INTCON = 0x00;
    }
    
    // Initialize I2C Master with 100kHz clock
    I2C_Master_Init(100000);  
    
    // Set initial volume and settings
    LM_Init();
     
    while(1)
    {    
        if (mode)
        {
            // Check potentiometer every 100ms
            Check_ADC();
            __delay_ms(100);
        }
    }
}
