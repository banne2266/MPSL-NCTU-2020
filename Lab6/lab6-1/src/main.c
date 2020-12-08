#include "stm32l476xx.h"
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT)))|(VAL));}
void GPIO_init();
void Delay1sUnder4MHz();
void Set_HCLK(int freq);
void hclk_init();
void pll_enable();
void pll_disable();
extern int CheckPress();


int main(){
	// Do initializations.
	GPIO_init();
	hclk_init();
	int freq[] = {1, 6, 10, 16, 40};
	int led_state = 0, freq_index = -1;
	//Set_HCLK(freq[0]);
	for(;;){
		// change LED state
		if(led_state == 0){
			GPIOA->BSRR = 1<<5;
			led_state = 1;
		}
		else{
			GPIOA->BRR = 1<<5;
			led_state = 0;
		}
		Delay1sUnder4MHz();
		if(CheckPress() != 0){
			freq_index = (freq_index + 1) % 5;
			Set_HCLK(freq[freq_index]);
		}
		// change HCLK if button pressed
	}
}

void Set_HCLK(int freq){
	pll_disable();
	if(freq > 32){
		SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN, (freq*2)<< 8);
		SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLR, RCC_PLLCFGR_PLLR_0);
	}
	else{
		SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN, (freq*4)<< 8);
		SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLR, RCC_PLLCFGR_PLLR_0 | RCC_PLLCFGR_PLLR_1);
	}
	pll_enable();

	SET_REG(RCC->CFGR, RCC_CFGR_HPRE, 0);
	return;
	// 1. change to the temporary clock source if needed
	// 2. set the target clock source
	// 3. change to the target clock source
}

void GPIO_init()
{
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN)
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN, RCC_AHB2ENR_GPIOCEN)
	SET_REG(GPIOA->MODER, 0B11<<10, 0B01<<10)
	SET_REG(GPIOC->MODER, 0B11<<26, 0B00<<26)
}

void Delay1sUnder4MHz()
{
	int i = 600000;
	while(i--);
	return;
}

void hclk_init(){
	// Make sure MSI is ready
	RCC->CR |= RCC_CR_MSION;
	while(!(RCC->CR & RCC_CR_MSIRDY)){}
	SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, RCC_PLLCFGR_PLLSRC_MSI); // set MSI as PLL source
	RCC->CR |= RCC_CR_MSIRGSEL; // enable MSIRGSEL

	// Set up predetermined parameters
	SET_REG(RCC->CR, RCC_CR_MSIRANGE, 0B0110 << 4);		// MSI = 4MHZ
	pll_disable();
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; 						// enable main PLL output
	SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, 0);			// PLLM = 1


	SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN, 16 << 8); 			// PLLN = 16
	SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLR, RCC_PLLCFGR_PLLR_0 | RCC_PLLCFGR_PLLR_1) // PLLR = 8
	pll_enable();
}

// Disable PLL and set MSI as clock source
void pll_disable() {
	SET_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_MSI); // switch to MSI
	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY){}
}

// Re-enable PLL as clock source
void pll_enable() {
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)){}
	SET_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
}
