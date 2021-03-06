/*
 * PR_Timers.c
 *
 *  Created on: 3 Oct 2019
 *      Author: rnsavinelli (R. Nicolás Savinelli)
 */

#include "PR_Timers.h"
#include "DR_Variables.h"

volatile timer_t g_timer[MAX_TIMERS];

void Timers_Initialize(void) {
	for (uint8_t i = 0; i < MAX_TIMERS ; i++) {
		g_timer[i].time = TIMER_RESET;
		g_timer[i].flag = TIMER_OFF;
		g_timer[i].handler = NULL;
	}

	return;
}

void Timers_Daemon(void){
	for (uint8_t i = 0; i < MAX_TIMERS ; i++) {
		if(g_timer[i].time && (g_timer[i].flag == TIMER_RUNNING)) {
			g_timer[i].time--;

			if(g_timer[i].time == TIMER_RESET) {
				g_timer[i].flag = TIMER_COMPLETED;
			}
		}
	}

	return;
}


void TimerSet(uint8_t index, uint32_t time, Timer_Handler handler , uint8_t base ) {
	switch (base) {
		case SEG:
			g_timer[index].time = time * 1000 * INTERRUPTION_FREQUENCY;
			break;

		case MIN:
			g_timer[index].time = time * 60 * 1000 * INTERRUPTION_FREQUENCY;
			break;

		// Defaults to mseg
		default:
			g_timer[index].time = time * INTERRUPTION_FREQUENCY;
			break;
	}

	g_timer[index].handler = handler;

	return;
}

void TimerStart(uint8_t index) {
	if(g_timer[index].time != TIMER_RESET) {
		g_timer[index].flag = TIMER_RUNNING;
	}

	return;
}

void TimerSetAndStart(uint8_t index, uint32_t time, Timer_Handler handler , uint8_t base) {
	TimerSet(index, time, handler, base);
	TimerStart(index);

	return;
}

void TimerStop(uint8_t index) {
	if(g_timer[index].time != TIMER_RESET) {
		g_timer[index].time = TIMER_RESET;
		g_timer[index].flag = TIMER_OFF;
		g_timer[index].handler = NULL;
	}

	return;
}

uint32_t GetTimerTime(uint8_t index) {
	return g_timer[index].time;
}

uint32_t GetTimerFlag(uint8_t index) {
	return g_timer[index].flag;
}

void TimerPause(uint8_t index) {
	if(g_timer[index].time != TIMER_RESET) {
		g_timer[index].flag = TIMER_OFF;
	}

	return;
}

void TimerResume(uint8_t index) {
	if(g_timer[index].time != TIMER_RESET) {
		if(g_timer[index].flag == TIMER_PAUSE){
			g_timer[index].flag = TIMER_RUNNING;
		}
	}

	return;
}

void TimerEvents(void) {
	for (uint8_t i = 0; i < MAX_TIMERS ; i++) {
		if(g_timer[i].flag == TIMER_COMPLETED) {
			if(g_timer[i].handler != NULL) {
				g_timer[i].handler();
			}

			g_timer[i].time = TIMER_RESET;
			g_timer[i].flag = TIMER_OFF;
			g_timer[i].handler = NULL;
		}
	}
}

void TimersClear(void) {
	Timers_Initialize();
	return;
}

void TimerClear(uint8_t index) {
	g_timer[index].time = TIMER_RESET;
	g_timer[index].flag = TIMER_OFF;
	g_timer[index].handler = NULL;

	return;
}
