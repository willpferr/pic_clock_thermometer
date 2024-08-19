// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "system.h"
#include "i2c.h"
#include "one_wire.h"
#include "ds18b20.h"
#include "ds3231.h"
#include "tm1637.h"

//global variables
volatile uint8_t update_step = 0; //indicates the value being updated
volatile uint8_t updating = 0;  //updating on progress
volatile uint8_t update_tick = 0; //time interval to update the display digits
volatile uint8_t alarm_flag = 0; //indicates an alarm interrupt (turn on buzzer)
volatile long alarm_countdown = 32000; //buzzer on for about 30 seconts after alarm interrupt 
volatile uint8_t buzzer_on = 0; //indicates when zuzzer is enabled


void __interrupt() tcInt(void){
    static uint16_t counter = 0; //update_tick counter
    static uint16_t c = 0; //buzzer tone counter
    static uint8_t v = 90; //buzzer tone

    //alarm interrupt
    if(INTF){
        INTF = 0;
        alarm_flag = 1; //alarm IRQ enables buzzer
    }

    //TIMER2: update display
    if(TMR2IF && TMR2ON){
        //buzzer tone
        c = c<v*10 ? c+1 : 0;
        if(buzzer_on == 1){
            //buzzer tone
            if(c < v) BUZZER = ~BUZZER;
            else if(c < v*2){}
            else if(c < v*3) BUZZER = ~BUZZER;
            else if(c < v*4){}
            else if(c < v*5) BUZZER = ~BUZZER;
            else if(c < v*6){}
            else if(c < v*7) BUZZER = ~BUZZER;
               
            //buzzer timeout
            if(alarm_countdown < 0){
                alarm_countdown = 32000;
                buzzer_on = 0;
            }
            else{
                alarm_countdown--;
            }
        }
        
        //update_tick processing
        TMR2IF = 0;
        if(updating == 1){
            if(counter<90){
                counter++;
            }
            else{
                update_tick = 1;
                counter = 0;
            }
        }
        //running
        else{
            //~0.5 Hz to blink ':' 
            if(counter < 477){
                counter++;
            }
            else{
                update_tick = 1;
                counter = 0;
            }   
        }     
    }
}

void main(void){
    update_data up;
    uint8_t alternate = 0;
    uint8_t btn_inc_pressed = 0;
    uint8_t display[] = {0x00, 0x00, 0x00, 0x00}; //display digits array
    uint8_t therm_counter = 0; //temperature read interval time
    uint8_t mode = 0; //0: clock; 1: thermometer; 2: clock / thermmometer
    
    //TM1637
    D_CLK_PIN = 1;
    D_DIO_PIN = 1;
    D_CLK = 0;
    D_DIO = 0;
    
    //PORTA (LEDS)
    ADCON1 = 0x06; //ANx as digital output 
    ADCON0 = 0x00; //AD converter shut-off
    TRISA = 0x00; //digital output
    PORTA = 0x00;
    
    //DS18B20
    DS_PIN = 1; //input
    DS_DQ = 0;

    //button
    TRISB = 0xFF;
    nRBPU = 0; //enable internal PORTB pull-up resistors
    i2c_init();
    
    //TIMER2: display timing update
    PR2 = 0xFA; //250 for 1 ms interrupt
    T2CON = 0x07; //TIMER2 on, prescaler 1:16
    TMR2IE = 1; //enable interrupt
    PEIE = 1; //enable all unmasked peripheral interrupts
    
    //alarm interrupt
    INTEDG = 0; //RB0 falling edge interrupt detection
    INTE = 1; //enable RB0 external interrupt
    
    //if alarm occurred while system was off
    rtc_clear_alarm_irq(); 
    buzzer_on = 0;
    BUZZER = 0;
    alarm_flag = 0;
    
    //initial configuration
    up.temperature_mode = 0;
    up.display_brightness = 0x01;
    LED_ALARM_STATUS = rtc_read_alarm_status();
    
    ei();
    
    while(1){
        //apply selected update
        if(!BTN_UPDATE){
            while(!BTN_UPDATE);
            //disable buzzer
            if(buzzer_on){
                buzzer_on = 0;
                alarm_countdown = 320000;
                BUZZER = 0;
            }
            else if(updating == 1){
                //set clock
                if(update_step == 1 || update_step == 2){
                    rtc_set_time(0x00, up.clock_hour, up.clock_min); //clock seconds address
                }
                //set alarm
                else if(update_step == 4 || update_step == 5){
                    rtc_set_time(0x07, up.alarm_hour, up.alarm_min); //alarm 1 seconds address
                }
                //set alarm status
                else if(update_step == 3){
                    rtc_set_alarm_status(up.alarm_status);  
                    LED_ALARM_STATUS = rtc_read_alarm_status();
                }
                update_step = 0;
                updating = 0;
            }
             //run mode selection
            else
            {
                therm_counter = 0;
                mode = (mode < 2) ? mode+1 : 0;
            }
        }
        //update step
        if(!BTN_STEP){
            while(!BTN_STEP);
            if(!update_step){
                rtc_read_time(0x08, display, &up); //alarm 1 minutes addr to update values stored in up
                up.alarm_status = rtc_read_alarm_status();
            }
            updating = 1;
            update_step = (update_step < 7) ? update_step+1 : 1;
        }
        
        //update display
        if(update_tick){
            //increment selected field
            if(!BTN_INC){
                btn_inc_pressed = 1;
                switch(update_step){
                    case 1:
                        up.clock_hour = up.clock_hour<23 ? up.clock_hour+1 : 0;
                        break;
                    case 2:
                        up.clock_min = up.clock_min<59 ? up.clock_min+1 : 0; 
                        break;
                    case 3:
                        if(!up.alarm_status)
                            up.alarm_status = 1; //On
                        else
                            up.alarm_status = 0; //OFF
                        break;
                    case 4:
                        up.alarm_hour = up.alarm_hour<23 ? up.alarm_hour+1 : 0; 
                        break;
                    case 5:
                        up.alarm_min = up.alarm_min<59 ? up.alarm_min+1 : 0; 
                        break;
                    case 6:
                        if(!up.temperature_mode)
                            up.temperature_mode = 1;
                        else
                            up.temperature_mode = 0;
                        break;
                    case 7:
                        up.display_brightness = up.display_brightness<7 ? up.display_brightness+1 : 0;
                        break;
                    default: 
                        display[0] = display[1] = display[2] = 0x00;
                        display[3] = 0b01010100;//n
                        break;
                }
            }
            
            if(updating){
                switch(update_step){
                    case 1: //clock
                        display_format_time(up.clock_hour, up.clock_min, display);
                        break;
                    case 2: //clock
                        display_format_time(up.clock_hour, up.clock_min, display);
                        break;
                    case 3: //alarm 1 on/off
                         display[0] = 0b01110111; //A
                        if(up.alarm_status){
                            display[1] = 0x00;
                            display[2] = digit2segment[0];
                            display[3] = 0b01010100;//n
                        }
                        else{
                            display[1] = digit2segment[0];
                            display[2] =  display[3] = 0b01110001; //F
                        }
                        break;
                    case 4: //alarm
                        display_format_time(up.alarm_hour, up.alarm_min, display);
                        break;
                    case 5: //alarm
                        display_format_time(up.alarm_hour, up.alarm_min, display);
                        break;
                    case 6:
                        display[0] = 0b01111000; //t
                        display[1] = display[2] = 0x00;
                        if(up.temperature_mode) //integer
                            display[3] = 0b00110000; 
                        else //float
                            display[3] = 0b01110001; //F
                        break;
                    case 7:
                        display[0] = 0b01111100; //b
                        display[1] = display[2] = 0x00;
                        display[3] = digit2segment[up.display_brightness];
                        break;
                }   
                //blink data being updated
                if(!btn_inc_pressed){
                    if(update_step == 1 || update_step == 4){ //hour
                        display[0] = alternate? 0x00 : display[0];
                        display[1] = alternate? 0x00 : display[1];
                        alternate = ~alternate;
                    }
                    else if(update_step == 2 || update_step == 5){ //minute
                        display[2] = alternate? 0x00 : display[2];
                        display[3] = alternate? 0x00 : display[3];
                        alternate = ~alternate;
                    }
                    else if(update_step == 3 || update_step == 6 || update_step == 7){
                        display[0] = alternate? 0x00 : display[0];
                        display[1] = alternate? 0x00 : display[1];
                        display[2] = alternate? 0x00 : display[2];
                        display[3] = alternate? 0x00 : display[3];
                        alternate = ~alternate;
                    }
                    else{
                        display[0] =  display[1] =  display[2] =  display[2] = 0x00;
                    }
                }
            }
            else{
                if(buzzer_on){
                    display[0] = display[2] = 0b01110111; //A
                    display[1] = 0b00111000; //L
                    display[3] = 0b01010000; //r
                }
                else{
                    //running mode
                    switch(mode){
                        case 0: //clock
                            rtc_read_time(0x01, display, &up); //clock minutes addr
                            break;
                        case 1: //thermometer
                            if(therm_counter == 0)
                                ds18b20_start_conv();
                            if(therm_counter == 5)
                                ds18b20_read_temperature(display, up.temperature_mode);
                            therm_counter = therm_counter<5 ? therm_counter+1: 0;
                            break;
                        case 2: //clock / thermomether
                            if(therm_counter == 0)
                                ds18b20_start_conv();
                            if(therm_counter < 20)
                                rtc_read_time(0x01, display, &up); //clock minutes addr
                            else 
                                ds18b20_read_temperature(display, up.temperature_mode);
                            therm_counter = therm_counter<24 ? therm_counter+1: 0;
                            break;
                        default:
                             display[0] =  display[1] =  display[2] =  display[2] = 0x00;
                    }
                }
            }
            btn_inc_pressed = 0;
            update_display(display, up.display_brightness);
            update_tick = 0;
        }
  
        //enable buzzer on alarm interrupt 
        if(alarm_flag){ 
            rtc_clear_alarm_irq();
            alarm_countdown = 32000;
            alarm_flag = 0;
            buzzer_on = 1;
        }
    }
}