#include "i2c.h"

/* Configures I2C hardware
 */
void i2c_init(void){
    SSPCON = 0x28; //master mode
    SSPCON2 = 0x00; //master idle state
    SSPSTAT = 0x80; //slew rate disabled for standard speed mode (100 MHz)
    SSPADD = 0x26; // XTAL = 16MHz, I2C = 100 KBPS  //((_XTAL_FREQ/4)/i2c_baudrate) - 1;
    TRISCbits.TRISC3 = 1; //SCL as input pin 
    TRISCbits.TRISC4 = 1; //SDA as input pin
}

/* Waits until I2C be ready
 */
void i2c_wait(void){
    while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F)); //waits until all masked bits are 0
}

/* Performs a start condition 
 */
void i2c_start(void){
    i2c_wait();
    SEN = 1; //initiate a start condition
}

/* Performs a stop condition 
 */
void i2c_stop(void){
    i2c_wait();
    PEN = 1; //initiate a stop condition
}

/* Performs a repeated start condition 
 */
void i2c_repeated_start(void){
    i2c_wait();
    RSEN = 1; //initiate a repeated start condition
}

/* Performs a ACK sequence 
 */
void i2c_ack(void){
    i2c_wait();
    ACKDT = 0; //0: ACK
    ACKEN = 1; //initiate an ACK sequence
}

/* Performs a NACK sequence 
 */
void i2c_nack(void){
    i2c_wait();
    ACKDT = 1; //1: NACK
    ACKEN = 1; //initiate an ACK sequence
}

/* Writes one byte 
 *  @param byte: value to be writted
 */
uint8_t i2c_write(uint8_t byte){
    i2c_wait();
    SSPBUF = byte;
    i2c_wait();
    return ACKSTAT; //ACK status
}

/* Reads one byte 
 *  @return byte: value readed
 */
uint8_t i2c_read(void){
    uint8_t t;

    i2c_wait();
    RCEN = 1;
    i2c_wait();
    t = SSPBUF;
    return t;
}