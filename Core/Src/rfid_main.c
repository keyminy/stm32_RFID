#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rfid_main.h"
#include "mfrc522.h"    // for RFID
#include "def.h"

uint8_t rfid_check_flag=0;  // 현재 rfid를 checking중인지

extern SPI_HandleTypeDef hspi1;   // for RFID interface
extern TIM_HandleTypeDef htim3;   // for servo motor interface

volatile int T300ms_counter=0;
uint8_t readData;   // 1 byte save variable
uint8_t rxDataStr[MAX_LEN];   // rfid tagging data, MAX_LEN = 16

static uint8_t rfid_status = NORMAL;

cardDB cardInfo = {0,{0}};

void set_rfid_status(uint8_t pRFID_STAUS){
	rfid_status = pRFID_STAUS;
}

// RFID READER initialize function
void rfid_reader_init(void)
{
	mfrc522_init(&hspi1);
	HAL_Delay(1000);    // wait for RFID init

	readData = mfrc522_read(VersionReg);
	if (readData == 0x92)
	{
		printf("MIFARE RC522v2\n");
		printf("Detected !!!\n");
	}
	else if (readData == 0x91 || readData == 0x90)
	{
		printf("MIFARE RC522v1\n");
		printf("Detected !!!\n");
	}
	else
	{
		printf("No RFID Reader found !!!\n");
	}
}

// RFID tagging processing function
void rfid_tag_processing(void) // <- I'm gonna use in while loop
{
	switch(rfid_status){
	case NORMAL:
		if (T300ms_counter >= 300)  // polling 300ms rfid reader
		{
			T300ms_counter = 0;
			//1. Make sure that the card is touching the reader.
			// == RFID contack check
			readData = mfrc522_request(PICC_REQALL, rxDataStr);
			if (readData == CARD_FOUND) {
				// 2.Read the card information.
				for (int i = 0; i < MAX_LEN; i++)
					rxDataStr[i] = ' ';
				readData = mfrc522_get_card_serial(rxDataStr);
				printf("rfid:");
				for (int i = 0; i < 5; i++) {
					printf("%02x ", rxDataStr[i]);
				}
				printf("\n");
				 register_rfid_key(&cardInfo, &rxDataStr);
				 print_registered_keys(&cardInfo);
				if (removeCard(&cardInfo, &rxDataStr)) {
					printf("Card Removed\n");
				}
				 print_registered_keys(&cardInfo);
			}
		}
		break;
	case ENROLL:

		break;
	case DELETE:
		break;
	case RETREIVE_ALL:
		break;
	}
}


bool isExistCard(cardDB* card, uint8_t* pRxDataStr) {
    for (int i = 0; i < card->registeredKeyCount; i++) {
        if (memcmp(card->regCardKey[i], pRxDataStr, 5) == 0)
            return true;
    }
    return false;
}

void register_rfid_key(cardDB* card, uint8_t* newKey) {
    if (!isExistCard(card,newKey)) {
        if (card->registeredKeyCount < MAX_REGISTERED_KEYS) {
            memcpy(card->regCardKey[card->registeredKeyCount], newKey, 5);
            card->registeredKeyCount++;
            printf("RFID Key Registered!\n");
        }
        else {
            printf("RFID Key Storage Full\n");
        }
    }
    else {
        printf("RFID Key Already Registered\n");
    }
}

void print_registered_keys(cardDB* card) {
    printf("Print All registered RFID Keys:\n");
    for (int i = 0; i < card->registeredKeyCount; i++) {
        printf("Key %d: ", i + 1);
        for (int j = 0; j < 5; j++) {
            printf("%02x ", card->regCardKey[i][j]);
        }
        printf("\n");
    }
}

bool removeCard(cardDB* card, uint8_t* pRxDataStr) {
    int index = -1;

	for (int i = 0; i < card->registeredKeyCount; i++) {
		if (memcmp(card->regCardKey[i], pRxDataStr, 5) == 0) {
            index = i;
            break;
		}
	}
    if (index == -1) {
        return false;
    }
    // shift the rest of the arrays.
	for (int i = index; i < card->registeredKeyCount - 1; i++) {
		memcpy(card->regCardKey[i], card->regCardKey[i + 1], 5);
	}
	card->registeredKeyCount--;
	return true;
}

//------- start RFID DEMO ----------------
#if 0
void rfid_control_main(void)
{
printf("rfid_control_main start !!!!\n");

	rfid_reader_init();

	while(1)
	{
		readData = mfrc522_request(PICC_REQALL, rxDataStr);
		// RFID contack check
		if (readData == CARD_FOUND)
		{
			for (int i=0; i < MAX_LEN; i++)
				rxDataStr[i]=' ';
			readData = mfrc522_get_card_serial(rxDataStr);
			for (int i=0; i < 5; i++)
			{
				printf("%02x ", rxDataStr[i]);
			}

//			for (int i=0; i < MAX_REGISTERED_KEYS; i++)
//			{
//				if (strncmp(rxDataStr,regCardKey[i],5) == 0)
//				{
//					for (int i=0; i < 5; i++)
//					{
//						printf("%02x ", rxDataStr[i]);
//					}
//					printf("Valid Card !!!\n");
//					printf("DOOR Opened !!!\n");
//					__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 100); //2ms 100(2ms)/1000(20ms) :180degree
//					HAL_Delay(2000);
//
//					__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 50);  //1ms :180degree
//					HAL_Delay(1000);
//					printf("DOOR Closed !!!\n");
//
//					cardNotFound=0;
//					break;
//				}
//				cardNotFound=1;
//			}
//			if (cardNotFound)
//			{
//				for (int i=0; i < 5; i++)
//				{
//					printf("%02x ", rxDataStr[i]);
//				}
//				printf("Invalid Card !!!\n");
//			}
		}
		HAL_Delay(1000);
	}
}
#endif
//-------- end RFID DEMO -----------------
