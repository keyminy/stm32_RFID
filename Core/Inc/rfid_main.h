/*
 * rfid_main.h
 *
 *  Created on: Jun 5, 2024
 *      Author: HARMAN-27
 */

#ifndef INC_RFID_MAIN_H_
#define INC_RFID_MAIN_H_

#include "main.h"
#include <stdbool.h>
#define MAX_REGISTERED_KEYS  10

typedef struct _card
{
	 uint8_t registeredKeyCount; // Counter for registered keys
	 uint8_t regCardKey[MAX_REGISTERED_KEYS][5];
} cardDB;


void rfid_reader_init(void);
void rfid_tag_processing(void);
void set_rfid_status(uint8_t pRFID_STAUS);
uint8_t get_rfid_status(void);
bool isExistCard(cardDB* card, uint8_t* pRxDataStr);
void register_rfid_key(cardDB* card, uint8_t* newKey);
void print_registered_keys(cardDB* card);
bool removeCard(cardDB* card, uint8_t* pRxDataStr);


#endif /* INC_RFID_MAIN_H_ */
