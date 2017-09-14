
#include "SocketDimmer.h"

uint8_t light_state = 0;

const char dimmer_range = 250;  //0 (low, off) - 250 (high)
uint8_t dimmer_value = 0;

void dimmerEnable(char enable){
	if (enable){
		ZERO_DETECTOR_ENABLE_INT;
	}else{
		ZERO_DETECTOR_DISABLE_INT;
		DISABLE_TIMER_CMP_A;
		RELAY_0_OFF;
	}
}

char switchExecute(unsigned char relay_id, unsigned char command){
		if (relay_id == RELAY_0_ID){
		switch(command){
			case 0x00:	//����
				light_state = 0;
				break;
			case 0x01: //���
				light_state = 1;
				break;
			case 0x02: //������
				light_state = !light_state;
				break;
			default:
				return 0;
		}
	
		//disable pwm
		dimmerEnable(0);
		
		//set value
		if (light_state){
			RELAY_0_ON;
		}else{
			RELAY_0_OFF;
		}
		
	}
		return 1;
}

void switchResponse(unsigned char address){
	char info = (light_state << (RELAY_0_ID-1));
	clunet_send_fairy(address, CLUNET_PRIORITY_MESSAGE, CLUNET_COMMAND_SWITCH_INFO, &info, sizeof(info));
}

void switch_exec(unsigned char relay_id, unsigned char command, unsigned char address){
	if (switchExecute(relay_id, command)){
		switchResponse(address);
	}
}

char dimmerExecute(unsigned char value) {
	if (value >= 0 && value <= dimmer_range) {
		dimmer_value = value;
		light_state = value > 0;
		dimmerEnable(value > 0);
		return 1;
	}
	return 0;
}

void dimmerResponse(unsigned char address){
	char data[] = {1, RELAY_0_ID, dimmer_value};
	clunet_send_fairy(address, CLUNET_PRIORITY_MESSAGE, CLUNET_COMMAND_DIMMER_INFO, data, sizeof(data));
}

void dimmer_exec(unsigned char value, unsigned char address){
	if (dimmerExecute(value)){
		dimmerResponse(address);
	}
}

void cmd(clunet_msg* m){
	switch(m->command){
		case CLUNET_COMMAND_SWITCH:
			if (m->size == 1 && m->data[0] == 0xFF){	//info request
					switchResponse(m->src_address);
			}else if (m->size == 2){
					switch(m->data[0]){
						case 0x00:
						case 0x01:
						case 0x02:
							switch_exec(m->data[1], m->data[0], m->src_address);
							break;
						case 0x03:
							for (char i=0; i<8; i++){
								switchExecute(i+1, bit(m->data[1], i));
							}
							switchResponse(m->src_address);
							break;
					}
			}
			break;
		case CLUNET_COMMAND_DIMMER:
			if (m->size == 1 && m->data[0] == 0xFF){	//info request
				dimmerResponse(m->src_address);
			}else if (m->size == 2){
				//� ��� ������ ���� �����. ���������, ��� ������� ��� ����
				if ((m->data[0] >> (RELAY_0_ID - 1)) & 0x01) {
					dimmer_exec(m->data[1], m->src_address);
				}
			}
			break;
		case CLUNET_COMMAND_RC_BUTTON_PRESSED:
			//if (m->src_address == CLUNET_DEVICE_ID){
				if (m->data[0] == 0x00){	//nec
					if (m->data[1] == 0x02){
						switch (m->data[2]){
							case 0x12://������� ������
							//dimmer_exec(0, CLUNET_BROADCAST_ADDRESS);
							switch_exec(RELAY_0_ID, 0x00, CLUNET_BROADCAST_ADDRESS);
							break;
							case 0x92://������� ������
							dimmer_exec(dimmer_range * 1/3, CLUNET_BROADCAST_ADDRESS);
							break;
							case 0x52://������ ������
							dimmer_exec(dimmer_range * 2/3, CLUNET_BROADCAST_ADDRESS);
							break;
							case 0xD2://����� ������
							//dimmer_exec(dimmer_range, CLUNET_BROADCAST_ADDRESS);
							switch_exec(RELAY_0_ID, 0x01, CLUNET_BROADCAST_ADDRESS);
							break;
						}
					}
				}
			//}
	}
}

void clunet_data_received(unsigned char src_address, unsigned char dst_address, unsigned char command, char* data, unsigned char size){
	switch(command){
		case CLUNET_COMMAND_SWITCH:
		case CLUNET_COMMAND_DIMMER:
		case CLUNET_COMMAND_RC_BUTTON_PRESSED:
			clunet_buffered_push(src_address, dst_address, command, data, size);
	}
}

volatile uint8_t low_high = 0;	//waiting for rising or falling edge on zero detector
volatile uint8_t tick;

ISR(ZERO_DETECTOR_INT_VECTOR){
	if (ZERO_DETECTOR_READ == low_high){
		if (low_high){
			DISABLE_TIMER_CMP_A;
			RELAY_0_OFF;
		}else{
			ENABLE_TIMER_CMP_A;
		}
		tick = 0;
		low_high = !low_high;
	}
}


ISR(TIMER_COMP_VECTOR){
	TIMER_COUNTER = 0;	//reset counter
	
	tick++;
	if (dimmer_range - dimmer_value < tick){
		RELAY_0_ON;
	}
}

int main(void){
	cli();
	
	RELAY_0_INIT;
	RELAY_0_OFF;
	
	ZERO_DETECTOR_INIT;
	ZERO_DETECTOR_INIT_INT;
	ZERO_DETECTOR_DISABLE_INT;
	
	TIMER_INIT;
	DISABLE_TIMER_CMP_A;
	
	clunet_set_on_data_received(clunet_data_received);
	clunet_init();
	
	while (1){
		if (!clunet_buffered_is_empty()){
			cmd(clunet_buffered_pop());
		}
	}
	return 0;
}