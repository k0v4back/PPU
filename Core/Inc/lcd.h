#ifndef LCD_H_
#define LCD_H_
//------------------------------------------------
#include "main.h"
//------------------------------------------------
#define d4_set() HAL_GPIO_WritePin(LCD_Data_Port, LCD_D4_Pin, GPIO_PIN_SET)
#define d5_set() HAL_GPIO_WritePin(LCD_Data_Port, LCD_D5_Pin, GPIO_PIN_SET)
#define d6_set() HAL_GPIO_WritePin(LCD_Data_Port, LCD_D6_Pin, GPIO_PIN_SET)
#define d7_set() HAL_GPIO_WritePin(LCD_Data_Port, LCD_D7_Pin, GPIO_PIN_SET)
#define d4_reset() HAL_GPIO_WritePin(LCD_Data_Port, LCD_D4_Pin, GPIO_PIN_RESET)
#define d5_reset() HAL_GPIO_WritePin(LCD_Data_Port, LCD_D5_Pin, GPIO_PIN_RESET)
#define d6_reset() HAL_GPIO_WritePin(LCD_Data_Port, LCD_D6_Pin, GPIO_PIN_RESET)
#define d7_reset() HAL_GPIO_WritePin(LCD_Data_Port, LCD_D7_Pin, GPIO_PIN_RESET)
#define e1 HAL_GPIO_WritePin(LCD_BL_EN_Port, LCD_Enable_Pin, GPIO_PIN_SET)  //��������� ����� � � 1
#define e0 HAL_GPIO_WritePin(LCD_BL_EN_Port, LCD_Enable_Pin, GPIO_PIN_RESET) //��������� ����� � � 0
#define rs1 HAL_GPIO_WritePin(LCD_Data_Port, LCD_RS_Pin, GPIO_PIN_SET) //��������� ����� RS � 1
#define rs0 HAL_GPIO_WritePin(LCD_Data_Port, LCD_RS_Pin, GPIO_PIN_RESET) //��������� ����� RS � 0
//------------------------------------------------
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SendChar(char ch);
void LCD_String(char* st);
void LCD_SetCursor(uint8_t x, uint8_t y);
//------------------------------------------------
#endif /* LCD_H_ */
