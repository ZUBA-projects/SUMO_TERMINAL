/*
 * adc.h
 *
 *  Created on: Feb 22, 2019
 *      Author: zuba1
 */

#ifndef HARDWARE_HAL_ADC_H_
#define HARDWARE_HAL_ADC_H_


#define SAADC_CALIBRATION_INTERVAL 		5              //Determines how often the SAADC should be calibrated relative to NRF_DRV_SAADC_EVT_DONE event. E.g. value 5 will make the SAADC calibrate every fifth time the NRF_DRV_SAADC_EVT_DONE is received.
#define SAADC_SAMPLES_IN_BUFFER 		1                //Number of SAADC samples in RAM before returning a SAADC event. For low power SAADC set this constant to 1. Otherwise the EasyDMA will be enabled for an extended time which consumes high current.
#define SAADC_OVERSAMPLE 				NRF_SAADC_OVERSAMPLE_DISABLED//NRF_SAADC_OVERSAMPLE_16X  //Oversampling setting for the SAADC. Setting oversample to 4x This will make the SAADC output a single averaged value when the SAMPLE task is triggered 4 times. Enable BURST mode to make the SAADC sample 4 times when triggering SAMPLE task once.
#define SAADC_BURST_MODE 				0//1                        //Set to 1 to enable BURST mode, otherwise set to 0.

typedef void (*call_backadc) (uint8_t,uint32_t);

void saadc_init(uint8_t chanelv);
uint32_t get_battery_voltage();

#endif /* HARDWARE_HAL_ADC_H_ */
