#include "tm1637.h"

/* Sends a start condition
 */ 
void tm1637_start(void){
    D_DIO_PIN = 0;
    D_DIO = 0;
    __delay_us(100);
}

/* Sends a stop condition
 */
void tm1637_stop(void){
    D_DIO_PIN = 0;
    D_DIO = 0;
    __delay_us(100);
    D_CLK_PIN = 1;
    __delay_us(100);
    D_DIO_PIN = 1;
    __delay_us(100);
}

/* Writes one byte 
 * @param b: byte value
 */
void tm1637_write_byte(uint8_t b){
    uint8_t data = b;
    uint8_t ack = 0;
    
    for(uint8_t i=0; i<8; i++){
        //clk falling edge
        D_CLK_PIN = 0;
        D_CLK = 0;
        __delay_us(100);
        //data bit
        if(data & 0x01){
            D_DIO_PIN = 1;
        }
        else{
            D_DIO_PIN = 0;
            D_DIO = 0;
        }
        __delay_us(100);
        //clk rising edge
         D_CLK_PIN = 1;
        __delay_us(100);
        data = data >> 1;
    }
    //ACK
    D_CLK_PIN = 0;
    D_CLK = 0;
    D_DIO_PIN = 1;
    __delay_us(100);
    //CLK rising edge
    D_CLK_PIN = 1;
    __delay_us(100);
    ack = D_DIO;
    if(ack==0){
        D_DIO_PIN = 0;
        D_DIO = 0;
    }
    __delay_us(100);
    D_CLK_PIN = 0;
    D_CLK = 0;
    __delay_us(100);    
}

/* Updates display digits 
 * @param d: display digits array
 * @param brightness: brightness value
 */
void update_display(uint8_t *d, uint8_t brightness){
    di();
    tm1637_start();
    tm1637_write_byte(0x40); //write data command
    tm1637_stop();
    
    tm1637_start();
    tm1637_write_byte(0xC0); //display adress (most left digit)
    
    for (uint8_t i=0; i<4; i++){
        tm1637_write_byte(d[i]);
    }
    tm1637_stop();
   
    tm1637_start();
    tm1637_write_byte(0x88 | brightness); //brightness
    tm1637_stop();
    ei();
}