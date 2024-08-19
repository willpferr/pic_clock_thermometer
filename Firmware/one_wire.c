#include "one_wire.h"

/* Performs a start condition (reset and presence detect)
 * @return: one wire device status 
 */
uint8_t one_wire_start(void){
    uint8_t status;
    DS_PIN = 0; //output
    DS_DQ = 0;
    __delay_us(480);
    
    DS_PIN = 1;
    __delay_us(80);
    
    if(DS_DQ == 0)
        status = 0;
    else
        status = 1;
    return status;
}

/* Writes one byte
 * #param data: value to be written
 */
void one_wire_write_byte(uint8_t data){
    for(uint8_t i=0x01; i; i <<= 1){
        if(data & i){ //write 1
            DS_PIN = 0; //output -> low level
            DS_DQ = 0;
            __delay_us(10);
            DS_PIN = 1; //input: resistor pullup
            __delay_us(55);
        }
        else{ //write 0
            DS_PIN = 0; //output -> low level
            DS_DQ = 0;
            __delay_us(65);
            DS_PIN = 1; //input: resistor pullup
            __delay_us(1);
        }
    }
}

/* Reads one byte 
 * @return: value read
 */
uint8_t one_wire_read_byte(void){
    uint8_t b = 0x00;
    for(uint8_t i=0x01; i; i <<= 1){
        if(one_wire_read_bit())
            b |= i;
    }
    return b;
}

/* Reads one bit 
 * @return: value read
 */
uint8_t one_wire_read_bit(void){
    uint8_t b;
    DS_PIN = 0; //low
    __delay_us(1);
    DS_PIN = 1;
    __delay_us(15);
    b = DS_DQ; 
    __delay_us(45);
    return b;
}