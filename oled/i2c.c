/*
 * i2c-soft.c
 *
 *  Created on: 19.12.2009
 *      Author: Pavel V. Gololobov
 */
#if defined (NRF51)
		#include "nrf51.h"
#elif defined (NRF52)
		#include "nrf52.h"
#endif

#include "nrf_gpio.h"
#include "i2c.h"

uint8_t _scl=0;
uint8_t _sda=0;
//------------------------------------------------------------------
// I2C Speed Down
//------------------------------------------------------------------
#define I2CWAIT i2c_Wait()

//------------------------------------------------------------------
// I2C Delay
//------------------------------------------------------------------
void i2c_Wait()
{
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
}

//------------------------------------------------------------------
// I2C SDA SCL Control
//------------------------------------------------------------------
void SetLow(uint8_t pin)
{
	nrf_gpio_pin_clear(pin);
	I2CWAIT;
}
void SetHigh(uint8_t pin)
{
	nrf_gpio_pin_set(pin);
	I2CWAIT;
}

//------------------------------------------------------------------
// I2C Initialize Bus
//------------------------------------------------------------------
void i2c_Init(uint8_t n_scl)
{
	_scl=n_scl;
	nrf_gpio_pin_clear(n_scl);
	nrf_gpio_cfg_output(n_scl);
	SetHigh(n_scl);
}

void i2c_add_device(uint8_t n_sda)
{
	nrf_gpio_pin_clear(n_sda);

    nrf_gpio_cfg(
    		n_sda,
			NRF_GPIO_PIN_DIR_OUTPUT,//NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_CONNECT,
		NRF_GPIO_PIN_NOPULL,
		NRF_GPIO_PIN_S0D1,
        NRF_GPIO_PIN_NOSENSE);

	SetHigh(n_sda);
}

//------------------------------------------------------------------
// I2C Start Data Transfer
//------------------------------------------------------------------
void i2c_Start(uint8_t n_sda)
{
	_sda=n_sda;

	SetHigh(_scl);
	SetHigh(_sda);

	SetHigh(_scl);
	SetLow(_sda);

	SetLow(_scl);
	SetHigh(_sda);
}

//------------------------------------------------------------------
// I2C Stop  Transfer
//------------------------------------------------------------------
void i2c_Stop()
{
	SetLow(_scl);
	SetLow(_sda);

	SetHigh(_scl);
	SetLow(_sda);

	SetHigh(_scl);
	SetHigh(_sda);
}

//------------------------------------------------------------------
// I2C Write  Transfer
//------------------------------------------------------------------
int16_t i2c_Write(uint8_t a)
{
	int16_t i;
	int16_t return_ack;

	for (i = 0; i < 8; i++)
    {
		SetLow(_scl);
		if (a & 0x80)
			SetHigh(_sda);
		else
			SetLow(_sda);

		SetHigh(_scl);
		a <<= 1;
	}
	SetLow(_scl);

	/* ack Read */
	SetHigh(_sda);
	SetHigh(_scl);

	if ((uint8_t)(nrf_gpio_pin_read(_sda)))
		return_ack = NO_I2C_ACK;
	else
		return_ack = OK_I2C_ACK;

    SetLow(_scl);

	return (return_ack);
}

//------------------------------------------------------------------
// I2C Read  Transfer
//------------------------------------------------------------------
uint8_t i2c_Read(int16_t ack)
{
	int16_t i;
	uint8_t caracter = 0x00;

	SetLow(_scl);
	SetHigh(_sda);

	for (i = 0; i < 8; i++)
    {
		caracter = caracter << 1;
		SetHigh(_scl);
		if ((uint8_t)(nrf_gpio_pin_read(_sda)))
			caracter = caracter  + 1;

		SetLow(_scl);
	}

	if (ack)
    {
		SetLow(_sda);
	}
	SetHigh(_scl);
	SetLow(_scl);

	//SetHigh(_sda);

	return (caracter);
}
