#include "uart2.h"

extern UART_HandleTypeDef huart2;
extern uint8_t rx_data;

// move from HAL_UART_RxCpltCallback of stm32f4xx_hal_uart to here
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart2)
	{
		unsigned char data;

		data = rx_data;


		HAL_UART_Receive_IT(&huart2, &rx_data, 1);   // 반드시 집어 넣어야 한다. assign RX INT
	}

}
