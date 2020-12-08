#include "../inc/stm32l476xx.h"
#include "../inc/core_cm4.h"
#include <string.h>
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT)))|(VAL));}
#define BRAUD_RATE 9600L
int ADC_data = 1;
char buf[300];

void gpio_init() {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN);
	// UART
	SET_REG(GPIOA->MODER, GPIO_MODER_MODE9, 0b10<<18)
	SET_REG(GPIOA->MODER, GPIO_MODER_MODE10, 0b10<<20)

	SET_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD9, 2<<18)
	SET_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD10, 2<<20)

	SET_REG(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEEDR9, 0b00<<18)
	SET_REG(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEEDR10, 0b00<<20)

	SET_REG(GPIOA->AFR[1], GPIO_AFRH_AFSEL9, 7<<4)
	SET_REG(GPIOA->AFR[1], GPIO_AFRH_AFSEL10, 7<<8)


	// BUTTON
	SET_REG(GPIOC->MODER, GPIO_MODER_MODE13, 0b00<<26)
	SET_REG(GPIOC->PUPDR, GPIO_PUPDR_PUPD13, 0b00<<26)
	SET_REG(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR13, 0b01<<26)

	//ADC
	SET_REG(GPIOA->MODER, GPIO_MODER_MODE0, 0b11<<0)
	//SET_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD0, 2<<0)
	GPIOA->ASCR |= (1<<0);//analog switch control register

	//LED
	SET_REG(GPIOA->MODER, GPIO_MODER_MODE5, 0b01<<10)
	SET_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD5, 0b00<<10)
	SET_REG(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEEDR5, 0b01<<10)
}

/* uart */
int UART_Transmit(uint8_t *arr, uint32_t size) {
	int ret = 0;
	for (int i = 0; arr[i] && i < size; i ++) {
		while ((USART1->ISR & USART_ISR_TXE) == 0);
		USART1->TDR = arr[i];
		ret ++;
	}
	while ((USART1->ISR & USART_ISR_TXE) == 0);
	return ret;
}

char receive_char() {
	while (!(USART1->ISR & USART_ISR_RXNE));
	// USART1->RQR |= USART_RQR_RXFRQ;
	USART1->ISR = USART1->ISR & ~USART_ISR_RXNE;
	return USART1->RDR & 0xFF;
}

void read_cmd() {
	int ptr = 0;
	char c;
	do {
		c = receive_char();
		if((isalpha(c) || c == ' ' || isdigit(c)) && ptr < 300){
			buf[ptr++] = c;
			UART_Transmit((uint8_t*)&c, 1);
		}
		else if((c == 0x08 || c == 0x7F) && ptr > 0){
			buf[ptr--] = 0;
			UART_Transmit((uint8_t*)&c, 1);
			UART_Transmit((uint8_t*)" ", 1);
			UART_Transmit((uint8_t*)&c, 1);
		}
	} while (c != '\n' && c != '\r');
	buf[ptr++] = '\0';
	UART_Transmit((uint8_t*)"\r\n", 2);
}

void init_UART() {
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// CR1
	SET_REG(USART1->CR1, USART_CR1_M, 0)
	SET_REG(USART1->CR1, USART_CR1_PS, 0)
	SET_REG(USART1->CR1, USART_CR1_PCE, 0)
	SET_REG(USART1->CR1, USART_CR1_OVER8, 0)

	SET_REG(USART1->CR1, USART_CR1_TE, USART_CR1_TE)//enable transmit
	SET_REG(USART1->CR1, USART_CR1_RE, USART_CR1_RE)//enable receive

	// CR2
	SET_REG(USART1->CR2, USART_CR2_STOP, 0)//1 stop bit

	// BRR
	USART1->BRR &= ~(0xFF);
	USART1->BRR |= 4000000L / BRAUD_RATE ;

	/* In asynchronous mode, the following bits must be kept cleared:
	- LINEN and CLKEN bits in the USART_CR2 register,
	- SCEN, HDSEL and IREN bits in the USART_CR3 register.*/
	USART1->CR2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
	USART1->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);

	// Enable UART
	USART1->CR1 |= (USART_CR1_UE);
}

void init_ADC(){
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

	SET_REG(ADC1->CFGR, ADC_CFGR_CONT, 0)// continue
	SET_REG(ADC1->CFGR, ADC_CFGR_ALIGN, 0)

	ADC123_COMMON->CCR |= 1 << 16; // ckmode
	ADC123_COMMON->CCR |= 4 << 8; // delay

	SET_REG(ADC1->CFGR, ADC_CFGR_RES, 0b00<<3)//set resolution to 12 bit

	ADC1->SQR1 |= 0<<0;
	ADC1->SQR1 |= 5<<6;//use channel 5

	ADC1->SMPR1 |= 2<<15;


	SET_REG(ADC1->CR, ADC_CR_DEEPPWD, 0)//wake from Deep-power-down mode
	SET_REG(ADC1->CR, ADC_CR_ADVREGEN, ADC_CR_ADVREGEN)

	for (int i=0; i<200; i++);


	ADC1->IER |= ADC_IER_EOCIE;//make interrupt enable
	NVIC_EnableIRQ(ADC1_2_IRQn);

	ADC1->CR |= ADC_CR_ADEN;//enable ADC
	while (!(ADC1->ISR & ADC_ISR_ADRDY));//wait until ready
	//ADC1->CR |= ADC_CR_ADSTART;//start ADC
}

void UART_Transmit_Number(int n) {
	int dig[4] = {0};
	for (int i = 0; i < 4; i ++) {
		dig[i] = n % 10;
		n /= 10;
	}
	for (int i = 3; i >= 0; i --) {
		char c = '0' + dig[i];
		UART_Transmit((uint8_t*)&c, 1);
	}
	UART_Transmit((uint8_t*)"\r\n", 2);
}

void ADC1_2_IRQHandler(){
	NVIC_ClearPendingIRQ(ADC1_2_IRQn);
	for (int i=0; i<(1<<8); i++);
	ADC_data = ADC1->DR;
	ADC1->ISR |= ADC_ISR_EOC;
	UART_Transmit_Number(ADC_data);

	NVIC_ClearPendingIRQ(ADC1_2_IRQn);
}

void systick_enable() {
	SysTick->CTRL |= 1;
}

void systick_disable() {
	SysTick->CTRL &= ~1;
}

void systick_config() {
	SysTick->CTRL |= 7;
	SysTick->LOAD = 2000000;
}

void SysTick_Handler() {
	ADC1->CR |= ADC_CR_ADSTART;
}

int main() {
	gpio_init();
	systick_config();
	init_UART();
	init_ADC();
	systick_disable();

	UART_Transmit((uint8_t*)"\r\nWelcome to lab8-3\r\n", 21);

	while (1) {
		UART_Transmit((uint8_t*)"> ", 2);
		read_cmd();

		if (strncmp(buf, "showid", 6) == 0)
			UART_Transmit((uint8_t*)"0716325\r\n", 15);
		else if (strncmp(buf, "light", 5) == 0){
			systick_enable();
			char c;
			do {
				c = receive_char();
			} while (c != 'q');
			systick_disable();
		}
		else if (strncmp(buf, "led on", 6) == 0)
			GPIOA->BSRR = 1<<5;
		else if (strncmp(buf, "led off", 7) == 0)
			GPIOA->BRR = 1<<5;
		else if (strncmp(buf, "", 1) == 0){}
		else
			UART_Transmit((uint8_t*)"Unknown Command\r\n", 20);
	}
}
