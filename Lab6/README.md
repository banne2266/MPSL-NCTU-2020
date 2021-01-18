# Lab6 STM32 Clock and Timer

## Lab6-1: Modify System Clock(SYSCLK) and CPU Clock(HCLK)


## Lab6-2: Timer 計時器

| GPIO PORT | PC0 | PC1 | PC2 |
| -------- | -------- | -------- | -------- |
| Usage | MAX7219(CLK) | MAX7219(CS) | MAX7219(DIN) |
| Mode | output | output | output |
| PUPDR  | pull-up(default) | pull-up(default) | pull-up(default) |
| OSPEED  | HIGH | HIGH | HIGH |

### timer2
| TIM2->PSC | TIM2->ARR |
| -------- | -------- |
| 39999(0.01s) | TIME_SEC * 100 |

main function:使用pooling之方式去獲取clock之時間


## Lab6-3: Modify LED brightness 調整 LED 亮度

| GPIO PORT | PA5 |
| -------- | -------- |
| Usage | led |
| Mode | PWM output |
### timer2
| TIM2->PSC | TIM2->ARR | TIM_CR1_DIR |
| -------- | -------- | -------- |
| 119(333hz) | 100(333hz) | 0(count up) |

### timer2 cc1
| TIM_CCMR1_CC1S | TIM_CCMR1_OC1M | TIM2->CCR1 |
| -------- | -------- | -------- |
| 0(pwm output) | PWM mode | 50 |





