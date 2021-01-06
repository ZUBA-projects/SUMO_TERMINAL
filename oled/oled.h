/*
 * oled.h
 *
 *  Created on: Oct 10, 2019
 *      Author: zuba1
 */

#ifndef OLED_OLED_H_
#define OLED_OLED_H_


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "ssd1306.h"


void oled_init(uint8_t pin,uint8_t rstpin);
void oled_print_oponentring(uint8_t x,uint8_t y,float deg,float dist);
void oled_print(char* data,uint8_t size,uint8_t color,uint8_t x,uint8_t y,uint8_t update);
void oled_clear();
void oled_fill();

#endif /* OLED_OLED_H_ */
