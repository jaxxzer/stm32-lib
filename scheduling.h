/*
 * scheduling.h
 *
 *  Created on: Feb 7, 2018
 *      Author: jack
 */

#ifndef SCHEDULING_H_
#define SCHEDULING_H_



volatile uint32_t MicroSeconds = 0;

void DelayMicros(uint32_t us) {
	volatile uint32_t tStart = MicroSeconds;
	while(MicroSeconds < tStart + us) asm volatile("nop");
}

// Thanks https://hsel.co.uk/2014/06/20/stm32f0-tutorial-2-systick/
//Delay function for millisecond delay
void DelayMil(uint32_t MilS){
	DelayMicros(MilS*1000);
}


//Delay function for second delay
void DelaySec(uint32_t S){
	DelayMicros(S*1000000);
}

#ifdef __cplusplus
 extern "C" {
#endif
 void SysTick_Handler(void){
 	MicroSeconds+=100; //Increment millisecond variable
 }
#ifdef __cplusplus
 }
#endif

//
// class FrequencyTimer
// {
// public:
//	 FrequencyTimer(void);
//	 void update(void) {
//		 uint32_t time = MicroSeconds;
//		 lastDt = dt;
//		 dt = time - lastUpdate;
//		 lastUpdate = time;
//	 };
//	 uint32_t lastUpdate;
//	 uint32_t lastDt;
//	 uint32_t dt;
//	 uint32_t frequencyFiltered;
//	 uint32_t frequency
//	 uint32_t frequency(void) {
//		 frequency = 1.0 / (0.8);
//	 }
// };

#endif /* SCHEDULING_H_ */
