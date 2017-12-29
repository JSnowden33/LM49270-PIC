# LM49270-PIC
This is a program allowing for digital or analog volume control on the LM49270 headphone/speaker amplifier using a PIC16F1503 microcontroller.
It is in the form of an MPLAB X project and can be compiled using MPLAB X IDE with XC8. You can also use the pre-compiled program found at
Software/LM49270-PIC.X/dist/default/production/LM49270-PIC.X.production.hex with MPLAB IPE. The board can be programmed using a PICkit 3.

The program allows for 32 steps of volume. Speaker volume is allowed the full range from 0 to 31. Headphone volume is restricted from 0 to 21. This can be changed by modifying HP_OFFSET in Config.h.

The operating mode is selected by soldering a jumper located behind the headphone jack on the PCB. When soldered to GND, it operates in digital mode and volume is controlled with two tact switches connected between UP/DN and GND. When the jumper is soldered to V+, it operates in analog mode and volume is controlled with a potentiometer soldered to V+, VOL, and GND.

The PCB for this circuit can be ordered here: https://oshpark.com/shared_projects/Wy4lnqzu  
The circuit requires 3.3V to 5V input. It uses a headphone jack with a NC switch. Speaker output is automatically toggled off by hardware when headphones are detected. The amp works best with speakers between 0.7W and 1.2W.
