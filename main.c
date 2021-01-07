


#include <ble_app.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "nrf_pwr_mgmt.h"
#include "i2c.h"
#include "oled.h"
#include "ssd1306.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "device_config.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "hardware.h"
#include "app_util.h"
#include "adc.h"

static char text[32]; //tab for displayed chars

void app_ble_config(){

	ssd1306_fill_screen(BLACK);

	if(ble_get_status()==BLE_CONNECTED){

		sprintf(text,"Disconnect?");
		oled_print(text,1,WHITE,1,1,1);

		while(1){

			uint8_t state=get_one_button_state(0);

			if(state==SW_PWR) return; //go back to menu
			if(state==SW_CENTER) break; //disconnect and search new connections
		}

	}

	ble_disonnect();
	//ble_connect();
	ble_scan_start();


	while(1){


		ssd1306_fill_screen(BLACK);

		switch(ble_get_status()){
			case BLE_DISCONNECTED:
				sprintf(text,"Disconnected");
				break;
			case BLE_SCANNING:
				sprintf(text,"Scanning");
				break;
			case BLE_SCANNING_DONE:
				sprintf(text,"DONE");
				break;
			case BLE_CONNECTED:
				sprintf(text,"Connected");
				break;
		}

		oled_print(text,1,WHITE,1,1,1);


		switch(get_one_button_state(0)){

			default:
			case SW_FUNC:
				break;

			case SW_PWR:
				return; //go back to menu
				break;

			case SW_UP:
				break;

			case SW_LEFT:
				break;

			case SW_DOWN:
				break;

			case SW_RIGHT:
				break;

			case SW_CENTER:
				break;
		}

		//app code


		power_manage();
	}
}

void app_ble_treminal(){

	ssd1306_fill_screen(BLACK);

	while(1){

		if(ble_get_status()!=BLE_CONNECTED){
			sprintf(text,"BLE DISCONNECT");
			ssd1306_fill_screen(BLACK);
			oled_print(text,1,WHITE,1,1,1);

			while(1){
				uint8_t state=get_one_button_state(0);

				if(state==SW_PWR) return; //go back to menu
				if(state==SW_CENTER) return; //go back to menu
			}
		}



		switch(get_one_button_state(0)){

			default:
			case SW_FUNC:
				break;

			case SW_PWR:
				return; //go back to menu
				break;

			case SW_UP:
				break;

			case SW_LEFT:
				break;

			case SW_DOWN:
				break;

			case SW_RIGHT:
				break;

			case SW_CENTER:
				break;
		}

		//app code


		power_manage();
	}
}


#define BLE_REQUEST_NORESP_STAMP 300
#define BLE_REQUEST_SLOW_STAMP	100
#define BLE_REQUEST_QUICK_STAMP	50

#define BLE_RESP_TOUT		1500


typedef enum{

	//CMD_CLEAR_DISP_MEM=0x01,
	CMD_FILL=0x02,

	CMD_DRAW_LINE=0x03,
	//CMD_DRAW_BOX=0x04,
	CMD_DRAW_CIRCLE=0x05,

	CMD_DRAW_TEXT=0x06,

	CMD_LEDS_MANAGE=0x08,

}rtcommands_t;


void app_ble_live(){

	ssd1306_fill_screen(BLACK);
	uint32_t _lastresp=getsystemtime()+BLE_RESP_TOUT;
	uint32_t _tasktime=getsystemtime()+BLE_REQUEST_SLOW_STAMP;
	bledata_t _bledata;
	uint8_t buttonstate=0;

	while(1){

		if(ble_get_status()!=BLE_CONNECTED){
			sprintf(text,"BLE DISCONNECT");
			ssd1306_fill_screen(BLACK);
			oled_print(text,1,WHITE,1,1,1);

			while(1){
				uint8_t state=get_one_button_state(0);

				if(state==SW_PWR) return; //go back to menu
				if(state==SW_CENTER) return; //go back to menu
			}
		}

		if(getsystemtime()>_tasktime){
			_tasktime=getsystemtime()+ BLE_REQUEST_NORESP_STAMP;

			buttonstate=get_one_button_state(1);

			if(buttonstate==SW_PWR) 	return; //go back to menu

			 _bledata._data[0]=0xA9; //is a downlink message
			 _bledata._data[1]=buttonstate; //state of button
			 _bledata._data[2]=0x00;
			 _bledata._len=3;

			 ble_send(&_bledata);
		}

		if(ble_is_new_data()){
			_bledata._data[2]=0; //update display flag

			ble_get_data(&_bledata);
			_lastresp=getsystemtime()+BLE_RESP_TOUT;
/*
			NRF_LOG_INFO("ble data len %d data:",_bledata._len);
		    for(uint8_t i=0;i<_bledata._len;i++){
		    	NRF_LOG_RAW_INFO("0x%x",_bledata._data[i]);
		    }
*/



			if(_bledata._data[0]==0xAC){ //graphic pload

				if(_bledata._data[2]>3){
					//is not empty message
					uint8_t i=3;
					uint8_t n=0;

					while(i<_bledata._data[2]){

						n++;
						if(n>16) break; //max 16 effects

						switch(_bledata._data[i]){
							case CMD_FILL:
								ssd1306_fill_screen(_bledata._data[i+1]);
								i+=2;
								break;

							case CMD_DRAW_LINE:
								ssd1306_draw_line(_bledata._data[i+1],_bledata._data[i+2],_bledata._data[i+3],_bledata._data[i+4],_bledata._data[i+5]);
								i+=6;
								break;

							case CMD_DRAW_CIRCLE:
								if(_bledata._data[i+4]){
									ssd1306_fill_circle(_bledata._data[i+1],_bledata._data[i+2],_bledata._data[i+3],_bledata._data[i+5]);
								}else{
									ssd1306_draw_circle(_bledata._data[i+1],_bledata._data[i+2],_bledata._data[i+3],_bledata._data[i+5]);
								}
								i+=6;
								break;


							case CMD_DRAW_TEXT:
								text[0]=0;

								 for(uint8_t z=0; z<_bledata._data[i+5];z++){
										  text[z]=_bledata._data[6+i+z];
										  text[z+1]=0x00;
								 }

								oled_print(text,_bledata._data[i+4],_bledata._data[i+3],_bledata._data[i+1],_bledata._data[i+2],0);

								i+=7+_bledata._data[i+5];

								break;

							case CMD_LEDS_MANAGE:
								if(_bledata._data[i+1]!=0){
									set_led(_bledata._data[i+2],(_bledata._data[i+3]<<8)|_bledata._data[i+4],_bledata._data[i+1]);
								}
								i+=5;
								break;

							default:
								i=255; //finish operation -find an error
								break;

						}
					}
				}

				if(_bledata._data[1]& 0x01) ssd1306_display();
				if(_bledata._data[1]& 0x02) ble_disonnect();
				if(_bledata._data[1]& 0x04) {
					_tasktime=getsystemtime()+BLE_REQUEST_QUICK_STAMP;
				}else{
					_tasktime=getsystemtime()+BLE_REQUEST_SLOW_STAMP;

				}

				//clear meta data
				_bledata._data[0]=0;
				_bledata._data[1]=0;
				_bledata._data[2]=0;

			}

		}

		if(getsystemtime()>_lastresp){
			ssd1306_fill_screen(BLACK);
			sprintf(text,"BLE RQ Tout");
			oled_print(text,1,WHITE,10,10,1);
		}

		power_manage();
	}
}


void app_sys_info(){

	uint32_t lastdisptime=0;

	while(1){

		if((lastdisptime+500)<getsystemtime()){
			lastdisptime=getsystemtime();

			ssd1306_fill_screen(BLACK);
			sprintf(text,"Voltage: %lu[mV]",get_battery_voltage());
			oled_print(text,1,WHITE,1,0,0);

			sprintf(text,"SW version: V%d.%d",SW_VERSION_MINOR,SW_VERSION_MAJOR);
			oled_print(text,1,WHITE,1,8,1);


		}

		switch(get_one_button_state(0)){

			default:
			case SW_FUNC:
				break;

			case SW_PWR:
				return; //go back to menu
				break;

			case SW_UP:
				break;

			case SW_LEFT:
				break;

			case SW_DOWN:
				break;

			case SW_RIGHT:
				break;

			case SW_CENTER:
				break;
		}

		//app code


		power_manage();
	}
}


void app_ir_sumoremote(){

	uint8_t dispupdate=1;
	static uint8_t ringID=16;
	uint8_t idchangeenable=1;
	uint8_t lastwasstop=1;
	sprintf(text,"IDLE");
		while(1){

		switch(get_one_button_state(0)){

			default:
			case SW_FUNC:
				break;

			case SW_PWR:
				return; //go back to menu
				break;

			case SW_UP:
				if(idchangeenable==0){
					sprintf(text,"ID LOCKED");
				}else{
					sprintf(text,"ID++");
					if(ringID<32)ringID++;
				}
				dispupdate=1;
				break;

			case SW_LEFT:
				sprintf(text,"SEND START");
				//send start command
				dispupdate=1;
				lastwasstop=0;
				idchangeenable=0;
				break;

			case SW_DOWN:
				if(idchangeenable==0){
					sprintf(text,"ID LOCKED");
				}else{
					sprintf(text,"ID--");
					if(ringID)ringID--;
				}
				dispupdate=1;
				break;

			case SW_RIGHT:
				sprintf(text,"SEND STOP");
				//send stop command
				dispupdate=1;
				idchangeenable=0;
				lastwasstop=1;
				break;

			case SW_CENTER:
				if(lastwasstop==1){
					sprintf(text,"PROG ID");
					//send porg command
					idchangeenable=1;
				}else{
					sprintf(text,"PROG LOCK");
				}
				dispupdate=1;
				break;
		}

		//app code
		if(dispupdate){
			ssd1306_fill_screen(BLACK);

			oled_print(text,2,WHITE,1,1,1);
			sprintf(text,"NOW ID:%d",ringID);
			oled_print(text,2,WHITE,1,16,1);
			dispupdate=0;
		}


		power_manage();
	}
}

void app_ir_RC5(){

	ssd1306_fill_screen(BLACK);

	uint8_t addr=10;
	uint8_t id=10;
	uint8_t toggle=0;
	uint8_t pos=0;
	uint8_t sendflag=0;
	uint8_t updatedisplay=1;

	while(1){
		switch(get_one_button_state(0)){

			default:
			case SW_FUNC:
				break;

			case SW_PWR:
				return; //go back to menu
				break;

			case SW_UP:
				switch(pos){
					case 0:
						if(id<32) id++;
						break;

					case 1:
						if(addr<32) addr++;
						break;

					case 2:
						toggle=1;
						break;

					default:
						break;
				}
				updatedisplay=1;
				break;

			case SW_LEFT:
				if(pos) pos--;
				updatedisplay=1;
				break;

			case SW_DOWN:
				switch(pos){
					case 0:
						if(id) id--;
						break;

					case 1:
						if(addr) addr--;
						break;

					case 2:
						toggle=0;
						break;

					default:
						break;
				}
				updatedisplay=1;
				break;

			case SW_RIGHT:
				if(pos<2) pos++;
				updatedisplay=1;
				break;

			case SW_CENTER:
				sendflag=1;
				updatedisplay=1;
				//send msg
				break;
		}

		//app code
		if(updatedisplay){
			while(1){
				break;
			}
			ssd1306_fill_screen(BLACK);



			sprintf(text,"ID:");
			oled_print(text,1,WHITE,20,1,0);
			sprintf(text,"%d",id);
			oled_print(text,2,WHITE,20,9,0);

			sprintf(text,"ADR:");
			oled_print(text,1,WHITE,50,1,0);
			sprintf(text,"%d",addr);
			oled_print(text,2,WHITE,50,9,0);

			sprintf(text,"TGL:");
			oled_print(text,1,WHITE,80,1,0);
			sprintf(text,"%d",toggle);
			oled_print(text,2,WHITE,80,9,0);

			if(sendflag){
				sendflag=0;
				sprintf(text,"SEND");
				oled_print(text,1,WHITE,32,24,1);
			}else{
				sprintf(text,"-^-");
					oled_print(text,1,WHITE,26*(pos+1),24,1);
			}


			updatedisplay=0;
		}
		power_manage();
	}
}

void app_ir_config(){

	ssd1306_fill_screen(BLACK);

	sprintf(text,"APP ERROR");
	oled_print(text,2,WHITE,10,8,1);

	while(1){
		switch(get_one_button_state(0)){

			default:
			case SW_FUNC:
				break;

			case SW_PWR:
				return; //go back to menu
				break;

			case SW_UP:
				break;

			case SW_LEFT:
				break;

			case SW_DOWN:
				break;

			case SW_RIGHT:
				break;

			case SW_CENTER:
				break;
		}

		//app code


		power_manage();
	}
}


int main(void)
{
    // Initialize.
	init_hardware();
    init_buttons_and_leds();
    //saadc_init(AKU_LVL);//adc module init
    saadc_init(CPU_VTG_LVL);

    //set_led(LED_YELLOW, LED_PULSE, 0);
    //set_led(LED_GREEN, LED_SLOW, 1);
    //set_led(LED_GREEN, LED_FAST, 2);
    //set_led(LED_YELLOW, LED_ON, 3);

	set_power_mode(1);

	ble_init();
    i2c_Init(LED_SCL_PIN);		//init i2c controller
    i2c_add_device(LED_SDA_PIN);
    oled_init(LED_SDA_PIN,0xFF);

    uint8_t _login=0;

	while(1){
		if(get_one_button_state(1)!=SW_PWR) set_power_mode(0);

		_login++;
		if(_login>8) break;

		sprintf(text,"        ");

		for(uint8_t i=0;i<_login;i++){
			text[i]=252; //square icon
		}

		oled_print(text,2,WHITE,8,20,1);
		nrf_delay_ms(50);
	}

	oled_clear();



	uint8_t menupos=0;
	uint8_t update=1;

    while(1){

    	switch(get_one_button_state(0)){

    		default:
    		case SW_FUNC:
    		case SW_PWR:
    			break;

    		case SW_UP:
    			if(menupos)menupos--;
    			update=1;
    			break;

    		case SW_LEFT:
    			if(menupos>3) menupos-=4;
    			update=1;
        		break;

    		case SW_DOWN:
    			if(menupos<7)menupos++;
    			update=1;
        		break;

    		case SW_RIGHT:
    			if(menupos<4) menupos+=4;
    			update=1;
        		break;

    		case SW_CENTER:
    			switch(menupos){
					case 0:
						app_ble_config();
						break;

					case 1:
						app_ble_treminal();
						break;

					case 2:
						app_ble_live();
						break;

					case 3:
						app_sys_info();
						break;

					case 4:
						app_ir_sumoremote();
						break;

					case 5:
						app_ir_config();
						break;

					case 6:
						app_ir_RC5();
						break;

					case 7:
						set_power_mode(0);
						break;
    			}
    			update=1;
        		break;
    	}



    	if(update){

    		ssd1306_fill_screen(BLACK);

    		sprintf(text,"BLE_CFG");
    		oled_print(text,1,WHITE,1,0,0);

     		sprintf(text,"BLE_TERM");
    		oled_print(text,1,WHITE,1,8,0);

     		sprintf(text,"BLE_LIVE");
     		oled_print(text,1,WHITE,1,16,0);

     		sprintf(text,"SYS_INFO");
     		oled_print(text,1,WHITE,1,24,0);

     		sprintf(text,"IR_SUMO");
     		oled_print(text,1,WHITE,80,0,0);

      		sprintf(text,"IR_CFG");
         	oled_print(text,1,WHITE,80,8,0);

         	sprintf(text,"IR_RC5");
         	oled_print(text,1,WHITE,80,16,0);

         	sprintf(text,"PWR_OFF");
         	oled_print(text,1,WHITE,80,24,0);

     		if(menupos<4){
     			sprintf(text,"<-");
     		    oled_print(text,1,WHITE,49,(menupos*8),1);
     		}else{
     			sprintf(text,"->");
     		    oled_print(text,1,WHITE,65,((menupos-4)*8),1);
     		}

    		update=0;
    	}


    	power_manage();

    }
}
