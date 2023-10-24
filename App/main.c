/*
 * main.c
 *
 *  Created on: Oct 17, 2023
 *      Author: Hazim Emad
 */
#include "../Library/STD_types.h"
#include "../Library/Bit_Math.h"
#include "../MCAL/DIO/Header/DIO_Interface.h"
#include "../HAL/LCD/Header/LCD_Interface.h"
#include "../HAL/Keypad/Header/Keypad_Interface.h"
#include "util/delay.h"
#include <stdlib.h>

#define MAX_SNAKE_LENGTH	80
#define STARTING_SIZE		3


void InitializeSnake();
void SpawnSnake();
void MoveSnake();
u8* FindShape(u8 currentDir, u8 nextDir);
void ResetPositionArr();
void UpdateSize();
void SpawnFood();
u8 Random(u8 lower, u8 upper);

enum{
	Right = 0,
	Left,
	Up,
	Down,
	RightLeft = 0,
	UpDown = 1,
	RightLeft_Down = 2,
	Up_RightLeft = 2,
	RightLeft_Up = 3,
	Down_RightLeft = 3,
};

typedef struct{
	u8 currentDirection;
	u8 nextDirection;
	s8 posX;
	s8 posY;
	u8* shape;
}Snake;

u8 currentSize = STARTING_SIZE;
Snake snakeArr[MAX_SNAKE_LENGTH];
u8 key = NO_PRESSED_KEY;
u8 cgRamIndex = 0;
u8 endGame = 0;
u8 foodX = 0;
u8 foodY = 0;
u8 foodAvailable = 0;
u8 Head[4][8] = {
		{//Right
				0b00000,
				0b00000,
				0b11110,
				0b11001,
				0b11110,
				0b00000,
				0b00000,
				0b00000
		},
		{//Left
				0b00000,
				0b00000,
				0b01111,
				0b10011,
				0b01111,
				0b00000,
				0b00000,
				0b00000
		},
		{//Up
				0b00100,
				0b01010,
				0b01010,
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01110
		},
		{//Down
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01010,
				0b01010,
				0b00100
		}
};

u8 Body[4][8] = {
		{//Right/Left
				0b00000,
				0b00000,
				0b01110,
				0b01110,
				0b01110,
				0b00000,
				0b00000,
				0b00000
		},
		{//Up/Down
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01110
		},
		{//Right/Left -> Down or Up -> Right/Left
				0b00000,
				0b00000,
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01110
		},
		{//Right/Left -> Up or Down -> Right/Left
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b01110,
				0b00000,
				0b00000,
				0b00000
		}
};

u8 posArr[4][20] ={{0}};

int main(){
	DIO_voidInit();
	DIO_voidSetPortValue(DIO_PortC, 0xFF);
	LCD_voidInit();
	InitializeSnake();
	SpawnSnake();
	_delay_ms(100);
	while(1){
		if(foodAvailable == 0){
			foodAvailable = 1;
			SpawnFood();
			DIO_voidSetPinValue(DIO_PortC, DIO_PIN6, High);
		}
		key = KPD_u8GetPressedKey();
		if(key != NO_PRESSED_KEY){
			if(endGame == 0){
				if(key == 8 && snakeArr[0].currentDirection != Down){
					snakeArr[0].nextDirection = Up;
				}
				else if(key == 4 && snakeArr[0].currentDirection != Right){
					snakeArr[0].nextDirection = Left;
				}
				else if(key == 6 && snakeArr[0].currentDirection != Left){
					snakeArr[0].nextDirection = Right;
				}
				else if(key == 2 && snakeArr[0].currentDirection != Up){
					snakeArr[0].nextDirection = Down;
				}
			}
			if(key == 'C'){
				LCD_voidSendCommand(LCD_ClearDisplay);
				endGame = 0;
				currentSize = STARTING_SIZE;
				ResetPositionArr();
				InitializeSnake();
				SpawnSnake();
				_delay_ms(300);
				DIO_voidSetPinValue(DIO_PortC, DIO_PIN6, Low);
			}
		}
		LCD_voidSendCommand(LCD_ClearDisplay);
		if(endGame == 0){
			LCD_voidGoToPosition(foodX,foodY);
			LCD_voidWriteChar('o');
			MoveSnake();
			UpdateSize();
			if(posArr[snakeArr[0].posX][snakeArr[0].posY] == 1){
				endGame = 1;
				continue;
			}
		}
		else{
			endGame = 0;
			LCD_voidGoToPosition(1,5);
			DIO_voidSetPinValue(DIO_PortC, DIO_PIN6, High);
			LCD_voidWriteString((u8*)"Game Over!");
			_delay_ms(300);
			DIO_voidSetPinValue(DIO_PortC, DIO_PIN6, Low);
			_delay_ms(4700);
			LCD_voidSendCommand(LCD_ClearDisplay);
			currentSize = STARTING_SIZE;
			ResetPositionArr();
			InitializeSnake();
			SpawnSnake();
			foodAvailable = 0;
		}
		_delay_ms(300);
		DIO_voidSetPinValue(DIO_PortC, DIO_PIN6, Low);
	}
	return 0;
}

void InitializeSnake(){
	s8 index = 0;
	for(index = 0; index < currentSize; index++){
		snakeArr[index].currentDirection = Right;
		snakeArr[index].nextDirection = Right;
		if(index == 0){
			snakeArr[index].shape = Head[Right];
		}
		else{
			snakeArr[index].shape = Body[Right];
		}
	}
}

void SpawnSnake(){
	s8 index = 0;
	u8 cgRamIndex = 1;
	for(index = 0; index < currentSize; index++){

		snakeArr[index].posX = 0;
		snakeArr[index].posY = 10 - index;

		if(index == 0){
			cgRamIndex = 1;
		}
		else if(index == currentSize - 1){
			cgRamIndex = 2;
		}
		else{
			cgRamIndex = 3;
		}
		LCD_voidDisplaySpecialChar(snakeArr[index].shape, cgRamIndex, snakeArr[index].posX, snakeArr[index].posY);
	}
}
void MoveSnake(){
	s8 index = 0;
	index = currentSize - 1;
	posArr[snakeArr[index].posX][snakeArr[index].posY] = 0;
	while(index >= 0){

		if(index != 0){
			snakeArr[index].currentDirection = snakeArr[index - 1].currentDirection;
			snakeArr[index].nextDirection = snakeArr[index - 1].nextDirection;
			snakeArr[index].posX = snakeArr[index - 1].posX;
			snakeArr[index].posY = snakeArr[index - 1].posY;
			snakeArr[index].shape = FindShape(snakeArr[index].currentDirection, snakeArr[index].nextDirection);
			posArr[snakeArr[index].posX][snakeArr[index].posY] = 1;
		}
		else{
			snakeArr[index].currentDirection = snakeArr[index].nextDirection;
			snakeArr[index].shape = Head[snakeArr[index].currentDirection];
			if(snakeArr[index].nextDirection == Down){
				snakeArr[index].posX++;
				cgRamIndex = 3;
			}
			else if(snakeArr[index].nextDirection == Up){
				snakeArr[index].posX--;
				cgRamIndex = 2;
			}
			else if(snakeArr[index].nextDirection == Right){
				snakeArr[index].posY++;
				cgRamIndex = 0;
			}
			else if(snakeArr[index].nextDirection == Left){
				snakeArr[index].posY--;
				cgRamIndex = 1;
			}
		}
		if(snakeArr[index].posX > 3){
			snakeArr[index].posX = 0;
		}
		else if(snakeArr[index].posX < 0){
			snakeArr[index].posX = 3;
		}

		if(snakeArr[index].posY > 19){
			snakeArr[index].posY = 0;
		}
		else if(snakeArr[index].posY < 0){
			snakeArr[index].posY = 19;
		}

		LCD_voidDisplaySpecialChar(snakeArr[index].shape, cgRamIndex, (u8)snakeArr[index].posX, (u8)snakeArr[index].posY);

		index--;
	}
}

u8* FindShape(u8 currentDir, u8 nextDir){
	u8 index = 0;
	switch(currentDir){
	case Right:
		if(nextDir == Right){
			index = RightLeft;
			cgRamIndex = 4;
		}
		else if(nextDir == Up){
			index = RightLeft_Up;
			cgRamIndex = 5;
		}
		else if(nextDir == Down){
			index =  RightLeft_Down;
			cgRamIndex = 6;
		}
		break;
	case Left:
		if(nextDir == Left){
			index = RightLeft;
			cgRamIndex = 4;
		}
		else if(nextDir == Up){
			index = RightLeft_Up;
			cgRamIndex = 5;
		}
		else if(nextDir == Down){
			index =  RightLeft_Down;
			cgRamIndex = 6;
		}
		break;
	case Up:
		if(nextDir == Right || nextDir == Left){
			index =  Up_RightLeft;
			cgRamIndex = 6;
		}
		else if(nextDir == Up){
			index =  UpDown;
			cgRamIndex = 7;
		}
		break;
	case Down:
		if(nextDir == Right || nextDir == Left){
			index =  Down_RightLeft;
			cgRamIndex = 5;
		}
		else if(nextDir == Down){
			index =  UpDown;
			cgRamIndex = 7;
		}
		break;
	}
	return Body[index];
}

void ResetPositionArr(){
	u8 x= 0;
	u8 y =0;
	for(x =0; x < 4; x++){
		for(y=0; y < 20; y++){
			posArr[x][y] = 0;
		}
	}
}

void SpawnFood(){
	u8 oldfoodX = foodX;
	u8 oldfoodY = foodY;
	while(oldfoodX == foodX && oldfoodY == foodY){
		foodX = Random(0,3);
		foodY = Random(0,19);
	}
}

void UpdateSize(){
	if(snakeArr[0].posX == foodX && snakeArr[0].posY == foodY){
		currentSize++;
		foodAvailable = 0;
	}
}

u8 Random(u8 lower, u8 upper){
	u8 num = (rand() %  (upper - lower + 1)) + lower;
	return num;
}
