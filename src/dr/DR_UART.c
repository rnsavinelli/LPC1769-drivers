/*
 * DR_UART.c
 *
 *  Created on: 10 Oct 2019
 *      Author: rnsavinelli (R. Nicolás Savinelli)
 */

#include "DR_UART.h"
#include "DR_Variables.h"
#include "DR_GPIO.h"
#include "DR_PINSEL.h"
#include "DR_NVIC.h"
#include "PR_UART.h"

/* VARIABLES FOR UART0 */
uint8_t g_inTx0, g_outTx0;
uint8_t g_bufferTx0[TX0_BUFFER_SIZE];

uint8_t g_inRx0, g_outRx0;
uint8_t g_bufferRx0[RX0_BUFFER_SIZE];

/* VARIABLES FOR UART1 */
uint8_t g_inTx1, g_outTx1;
uint8_t g_bufferTx1[TX1_BUFFER_SIZE];

uint8_t g_inRx1, g_outRx1;
uint8_t g_bufferRx1[RX1_BUFFER_SIZE];

void UARTn_Initialize(uint8_t uart_number) {
	if (uart_number == UART0) {
		PCONP[3] |= 0x01; /* ENABLES UART0 */
		PCLKSEL0 &= ~(0x03 << 6); /* Sets clock to 25 MHz for UART0 */
		U0LCR |= 0x00000011; /* 8-bit character length, 1 stop bit,
								no parity, no break condition */
		U0LCR |= (0x00000001 << 7); /* ENABLES DLAB ACCESS */
		U0DLM = ( (uint32_t) (25000000 / (16 * UART0_BAUDRATE) ) ) & (0xFF); /* Sets the baudrate */
		U0DLL = ( ( (uint32_t) (25000000 / (16 * UART0_BAUDRATE) ) ) & (0xFF << 8) ) >> 8;
		SetPINSEL(TXD0, PINSEL_ALT1); /* TXD0 */
		SetPINSEL(RXD0, PINSEL_ALT1); /* RXD0 */
		U0LCR &= (0x00000001 << 7); /* DISABLES DLAB ACCESS */
		U0IER = 0x03; /* Enable the RDA and THRE interrupts */
		ISER[0] |= (1 << NVIC_UART0); /* Enables NVIC interrupts */
	}

	if (uart_number == UART1) {
		PCONP[4] |= 0x01; /* ENABLES UART1 */
		PCLKSEL0 &= ~(0x03 << 8); /* Sets clock to 25 MHz for UART1 */
		U1LCR |= 0x00000011; /* 8-bit character length, 1 stop bit,
								no parity, no break condition */
		U1LCR |= (0x00000001 << 7); /* ENABLES DLAB ACCESS */
		U1DLM = ( (uint32_t) (25000000 / (16 * UART1_BAUDRATE) ) ) & (0xFF); /* Sets the baudrate */
		U1DLL = ( ( (uint32_t) (25000000 / (16 * UART1_BAUDRATE) ) ) & (0xFF << 8) ) >> 8;
		SetPINSEL(TXD1, PINSEL_ALT1); /* TXD1 */
		SetPINSEL(RXD1, PINSEL_ALT1); /* RXD1 */
		U1LCR &= (0x00000001 << 7); /* DISABLES DLAB ACCESS */
		U1IER = 0x03; /* Enable the RDA and THRE interrupts */
		ISER[0] |= (1 << NVIC_UART1); /* Enables NVIC interrupts */
	}

	return;
}

void UART0_IRQHandler(void)
{
	uint8_t iir_register, dataTx;

	do {
		iir_register = U0IIR;

		if(iir_register & (0x01 << 1) ) {
			dataTx = UARTn_PopTx(UART0);
			if(dataTx != EMPTY) {
				U0THR = dataTx;
			}
		}

		if(iir_register&(0x01<<2)) {
			UARTn_PushRx(UART0, U0RBR);
		}

	} while(iir_register & 0x01);

	return;
}


void UARTn_PushTx(uint8_t uart_number, uint8_t data_byte) {
	if (uart_number == 0) {
		if(g_inTx0 == g_outTx0) {
			U0THR = data_byte;
		}

		g_bufferTx0[g_inTx0] = data_byte;
		g_inTx0 ++;

		if (g_inTx0 == TX0_BUFFER_SIZE) {
			g_inTx0 = 0;
		}
	}

	if (uart_number == 1) {
		if(g_inTx1 == g_outTx1) {
			U1THR = data_byte;
		}

		g_bufferTx1[g_inTx1] = data_byte;
		g_inTx1 ++;

		if (g_inTx1 == TX1_BUFFER_SIZE) {
			g_inTx1 = 0;
		}
	}
}

int16_t UARTn_PopTx(uint8_t uart_number) {
	if (uart_number == 0) {
		g_outTx0 ++;
		g_outTx0 %= TX0_BUFFER_SIZE;

		if(g_outTx0 != g_inTx0) {
			return g_bufferTx0[g_outTx0];
		}
	}

	else if (uart_number == 1 ) {
		g_outTx1 ++;
		g_outTx1 %= TX1_BUFFER_SIZE;

		if(g_outTx1 != g_inTx1) {
			return g_bufferTx1[g_outTx1];
		}
	}

	return -1;
}

void UARTn_PushRx(uint8_t uart_number, uint8_t data_byte) {
	if (uart_number == 0) {
		g_bufferRx0[g_inRx0] = data_byte;
		g_inRx0 ++;

		if (g_inRx0 == RX0_BUFFER_SIZE) {
			g_inRx0 = 0;
		}
	}

	if (uart_number == 1) {
		g_bufferRx1[g_inRx1] = data_byte;
		g_inRx1 ++;

		if (g_inRx1 == RX1_BUFFER_SIZE) {
			g_inRx1 = 0;
		}
	}
}

int16_t UARTn_PopRx(uint8_t uart_number) {
	if (uart_number == 0) {
		g_outRx0 ++;
		g_outRx0 %= TX0_BUFFER_SIZE;

		if(g_outRx0 != g_inRx0) {
			return g_bufferRx0[g_outRx0];
		}
	}

	else if (uart_number == 1 ) {
		g_outRx1 ++;
		g_outRx1 %= RX1_BUFFER_SIZE;

		if(g_outRx1 != g_inRx1) {
			return g_bufferRx1[g_outRx1];
		}
	}

	return -1;
}
