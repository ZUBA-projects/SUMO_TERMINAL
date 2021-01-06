/*
 * device_config.h
 *
 *  Created on: 28 lis 2020
 *      Author: zuba1
 */

#ifndef DEVICE_CONFIG_H_
#define DEVICE_CONFIG_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "nrf_saadc.h"

#define LED_0_R_PIN	18 //led0_r LED_1_PIN
#define LED_0_G_PIN	19 //led0_g LED_2_PIN

#define LED_1_R_PIN	31 //led1_g LED_6_PIN
#define LED_1_G_PIN	2 //led1_r LED_5_PIN

#define LED_2_R_PIN	29 //led2_r LED_4_PIN
#define LED_2_G_PIN	30 //led2_g LED_3_PIN

#define LED_3_R_PIN	27 //led3_r LED_7_PIN
#define LED_3_G_PIN	28 //led3_g LED_8_PIN

#define MAX_LEDS 4

#define SWITCH_RIGHT_PIN	6 //right SWITCH_2_PIN
#define SWITCH_UP_PIN	7 //up SWITCH_1_PIN
#define SWITCH_LEFT_PIN	8 //left SWITCH_4_PIN
#define SWITCH_CENTER_PIN	13 //center SWITCH_6_PIN
#define SWITCH_DOWN_PIN	14 //down SWITCH_3_PIN
#define SWITCH_FUNC_PIN		20 //PWR_BTN_PIN
#define SWITCH_PWR_PIN		17 //PWR_BTN_PIN

#define LED_SDA_PIN	3
#define LED_SCL_PIN	4

#define IR_TX_PIN	15
#define CPU_PWR_PIN	16
#define LED_BAT_LVL_PIN	5



#define POWER_OFF_BUTTON_TIME		3000	//in ms
#define SW_VERSION_MINOR			1
#define SW_VERSION_MAJOR			1

#define ARM_MATH_CM4	1
#define FPU_EXCEPTION_MASK               0x0000009F                      //!< FPU exception mask used to clear exceptions in FPSCR register.

enum {LOW,HIGH};

#define NAN_u32  0xFFFFFFFF
#define NAN_u16	 0xFFFF
#define NAN_u8	 0xFF

//config watchdog
#define SYSTEM_TIMEOUT					1000 //in ms
#define BATTERY_CALIBRATION				0
#define ADC_RES		4096//16383//8192//4096//2048//adc coef
#define AIN0			1 		//ADC0 //hardware pin out settings
#define CPU_VTG_LVL			NRF_SAADC_INPUT_VDD//cpu voltage measurment
#define AKU_LVL				(AIN0+4)

#define USER_TIMEOUT_TIME	300000 //5min

#endif /* DEVICE_CONFIG_H_ */
