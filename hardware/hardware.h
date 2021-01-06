/*
 * hardware.h
 *
 *  Created on: 28 lis 2020
 *      Author: zuba1
 */

#ifndef HARDWARE_HARDWARE_H_
#define HARDWARE_HARDWARE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


typedef enum{
	LED_BLACK=0x00,
	LED_RED=0x01,
	LED_GREEN=0x02,
	LED_YELLOW=0x03,
}ledcollor_t;


typedef enum{
	LED_OFF=0x0000,
	LED_PULSE=0x0001,
	LED_SLOW=0x00FF,
	LED_FAST=0xAAAA,
	LED_ON=0xFFFF,
}ledmode_t;

typedef enum{
	SW_UP=0x01,
	SW_LEFT=0x02,
	SW_DOWN=0x04,
	SW_RIGHT=0x08,
	SW_CENTER=0x10,
	SW_FUNC=0x20,
	SW_PWR=0x40,
}swmask_t;


void set_power_mode(uint8_t _mde);
void set_led(ledcollor_t _clr, ledmode_t _mde, uint8_t id);
void init_buttons_and_leds();
void power_manage();
void init_hardware();
uint32_t get_battery_voltage();
uint32_t getsystemtime();
swmask_t get_button_state(uint8_t continous); //return all mask
swmask_t get_one_button_state(uint8_t continous); //return most important mask

#endif /* HARDWARE_HARDWARE_H_ */
