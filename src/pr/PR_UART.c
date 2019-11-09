/*
 * PR_UART.c
 *
 *  Created on: 17 Oct 2019
 *      Author: rnsavinelli (R. Nicolás Savinelli)
 */

#include "DR_UART.h"
#include "PR_UART.h"

// need a flag to tell if the uart was initialized

uint8_t SendData(uint8_t uart_number, const int8_t * data, uint8_t data_length) {
	for(uint8_t i = 0; i < data_length; i++) {
		UARTn_PushTx(uart_number, data[i]);
	}

	return 0;
}
