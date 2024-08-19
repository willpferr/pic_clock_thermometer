#include "ds18b20.h"

/* Sends a convert temperature command
 */
void ds18b20_start_conv(void){
    di();
    one_wire_start();
    __delay_us(380);
    one_wire_write_byte(0xCC); //skip rom
    one_wire_write_byte(0x44); //convert temperature
    ei();
    /* verify ending of conversion
    do{
        __delay_us(1000);
    }while(!one_wire_read_bit());
    */
}

/* Reads raw temperature and updates display digits
 * @param *d: display digits array
 * @param mode: temperature mode selection (1: integer; 0: float)
 */
void ds18b20_read_temperature(uint8_t *d, uint8_t mode){
    uint8_t data[2], temp_int, digit;
    int16_t t_16;
    float temp;
 
    di();
    one_wire_start();
    ei();
    __delay_us(380);
    di();
    one_wire_write_byte(0xCC);
    one_wire_write_byte(0xBE); //read raw temperature registers
    for(uint8_t i=0; i<2; i++)
        data[i] = one_wire_read_byte();
    one_wire_start();
    ei(); 
       
    t_16 = (int16_t)((data[1] << 8) | data[0]);
    temp = (float)(t_16/16.0); 
    LED_TEMPERATURE = temp<0? 1 : 0; //LED to indicate negative temperature
            
    //integer
    temp_int = (uint8_t)(abs((int)(temp)));
    digit = (uint8_t)(temp_int/10);
    d[0] = digit2segment[digit];
    digit *= 10;
    digit = temp_int-digit;
    d[1] = digit2segment[digit];

    if(mode){ 
        d[2] = 0x63; //degree symbol
        d[3] = 0x39; //C
    }
    else{
        temp_int = (uint8_t)(abs((int)(temp*10)));
        digit = temp_int - ((uint8_t)(abs((int)(temp))))*10;
        d[1] ^= 1<<7; //point
        d[2] = digit2segment[digit];
        d[3] = 0x39; //C
    }
}       