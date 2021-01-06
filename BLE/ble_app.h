/*
 * ble.h
 *
 *  Created on: 28 lis 2020
 *      Author: zuba1
 */

#ifndef BLE_H_
#define BLE_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BLE_MAX_DATA	22


typedef struct{
	uint8_t _data[BLE_MAX_DATA];
	uint8_t _len;
}bledata_t;



typedef enum{
	BLE_DISCONNECTED,
	BLE_SCANNING,
	BLE_SCANNING_DONE,
	BLE_CONNECTED,
}blemodes_t;

void ble_disonnect();
void ble_connect(uint8_t id);
blemodes_t ble_get_status();
void ble_scan_start(void);
uint8_t ble_send(bledata_t*_bledata);
uint8_t ble_is_new_data();
void ble_get_data(bledata_t*_bledata);

void ble_init(void);

#endif /*BLE_H_ */
