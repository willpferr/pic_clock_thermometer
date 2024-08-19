#include "ds3231.h"

/* Sets RTC hour and minute values 
 * @param addr: RTC minute address (works with time minute and alarm minute)
 * @param h: decimal hour value
 * @param m: decimal minute value
 */ 
void rtc_set_time(uint8_t addr, uint8_t h, uint8_t m){
	uint8_t hour, minute;
    
    minute = dec2bcd(m);
	hour = dec2bcd(h) & 0b10111111;

    i2c_start();
    i2c_write(0xD0); //RTC write address
    i2c_write(addr); //clock or alarm 1 seconds address
    i2c_write(0x00); //seconds
    i2c_write(minute);
    i2c_write(hour);
    if(addr == 0x07)
        i2c_write(0x81); //alarm on match hour and minutes
    i2c_stop();
}

/* Reads RTC hour and minute values and updates display digits
 * @param addr: RTC minute address (works with time minute and alarm minute) 
 * @param d: display digits array
 * @up: struct of update values  
 */
void rtc_read_time(uint8_t addr, uint8_t *d, update_data *up){
    static uint8_t point = 0;
    uint8_t minute, hour;
    
    i2c_start();
    i2c_write(0xD0); //RTC write address
    i2c_write(addr); //RTC minutes address
    i2c_repeated_start();
    i2c_write(0xD1); //RTC read address
    minute = i2c_read();
    i2c_ack();
    hour = i2c_read();
    i2c_nack();
    i2c_stop();
    //alarm read
    if(addr == 0x08){
        up->alarm_hour =  bcd2dec(hour);
        up->alarm_min =  bcd2dec(minute);
        display_format_time(up->alarm_hour, up->alarm_min, d);
    }
    //clock read
    else{
        up->clock_hour =  bcd2dec(hour);
        up->clock_min =  bcd2dec(minute);
        display_format_time(up->clock_hour, up->clock_min, d);
        //blink point
        if(point)
            d[1] ^= 1<<7;
        point = !point;
    }
}

/* Formats display digts to show hour and minutes
 * @param hour: decimal hour value
 * @param minute: decimal minute value
 * @param d: display digits array
 */
void display_format_time(uint8_t hour, uint8_t minute, uint8_t *d){
    uint8_t aux;
    
    aux = (uint8_t)(hour/10);
    d[0] = (aux == 0) ? 0x00 : digit2segment[aux];
    aux *= 10;
    d[1] = digit2segment[hour-aux];

    aux = (uint8_t)(minute/10);
    d[2] = digit2segment[aux]; 
    aux *= 10;
    d[3] = digit2segment[minute-aux];
}

/* Set RTC alarm status
 * @param status: alarm status (0: disabled; 1: enabled) 
 */
void rtc_set_alarm_status(uint8_t status){
    uint8_t aux = 0x04;
    
    if(status == 1)
        aux |= (1<<0);
    else 
        aux &= ~(1<<0);
    
    i2c_start();
    i2c_write(0xD0); //slave address write
    i2c_write(0x0E); //minute address
    i2c_write(aux);
    i2c_stop(); 
}

/* Reads RTC alarm status
 *  @return: alarm status (0: alarm disabled; 1: alarm enabled)
 */
uint8_t rtc_read_alarm_status(void){
    uint8_t byte;
    
    i2c_start();
    i2c_write(0xD0); //RTC write address
    i2c_write(0x0E); //RTC CONTROL address
    i2c_repeated_start();
    i2c_write(0xD1); //RTC read address
    byte = i2c_read();
    i2c_nack();
    i2c_stop();
    return (byte & (1<<0)) ? 1 : 0;
}

/* Clears RTC alarm interrupt flag
 */
void rtc_clear_alarm_irq(void){
    i2c_start();
    i2c_write(0xD0); //RTC address write
    i2c_write(0x0F); //RTC STATUS address
    i2c_write(0x00); //clear interrupt flag
    i2c_stop(); 
}

/* BCD to decimal conversion
 * @param val: BCD value to be converted
 * @return: decimal converted value
 */
uint8_t bcd2dec(uint8_t val){
	return(uint8_t)((uint8_t)(val/16*10) + (uint8_t)(val%16));
}

/* Decimal to BCD conversion
 * @param val: decimal value to be converted
 * @return: BCD converted value 
 */
uint8_t dec2bcd(uint8_t val){
	return(uint8_t)((uint8_t)(val/10*16) + (uint8_t)(val%10));	
}