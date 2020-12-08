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

extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

unsigned int x_pin[4] = {X0, X1, X2, X3};
unsigned int y_pin[4] = {Y0, Y1, Y2, Y3};
unsigned int Table[4][4] = {{1,2,3,10},{4,5,6,11},{7,8,9,12},{15,0,14,13}};
int n = 0;

void GPIO_init_AF(){
	//TODO: Initial GPIO pin as alternate function for buzzer. You can choose to use C or assembly to finish this function.
	SET_REG(GPIOB->MODER, 0B11<<6, 0B10<<6)//set A5's otype to AF
	SET_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL3, 0B1<<12)//set AF = 1
}

void Timer_init(){
	//TODO: Initialize timer
	SET_REG(RCC->APB1ENR1, RCC_APB1ENR1_TIM2EN, RCC_APB1ENR1_TIM2EN);//enable TIM2
	TIM2->PSC = 30;
	TIM2->ARR = 39;

	SET_REG(TIM2->CR1, TIM_CR1_DIR, 0)//count up
	TIM2->CR1 |= TIM_CR1_ARPE;
}

void PWM_channel_init(){
	//TODO: Initialize timer PWM channel

	TIM2->CCER |= TIM_CCER_CC2E;//ENABLE cc2
	SET_REG(TIM2->CCMR1, TIM_CCMR1_CC2S, 0);//set pwm output
	SET_REG(TIM2->CCMR1, TIM_CCMR1_OC2M , (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1));//set PWM mode

	TIM2->EGR = TIM_EGR_UG;
	TIM2->CCR2 = 0;  //set cc to 0
	TIM2->CR1 |= TIM_CR1_CEN;//start timer
}

void init_GPIO()
{
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN)
	SET_REG(GPIOA->MODER, 0B11<<10, 0B01<<10)
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN, RCC_AHB2ENR_GPIOCEN)
	SET_REG(GPIOC->MODER, 0B11<<26, 0B00<<26)
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
	SET_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13, SYSCFG_EXTICR4_EXTI13_PC)

	EXTI->IMR1 |= EXTI_IMR1_IM5 | EXTI_IMR1_IM6 | EXTI_IMR1_IM7 | EXTI_IMR1_IM9 | EXTI_IMR1_IM13;
	EXTI->RTSR1 |= EXTI_RTSR1_RT5 | EXTI_RTSR1_RT6 | EXTI_RTSR1_RT7 | EXTI_RTSR1_RT9 | EXTI_RTSR1_RT13;
	EXTI->PR1 |= EXTI_PR1_PIF5 | EXTI_PR1_PIF6 | EXTI_PR1_PIF7 | EXTI_PR1_PIF9 | EXTI_PR1_PIF13;
}

void NVIC_config()
{
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}

void display(){
	for(int i = 1 ; i <= 8; i++)
		max7219_send(i, 15);
	if(n >= 10){
		max7219_send(1, n%10);
		max7219_send(2, n/10);
	}
	else
		max7219_send(1, n);
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
					if(ans1 == -1 && cnt > 50)
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
void EXTI15_10_IRQHandler()
{
	TIM2->CCR2 = 0;

	EXTI->PR1 |= EXTI_PR1_PIF13;
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}
void EXTI9_5_IRQHandler()
{
	int value = keypad_scan();
	if(value >= 0 && value < 16){
		SysTick_Config(4000000 / 8);
		n = value;
		NVIC_DisableIRQ(EXTI9_5_IRQn);
		display();
		if(value == 0){
			TIM2->CCR2 = 20;
		}
	}
	EXTI->PR1 |= EXTI_PR1_PIF5 | EXTI_PR1_PIF6 | EXTI_PR1_PIF7 | EXTI_PR1_PIF9;
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}

void SysTick_Handler()
{
	n--;
	display();
	if(n <= 0){
		n = 0;
		display();
		TIM2->CCR2 = 20;
		SET_REG(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 0)
	}
}




int main() {
	SET_REG(RCC->CFGR, RCC_CFGR_HPRE, 0B1010<<4);
	display();
	GPIO_init();
	max7219_init();
	NVIC_config();
	EXTI_config();
	init_GPIO();
	GPIO_init_AF();
	Timer_init();
	PWM_channel_init();
	while(1);
}
