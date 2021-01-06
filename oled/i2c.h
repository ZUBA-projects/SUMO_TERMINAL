/*
 * i2c-soft.h
 *
 *  Created on: 19.12.2009
 *      Author: Pavel V. Gololobov
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stdbool.h>
//#include "twi_master_config.h"

#define NO_I2C_ACK 0
#define OK_I2C_ACK 1

#define ACK 1
#define NACK 1


// Init Bus
void i2c_Init(uint8_t n_scl);
//ad new sda line to controller
void i2c_add_device(uint8_t n_sda);

// Start Transfer
void i2c_Start(uint8_t n_sda);
// Stop Transfer
void i2c_Stop(void);
// Write Transfer
int16_t i2c_Write(uint8_t a);
// Read Transfer
uint8_t i2c_Read(int16_t ack);
#endif
