#include "stm32l476xx.h"
//TODO: define your gpio pin
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT)))|(VAL));};
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


void GPIO_init_AF(){
	//TODO: Initial GPIO pin as alternate function for buzzer. You can choose to use C or assembly to finish this function.
	SET_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN, RCC_AHB2ENR_GPIOAEN)//enable gpioA
	SET_REG(GPIOA->MODER, 0B11<<10, 0B10<<10)//set A5's otype to AF
	SET_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL5, 0B1<<20)//set AF = 1
}
void Timer_init(){
	//TODO: Initialize timer
	SET_REG(RCC->APB1ENR1, RCC_APB1ENR1_TIM2EN, RCC_APB1ENR1_TIM2EN);//enable TIM2
	TIM2->PSC = 119; //prescaler = 100
	TIM2->ARR = 100; // arr = 100

	SET_REG(TIM2->CR1, TIM_CR1_DIR, 0)//count up
	TIM2->CR1 |= TIM_CR1_ARPE;
}
void PWM_channel_init(){
	//TODO: Initialize timer PWM channel

	TIM2->CCER |= TIM_CCER_CC1E;//ENABLE cc1
	SET_REG(TIM2->CCMR1, TIM_CCMR1_CC1S, 0);//set pwm output
	SET_REG(TIM2->CCMR1, TIM_CCMR1_OC1M , (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1));//set PWM mode

	TIM2->EGR = TIM_EGR_UG;
	TIM2->CCR1 = 50;  //set cc to 50
	TIM2->CR1 |= TIM_CR1_CEN;//start timer
}




/* TODO: initial keypad gpio pin, X as output and Y as input */
void keypad_init()
{
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

/* TODO: scan keypad value
return: >=0: key-value pressed¡A-1: keypad is free
*/
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


int main(){
	GPIO_init_AF();
	keypad_init();
	Timer_init();
	PWM_channel_init();
	int duty_cycle = 50;
	//TODO: Scan the keypad and use PWM to send the corresponding frequency square wave to the buzzer.
	while(1){
		int key = keypad_scan();
		if(key == 15 && duty_cycle > 10){
			duty_cycle -= 10;
		}
		else if(key == 14 && duty_cycle < 90){
			duty_cycle += 10;
		}
		TIM2->CCR1 = duty_cycle;
		TIM2->CR1 |= TIM_CR1_CEN;
	}
}
