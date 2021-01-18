# Lab5 STM32 Keypad Scanning
#### 從此lab開始使用c環境開發

## Lab5-1: Max7219 displayer

| GPIO PORT | PC0 | PC1 | PC2 |
| -------- | -------- | -------- | -------- |
| Usage | MAX7219(CLK) | MAX7219(CS) | MAX7219(DIN) |
| Mode | output | output | output |
| PUPDR  | pull-up(default) | pull-up(default) | pull-up(default) |
| OSPEED  | HIGH | HIGH | HIGH |



## Lab5-2: KeypadScanning

## Lab5-3: multi buttons 處理多按鍵
多按鍵輸入時有時會因為等效電位的原因而無法讀到正確輸入。解法:正反各掃一遍

| GPIO PORT | PA8 | PA9 | PA10 | PA12 |
| -------- | -------- | -------- | -------- | -------- |
| Usage | KeypadI/O | KeypadI/O | KeypadI/O | KeypadI/O |
| Mode | input/output | input/output | input/output | input/output |
| PUPDR  | pull-up | pull-up | pull-up | PCpull-up2 |
| OSPEED  | medium  | medium  | medium  | medium  |

| GPIO PORT | PB5 | PB6 | PB7 | PB9 |
| -------- | -------- | -------- | -------- | -------- |
| Usage | KeypadI/O | KeypadI/O | KeypadI/O | KeypadI/O |
| Mode | input/output | input/output | input/output | input/output |
| PUPDR  | pull-up | pull-up | pull-up | PCpull-up2 |
| OSPEED  | medium  | medium  | medium  | medium  |



