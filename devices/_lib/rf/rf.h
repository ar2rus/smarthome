
#include "utils/bits.h"
#include "rf_config.h"



#include <avr/io.h>

//protocol delays (mks)
#define RF_START_BIT1_DELAY 3000
//#define RF_START_BIT1_DELAY 1000
//#define RF_START_BIT0_DELAY 1000
#define RF_START_BIT0_DELAY 100

#define RF_BIT0_DELAY 100
#define RF_BIT1_DELAY 150
#define RF_HI_DELAY 75

//transmitter
#define RF_TX_HI set_bit(OUTPORT(RF_PORT), RF_PIN)
#define RF_TX_LO unset_bit(OUTPORT(RF_PORT), RF_PIN)
#define RF_TX_INIT {set_bit(DDRPORT(RF_PORT), RF_PIN); RF_TX_LO;}

void rf_send_byte(char data);
void rf_send_message(unsigned char device_id, char* data, unsigned char num_repeats);



//PROTOCOL DEFINITION

// | preambule | device id | message id |    data   |  CRC   |
// |  6 bits	  2 bits   |   1 byte	|  n bytes  | 1 byte |

//static preambule (0b10101100)
#define RF_MESSAGE_PREAMBULE_HEADER 172


//device descriptors:
//2 bits device id
#define RF_RGB_LIGHTS_ID 2
//data length
#define RF_RGB_LIGHTS_DATA_LEN 10