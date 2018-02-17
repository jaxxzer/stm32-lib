/*
 * scheduling.h
 *
 *  Created on: Feb 7, 2018
 *      Author: jack
 */

#ifndef SCHEDULING_H_
#define SCHEDULING_H_



volatile uint32_t Milliseconds = 0, Seconds = 0;

// Thanks https://hsel.co.uk/2014/06/20/stm32f0-tutorial-2-systick/
//Delay function for millisecond delay
void DelayMil(uint32_t MilS){
	volatile uint32_t MSStart = Milliseconds;
	while((Milliseconds - MSStart)<MilS) asm volatile("nop");
}

//Delay function for second delay
void DelaySec(uint32_t S){
	volatile uint32_t Ss = Seconds;
	while((Seconds - Ss)<S) asm volatile("nop");
}


#endif /* SCHEDULING_H_ */
