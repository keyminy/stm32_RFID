#include "DS1302.h"
#include <string.h> // for strncpy etc
#include "i2c_lcd.h"


DS1302 stTime;

char *dayofweek[7] = {"SUN", "MON", "TUE", "WED",
	"THU", "FRI", "SAT"};

void DS1302_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// Enable the GPIO clock for the port where the DS1302 pins are connected
	__HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure DS1302_CLK_Pin
    GPIO_InitStruct.Pin = DS1302_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_CLK_GPIO_Port, &GPIO_InitStruct);

    // Configure DS1302_DAT_Pin
    GPIO_InitStruct.Pin = DS1302_DAT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // This pin is also output
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_DAT_GPIO_Port, &GPIO_InitStruct);

    // Configure DS1302_RST_Pin
    GPIO_InitStruct.Pin = DS1302_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // This pin is also output
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_RST_GPIO_Port, &GPIO_InitStruct);

    // initial CE(=DS1302_RST_Pin) is Low
    HAL_GPIO_WritePin(DS1302_RST_GPIO_Port, DS1302_RST_Pin, GPIO_PIN_RESET);
    i2c_lcd_init();
}

void DS1302_InitData(void){
	stTime.year = 24;
	stTime.month = 6;
	stTime.date = 8;
	stTime.dayofweek = 7;
	stTime.hour = 2;
	stTime.minutes = 33;
	stTime.seconds = 0;

	DS1302_SetTimeDates(stTime);
}

void printLCD_DS1302_data(DS1302 *pDS1302){
	char scm[40] = {0,};
//	printf("%02d-%02d %02d:%02d:%02d\n"
//			,pDS1302->month
//			,pDS1302->date
//			,pDS1302->hour
//			,pDS1302->minutes
//			,pDS1302->seconds
//	);
	sprintf(scm,"%02d-%02d %02d:%02d:%02d"
			,pDS1302->month
			,pDS1302->date
			,pDS1302->hour
			,pDS1302->minutes
			,pDS1302->seconds
	);
	move_cursor(0, 0);
	lcd_string(scm);
}

void DS1302_Selected(void)
{
    HAL_GPIO_WritePin(DS1302_RST_GPIO_Port, DS1302_RST_Pin, GPIO_PIN_SET); // CE High
}

void DS1302_Deselected(void)
{
    HAL_GPIO_WritePin(DS1302_RST_GPIO_Port, DS1302_RST_Pin, GPIO_PIN_RESET); // CE Low
}

void DS1302_Clock(void)
{
    HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_RESET); // clk l->h->l
    HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_RESET);
}

void DS1302_DataBitSet(void)
{
    HAL_GPIO_WritePin(DS1302_DAT_GPIO_Port, DS1302_DAT_Pin, GPIO_PIN_SET); // data bit high
}

void DS1302_DataBitReset(void)
{
    HAL_GPIO_WritePin(DS1302_DAT_GPIO_Port, DS1302_DAT_Pin, GPIO_PIN_RESET); // data bit low
}

uint8_t DS1302_Read_DataPin(void)
{
    return HAL_GPIO_ReadPin(DS1302_DAT_GPIO_Port, DS1302_DAT_Pin);
}

void DS1302_Change_ReadMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS1302_DAT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DS1302_DAT_GPIO_Port, &GPIO_InitStruct); // read mode
}

void DS1302_Change_WriteMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS1302_DAT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_DAT_GPIO_Port, &GPIO_InitStruct); // write mode
}

uint8_t decimal_to_bcd(uint8_t decimal)
{
    return (((decimal / 10) << 4) | (decimal % 10));
}

uint8_t bcd_to_decimal(uint8_t bcd)
{
    return (((bcd >> 4) * 10) + (bcd & 0x0f));
}


void DS1302_TxData(uint8_t txData)
{
    DS1302_Change_WriteMode();
    for (int i = 0; i < 8; i++)
    {
        if (txData & (1 << i))
            DS1302_DataBitSet();
        else
            DS1302_DataBitReset();

        DS1302_Clock();
    }
}

void DS1302_WriteData(uint8_t address, uint8_t data)
{
    DS1302_Selected();
    DS1302_TxData(address);
    DS1302_TxData(decimal_to_bcd(data));
    DS1302_Deselected();
}

uint8_t DS1302_RxData(void)
{
    uint8_t rxData = 0;
    DS1302_Change_ReadMode();
    for (int i = 0; i < 8; i++)
    {
        rxData |= DS1302_Read_DataPin() ? (1 << i) : 0;
        if(i!=7){
        	DS1302_Clock(); // Raise clock 7 times.(from datasheet)
        }
    }
    return rxData;
}


uint8_t DS1302_ReadData(uint8_t address)
{
    uint8_t rxData = 0;
    DS1302_Selected();
    DS1302_TxData(address + 1);
    rxData = DS1302_RxData(); // The value read here, "rxData", is 1 byte with 8 bits.
    DS1302_Deselected();

    return bcd_to_decimal(rxData);
}

void DS1302_GetTime(DS1302 *timeData)
{
    timeData->seconds = DS1302_ReadData(ADDRESS_SECONDS);
    timeData->minutes = DS1302_ReadData(ADDRESS_MINUTES);
    timeData->hour = DS1302_ReadData(ADDRESS_HOUR);
}

void DS1302_GetDate(DS1302 *dateData)
{
    dateData->date = DS1302_ReadData(ADDRESS_DATE);
    dateData->month = DS1302_ReadData(ADDRESS_MONTH);
    dateData->dayofweek = DS1302_ReadData(ADDRESS_DAYOFWEEK);
    dateData->year = DS1302_ReadData(ADDRESS_YEAR);
}

void DS1302_SetTimeDates(DS1302 timeDate)
{
    DS1302_WriteData(ADDRESS_SECONDS, timeDate.seconds);
    DS1302_WriteData(ADDRESS_MINUTES, timeDate.minutes);
    DS1302_WriteData(ADDRESS_HOUR, timeDate.hour);
    DS1302_WriteData(ADDRESS_DATE, timeDate.date);
    DS1302_WriteData(ADDRESS_MONTH, timeDate.month);
    DS1302_WriteData(ADDRESS_DAYOFWEEK, timeDate.dayofweek);
    DS1302_WriteData(ADDRESS_YEAR, timeDate.year);
}
