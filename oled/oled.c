/*
 * oled.c
 *
 *  Created on: Oct 10, 2019
 *      Author: zuba1
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "nordic_common.h"
#include "ssd1306.h"
#include "oled.h"
#include "math.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"

void oled_init(uint8_t pin,uint8_t rstpin){

	nrf_gpio_cfg_output(rstpin);

	nrf_gpio_pin_clear(rstpin);
	nrf_delay_ms(10);
	nrf_gpio_pin_set(rstpin);
	nrf_delay_ms(10);


	ssd1306_init_i2c(pin);
	//ssd1306_set_rotation(0);
	ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS, false);
	//ssd1306_display();
}


void oled_print_oponentring(uint8_t x,uint8_t y,float deg,float dist){

	deg-=90;
	deg=(deg/57.29577951) ; //Convert degrees to radians

	dist=dist/12;

	float x2=dist*cos(deg);
	float y2=dist*sin(deg);

	ssd1306_draw_circle(x,y, 21,1);

	ssd1306_draw_line(x, y, x2+x, y2+y,1);
}

void oled_print(char* data,uint8_t size,uint8_t color,uint8_t x,uint8_t y,uint8_t update){


    ssd1306_set_textsize(size);
    ssd1306_set_textcolor(color);
    ssd1306_set_cursor(x, y);

    for(uint16_t i=0; i<strlen(data);i++){
    	  ssd1306_write(data[i]);
    }

    if(update==1)ssd1306_display();
}

void oled_clear(){
	   ssd1306_clear_display();
}

void oled_fill(){
	ssd1306_fill_screen(WHITE);
}
