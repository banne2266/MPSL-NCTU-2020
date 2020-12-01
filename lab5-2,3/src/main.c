#include "stm32l476xx.h"
//TODO: define your gpio pin
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

extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char
data);

int display(int data, int num_digs){
	if(data < 0){
		for(int i = 0; i < num_digs; i++){
			max7219_send(i+1, 15);
		}
		return -1;
	}

	int first = 0;
	int digit[num_digs];
	for(int i = 0; i < num_digs; i++){
		digit[i] = data % 10;
		data /= 10;
	}

	for(int i = num_digs; i > 0; i--){
		if(digit[i-1] != 0)
			first = 1;
		if(first == 1 || i == 1)
			max7219_send(i, digit[i-1]);
		else
			max7219_send(i, 15);
	}
	return 0;

};


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


/* TODO: reverse keypad io i to o, or o to i */
void reverse_keypad_io(int mode)
{
	if(mode == 0){//io exchange
		GPIOA->MODER = (GPIOA->MODER & 0xFCC0FFFF) | 0x0;
		GPIOB->MODER = (GPIOB->MODER & 0xFFF303FF) | 0x45400;
	}
	else if(mode == 1){//restore
		GPIOA->MODER = (GPIOA->MODER & 0xFCC0FFFF) | 0x1150000;
		GPIOB->MODER = (GPIOB->MODER & 0xFFF303FF) | 0x0;
	}
	return;
}


/* TODO: scan keypad value
return: >=0: key-value pressed¡A-1: keypad is free
*/
int keypad_scan(){
	int flag_keypad=GPIOB->IDR&10111<<5;
	int ans1 = -1, ans2 = -1;

	if(flag_keypad!=0){
		for(int i=0;i<4;i++){ //scan keypad from first column
			//set PA8,9,10,12(column) low and set pin high from PA8
			GPIOA->ODR= (GPIOA->ODR & 0xFFFFE8FF) | 1 << x_pin[i];
			for(int j=0;j<4;j++){ //read input from first row
				int flag_keypad_r = GPIOB->IDR&1<<y_pin[j];
				if(flag_keypad_r!=0){
					if(ans1 == -1)
						ans1 = Table[j][i];
					else{
						ans1 += Table[j][i];
					}
				}
			}
		}
		reverse_keypad_io(0);
		for(int i=0;i<4;i++){ //scan keypad from first column
			//set PB 5,6,7,9(column) low and set pin high from PA8
			GPIOB->ODR= (GPIOB->ODR & 0xFFFFFD1F) | 1 << y_pin[i];
			for(int j=0;j<4;j++){ //read input from first row
				int flag_keypad_r = GPIOA->IDR&1<<x_pin[j];
				if(flag_keypad_r!=0){
					if(ans2 == -1)
						ans2 = Table[i][j];
					else{
						ans2 += Table[i][j];
					}
				}
			}
		}
		reverse_keypad_io(1);
	}
	GPIOA->ODR = GPIOA->ODR|0b10111<<8;
	if(ans1 > ans2)
		return ans1;
	else
		return ans2;
}

int main()
{
	GPIO_init();
	max7219_init();
	keypad_init();
	while(1)
		display(keypad_scan(),4);
}
