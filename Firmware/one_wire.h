/* 
 * File:   one_wire.h
 * Author: willi
 *
 * Created on 4 de Julho de 2024, 19:43
 */

#ifndef ONE_WIRE_H
#define	ONE_WIRE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include "system.h"
    
uint8_t one_wire_start(void);
void one_wire_write_byte(uint8_t);
uint8_t one_wire_read_byte(void);
uint8_t one_wire_read_bit(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ONE_WIRE_H */