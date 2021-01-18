# Lab4 MAX7219 7-Seg LED

## Lab4-1: Practice of Max7219 and 7-Seg LED with no-decode mode

| GPIO PORT | PC0 | PC1 | PC2 |
| -------- | -------- | -------- | -------- |
| Usage | MAX7219(CLK) | MAX7219(CS) | MAX7219(DIN) |
| Mode | output | output | output |
| PUPDR  | pull-up(default) | pull-up(default) | pull-up(default) |
| OSPEED  | HIGH | HIGH | HIGH |

### MAX7219 setting

| shutdown register | decode mode | intensity | scan limit |
| -------- | -------- | -------- | -------- |
| normal mode(0X1) | no decode(0X00) | max on(0X0F) | 0 only(#0X00) |


## Lab4-2: Practice of Max7219 and 7-Seg LED with code B decode mode

### MAX7219 setting

| shutdown register | decode mode | intensity | scan limit |
| -------- | -------- | -------- | -------- |
| normal mode(0X1) | CODE B decode(0XFF) | max on(0X0F) | 7(#0X06) |


## Lab4-3: Show the Fibonacci number

### MAX7219 setting

| shutdown register | decode mode | intensity | scan limit |
| -------- | -------- | -------- | -------- |
| normal mode(0X1) | CODE B decode(0XFF) | max on(0X0F) | 7(#0X06) |




