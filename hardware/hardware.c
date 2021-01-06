/*
 * hardware.c
 *
 *  Created on: 28 lis 2020
 *      Author: zuba1
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "device_config.h"
#include "nrf_drv_wdt.h"
#include "arm_math.h"
#include "app_timer.h"
#include "hardware.h"
#include "nrf_gpio.h"
#include "app_util.h"

APP_TIMER_DEF(led_timer);
APP_TIMER_DEF(systime_timer);
nrf_drv_wdt_channel_id m_channel_id;

uint16_t ledtabcycle[MAX_LEDS];
uint8_t ledtabcollor[MAX_LEDS];
uint8_t ledtabgpio[MAX_LEDS][2];
swmask_t _bstate=0;
volatile uint32_t _systemlocattimems=0;
volatile uint32_t _activitytime=0;
















void wdt_event_handler(void)
{
    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}

void watchdg_allive(){
	nrf_drv_wdt_channel_feed(m_channel_id);
}

void wdt_init(){
	  uint32_t err_code = NRF_SUCCESS;
    //Configure WDT.
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    config.reload_value=SYSTEM_TIMEOUT;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
}


void power_manage(){
    if (NRF_LOG_PROCESS() == false)
      {
          nrf_pwr_mgmt_run();
      }
}


void set_power_mode(uint8_t _mde){

	if(_mde){
		nrf_gpio_cfg_output(CPU_PWR_PIN);
		nrf_gpio_pin_write(CPU_PWR_PIN,1);
		return;
	}

	nrf_gpio_pin_write(CPU_PWR_PIN,0);

	while(1){
		watchdg_allive(); //system is working
		power_manage(); //power manage
	}
}


swmask_t check_buttons(){

	uint8_t _ret=0;
	//if(nrf_gpio_pin_read(SWITCH_FUNC_PIN)==0) _ret+=SW_FUNC;
	if(nrf_gpio_pin_read(SWITCH_UP_PIN)==0) _ret+=SW_UP;
	if(nrf_gpio_pin_read(SWITCH_LEFT_PIN)==0) _ret+=SW_LEFT;
	if(nrf_gpio_pin_read(SWITCH_DOWN_PIN)==0) _ret+=SW_DOWN;
	if(nrf_gpio_pin_read(SWITCH_RIGHT_PIN)==0) _ret+=SW_RIGHT;
	if(nrf_gpio_pin_read(SWITCH_CENTER_PIN)==0) _ret+=SW_CENTER;
	if(nrf_gpio_pin_read(SWITCH_PWR_PIN)==1)  _ret+=SW_PWR;

	return _ret;
}


void init_led(uint8_t _redledgpio, uint8_t _greenledgpio, uint8_t id){
	ledtabgpio[id][0]=_redledgpio;
	ledtabgpio[id][1]=_greenledgpio;
	ledtabcollor[id]=LED_BLACK;
	ledtabcycle[id]=LED_OFF;
	nrf_gpio_cfg_output(_redledgpio);
	nrf_gpio_cfg_output(_greenledgpio);
	nrf_gpio_pin_clear(_redledgpio);
	nrf_gpio_pin_clear(_greenledgpio);


}

void set_led(ledcollor_t _clr, ledmode_t _mde, uint8_t id){
	if(id>=MAX_LEDS) return;
	ledtabcollor[id]=_clr;
	ledtabcycle[id]=_mde;
}

static void systime_task(void * p_context){
	_systemlocattimems+=50;
}

static void led_task(void * p_context){
	static uint8_t _cyclestamp=0;
	static uint32_t _pofswtime=0;
	static uint8_t rebootdevice=0;
	_bstate=check_buttons();


	if(_bstate==0){
		_activitytime+=50; //50ms task stamp
		if(_activitytime>USER_TIMEOUT_TIME) rebootdevice=1;
	}else{
		_activitytime=0;
	}



	if(_bstate & SW_PWR) {
		_pofswtime+=50;
		if(_pofswtime>POWER_OFF_BUTTON_TIME) {
			for(uint8_t i=0;i<MAX_LEDS;i++){
				set_led(LED_YELLOW, LED_ON, i);
			}
			 rebootdevice=1;//set reboot device flag and wait for leds set up
		}
	}else{
		_pofswtime=0;
	}

	//notify watchdog that stystem is working
	watchdg_allive();


	_cyclestamp++;
	if(_cyclestamp>15) _cyclestamp=0;

	for (uint8_t i=0;i<MAX_LEDS; i++){
		if(ledtabcycle[i]&(0x01<<_cyclestamp)){
			nrf_gpio_pin_write(ledtabgpio[i][0],ledtabcollor[i]&0x01);
			nrf_gpio_pin_write(ledtabgpio[i][1],ledtabcollor[i]&0x02);
		}else{
			nrf_gpio_pin_clear(ledtabgpio[i][0]);
			nrf_gpio_pin_clear(ledtabgpio[i][1]);
		}
	}

	if(rebootdevice==1) set_power_mode(0); //power off device

}


void init_buttons_and_leds(){
	nrf_gpio_cfg_input(SWITCH_PWR_PIN, NRF_GPIO_PIN_NOPULL);
	//nrf_gpio_cfg_input(SWITCH_FUNC_PIN,NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(SWITCH_UP_PIN, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(SWITCH_LEFT_PIN, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(SWITCH_DOWN_PIN, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(SWITCH_RIGHT_PIN, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(SWITCH_CENTER_PIN, NRF_GPIO_PIN_NOPULL);

	init_led(LED_0_R_PIN, LED_0_G_PIN,0);
	init_led(LED_1_R_PIN, LED_1_G_PIN,1);
	init_led(LED_2_R_PIN, LED_2_G_PIN,2);
	init_led(LED_3_R_PIN, LED_3_G_PIN,3);

	app_timer_create(&led_timer,APP_TIMER_MODE_REPEATED, led_task);
	app_timer_start(led_timer, APP_TIMER_TICKS(50), NULL);
}


void init_hardware(){
	    ret_code_t  err_code;

	err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

	app_timer_create(&systime_timer,APP_TIMER_MODE_REPEATED, systime_task);
	app_timer_start(systime_timer, APP_TIMER_TICKS(10), NULL);

	wdt_init();
}

swmask_t get_buttonstate(uint8_t continous){

	static swmask_t _lastbstate=0;

	if(continous) {
		_lastbstate=_bstate;
		return _lastbstate;
	}

	if(_bstate!=_lastbstate){
		_lastbstate=_bstate;
		return _lastbstate;
	}else{
		return 0;
	}
}

swmask_t get_one_button_state(uint8_t continous){
	swmask_t mask=get_buttonstate(continous);
	for(uint8_t i=0;i<8;i++){
		if(mask&(1<<i)) return mask & (1<<i);
	}
	return 0;
}

uint32_t getsystemtime(){
	return _systemlocattimems;
}


void clean_acitvity(){
	_activitytime=0;
}


