/* 
 * File:   tm1637.h
 * Author: willi
 *
 * Created on 4 de Julho de 2024, 19:42
 */

#ifndef TM1637_H
#define	TM1637_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include "system.h"
    
void tm1637_start(void);
void tm1637_stop(void);
void tm1637_write_byte(uint8_t);
void update_display(uint8_t *, uint8_t);

#ifdef	__cplusplus
}
#endif

#endif	/* TM1637_H */