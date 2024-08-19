/* 
 * File:   ds3231.h
 * Author: willi
 *
 * Created on 4 de Julho de 2024, 19:39
 */

#ifndef DS3231_H
#define	DS3231_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "i2c.h"    
#include "system.h"
        
void rtc_set_time(uint8_t, uint8_t, uint8_t);
void rtc_read_time(uint8_t, uint8_t *, update_data *);
void display_format_time(uint8_t, uint8_t, uint8_t *);
void rtc_set_alarm_status(uint8_t);
uint8_t rtc_read_alarm_status(void);
void rtc_clear_alarm_irq(void);
uint8_t bcd2dec(uint8_t);
uint8_t dec2bcd(uint8_t);

#ifdef	__cplusplus
}
#endif

#endif	/* DS3231_H */