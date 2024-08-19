/* 
 * File:   system.h
 * Author: willi
 *
 * Created on 4 de Julho de 2024, 20:40
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#ifdef	__cplusplus
extern "C" {
#endif

//TM1637
#define D_CLK_PIN TRISCbits.TRISC0
#define D_CLK PORTCbits.RC0
#define D_DIO_PIN TRISCbits.TRISC1
#define D_DIO PORTCbits.RC1
    
//DS18B20    
#define DS_PIN TRISCbits.TRISC5
#define DS_DQ PORTCbits.RC5    
    
//buttons
#define BTN_STEP PORTBbits.RB5
#define BTN_INC PORTBbits.RB4
#define BTN_UPDATE PORTBbits.RB2

//LEDs and buzzer
#define LED_TEMPERATURE PORTAbits.RA1
#define LED_ALARM_STATUS PORTAbits.RA2
#define BUZZER PORTAbits.RA3    
    
#define _XTAL_FREQ 16000000

typedef struct up_data{
    uint8_t clock_hour;
    uint8_t clock_min;
    uint8_t alarm_hour;
    uint8_t alarm_min;
    uint8_t alarm_status;
    uint8_t temperature_mode;
    uint8_t display_brightness;
}update_data;

const uint8_t digit2segment[] = {
    0b00111111, //0
    0b00000110, //1
    0b01011011, //2
    0b01001111, //3
    0b01100110, //4
    0b01101101, //5
    0b01111101, //6
    0b00000111, //7
    0b01111111, //8
    0b01101111  //9
};

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_H */