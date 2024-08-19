/* 
 * File:   i2c.h
 * Author: willi
 *
 * Created on 4 de Julho de 2024, 19:13
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>

void i2c_init(void);
void i2c_wait(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_repeated_start(void);
void i2c_ack(void);
void i2c_nack(void);
uint8_t i2c_write(uint8_t);
uint8_t i2c_read(void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */