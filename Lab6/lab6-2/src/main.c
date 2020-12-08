#include "stm32l476xx.h"
#define TIME_SEC 0.10
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT)))|(VAL));};
extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);
int display(int data, int num_digs);
void GPIO_init2()
{
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN)
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN, RCC_AHB2ENR_GPIOCEN)
	SET_REG(GPIOA->MODER, 0B11<<10, 0B01<<10)
	SET_REG(GPIOC->MODER, 0B11<<26, 0B00<<26)
}
void Timer_init() {
	//TODO: Initialize timer
	SET_REG(RCC->APB1ENR1, RCC_APB1ENR1_TIM2EN, RCC_APB1ENR1_TIM2EN);
	TIM2->PSC = 40000 - 1;//UPDATE CNT EVERY 0.01 SEC
	TIM2->ARR = TIME_SEC * 100;
	TIM2->EGR = TIM_EGR_UG; //reinitialize timer
	TIM2->SR &= ~TIM_SR_UIF;
}

void Timer_start() {
	//TODO: start timer and show the time on the 7-SEG LED.
	TIM2->CR1 |= TIM_CR1_CEN;//start timer
}

int main(){
	GPIO_init();
	GPIO_init2();
	max7219_init();
	Timer_init();
	Timer_start();
	while(!(TIM2->SR & TIM_SR_UIF)) {
		int timer_value = TIM2->CNT;
		display(timer_value, 8);
	}
}

int display(int data, int num_digs){
	int first = 0;
	int digit[num_digs];
	for(int i = 0; i < num_digs; i++){
		digit[i] = data % 10;
		data /= 10;
	}

	for(int i = num_digs; i > 0; i--){
		if(digit[i-1] != 0)
			first = 1;
		if(i == 3)
			max7219_send(i, digit[i-1] | 0b10000000);
		else if(first == 1 || i == 1 || i <= 2)
			max7219_send(i, digit[i-1]);
		else
			max7219_send(i, 15);
	}
	return 0;
};
