#include "../inc/stm32l476xx.h"
#include "../inc/core_cm4.h"
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT)))|(VAL));}
#define WAITING_TIME 200000

#define X0 8
#define X1 9
#define X2 10
#define X3 12
#define Y0 5
#define Y1 6
#define Y2 7
#define Y3 9

unsigned int x_pin[4] = {X0, X1, X2, X3};
unsigned int y_pin[4] = {Y0, Y1, Y2, Y3};
unsigned int Table[4][4] = {{1,2,3,10},{4,5,6,11},{7,8,9,12},{15,0,14,13}};


void init_GPIO()
{
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN)
	SET_REG(GPIOA->MODER, 0B11<<10, 0B01<<10)
	GPIOA->BSRR = 1<<5;

// SET keypad gpio OUTPUT //
	RCC->AHB2ENR = RCC->AHB2ENR|0x2;
	//Set PA8,9,10,12 as output mode
	GPIOA->MODER = GPIOA->MODER&0xFDD5FFFF;
	//set PA8,9,10,12 is Pull-up output
	//GPIOA->PUPDR = GPIOA->PUPDR|0x1150000;
	GPIOA->PUPDR = 0x22A0000;
	//Set PA8,9,10,12 as medium speed mode
	GPIOA->OSPEEDR = GPIOA->OSPEEDR|0x1150000;
	//Set PA8,9,10,12 as high
	GPIOA->ODR = GPIOA->ODR|10111<<8;
// SET keypad gpio INPUT //
	//Set PB5,6,7,9 as INPUT mode
	GPIOB->MODER = GPIOB->MODER&0xFFF303FF;
	//set PB5,6,7,9 is Pull-down input
	GPIOB->PUPDR = GPIOB->PUPDR|0x8A800;
	//Set PB5,6,7,9 as medium speed mode
	GPIOB->OSPEEDR = GPIOB->OSPEEDR|0x45400;
}

void EXTI_config()
{
	SET_REG(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN, RCC_APB2ENR_SYSCFGEN)
	SET_REG(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI5, SYSCFG_EXTICR2_EXTI5_PB)
	SET_REG(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI6, SYSCFG_EXTICR2_EXTI6_PB)
	SET_REG(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI7, SYSCFG_EXTICR2_EXTI7_PB)
	SET_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI9, SYSCFG_EXTICR3_EXTI9_PB)

	EXTI->IMR1 |= EXTI_IMR1_IM5 | EXTI_IMR1_IM6 | EXTI_IMR1_IM7 | EXTI_IMR1_IM9;
	EXTI->RTSR1 |= EXTI_RTSR1_RT5 | EXTI_RTSR1_RT6 | EXTI_RTSR1_RT7 | EXTI_RTSR1_RT9;
	EXTI->PR1 |= EXTI_PR1_PIF5 | EXTI_PR1_PIF6 | EXTI_PR1_PIF7 | EXTI_PR1_PIF9;
}

void NVIC_config()
{
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}

int keypad_scan(){
	int flag_keypad=GPIOB->IDR&10111<<5;
	int ans1 = -1, cnt = 0;

	while(flag_keypad!=0){
		for(int i=0;i<4;i++){ //scan keypad from first column
			//set PA8,9,10,12(column) low and set pin high from PA8
			GPIOA->ODR= (GPIOA->ODR & 0xFFFFE8FF) | 1 << x_pin[i];
			for(int j=0;j<4;j++){ //read input from first row
				int flag_keypad_r = GPIOB->IDR&1<<y_pin[j];
				if(flag_keypad_r!=0){
					if(ans1 == -1 && cnt > 400)
						ans1 = Table[j][i];
				}
			}
		}
		cnt++;
		GPIOA->ODR = GPIOA->ODR|0b10111<<8;
		flag_keypad=GPIOB->IDR&10111<<5;
	}
	GPIOA->ODR = GPIOA->ODR|0b10111<<8;
	return ans1;
}

void EXTI9_5_IRQHandler()
{
	int value = keypad_scan(), timer = WAITING_TIME;
	while(value > 0){
		while(timer--);
		timer = WAITING_TIME;
		GPIOA->BRR = 1<<5;

		while(timer--);
		timer = WAITING_TIME;
		GPIOA->BSRR = 1<<5;

		value--;
	}
	EXTI->PR1 |= EXTI_PR1_PIF5 | EXTI_PR1_PIF6 | EXTI_PR1_PIF7 | EXTI_PR1_PIF9;
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}



int main() {
	NVIC_config();
	EXTI_config();
	init_GPIO();
	while(1);
}
