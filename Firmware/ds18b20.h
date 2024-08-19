/* 
 * File:   ds18b20.h
 * Author: willi
 *
 * Created on 4 de Julho de 2024, 19:38
 */

#ifndef DS18B20_H
#define	DS18B20_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "one_wire.h"
#include "i2c.h"    
    
void ds18b20_read_temperature(uint8_t *, uint8_t);
void ds18b20_start_conv(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DS18B20_H */

#include <stdint.h>