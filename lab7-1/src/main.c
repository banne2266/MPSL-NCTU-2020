#include "../inc/stm32l476xx.h"
#include "../inc/core_cm4.h"
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT)))|(VAL));}

int led_state = 0;

void GPIO_init()
{
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN)
	SET_REG(GPIOA->MODER, 0B11<<10, 0B01<<10)
};

//uint32_t SysTick_Config(uint32_t ticks); in core_cm4.h

void SystemClock_Config()
{
	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY)){}
	SET_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSI);
	SET_REG(RCC->CFGR, RCC_CFGR_HPRE, 0B1010<<4);
}


void SysTick_Handler()
{
	if(led_state == 0){
		GPIOA->BSRR = 1<<5;
		led_state = 1;
	}
	else{
		GPIOA->BRR = 1<<5;
		led_state = 0;
	}
};



int main(){
	GPIO_init();
	SystemClock_Config();
	SysTick_Config(16000000 * 3 / 8);
	while(1);
}
