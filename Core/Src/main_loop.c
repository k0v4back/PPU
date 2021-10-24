
#include "main_loop.h"
#include "lcd.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef hlpuart1;

//для приёма из орбитрона
volatile uint8_t RxBf[1] = {0}; // буфер для посимвольного приема из usart1
volatile uint8_t orbitron_buffer[ORBITRON_BUFFER_SIZE] = {0}; //массив с принятыми данными из орбитрона
volatile int orbitron_buffer_position; // индекс для массива, в который записываем приходящие данные


orbitronDate_t Coords; // экземпляр структуры для данных

uint8_t ptz_camera_txBuff[7];
uint8_t ptz_adress = 0x39;
uint8_t elevation_speed = 0x08;
uint8_t azimuth_speed = 0x08;


//флаг для прерывания
volatile _Bool usart1_interrupt_flag = 0;

//координаты ПУ и прочее 
float AZ_Rotator_Coords = 0;
float EL_Rotator_Coords = 35;
float moving_time_AZ = 0;
float moving_time_EL = 0;

typedef enum
{
	STATE_RECIEVE_DATA,
	STATE_SHOW_RECIEVE_DATA,
	STATE_SEND_DATA_TO_ROTATOR,
} State_t;

volatile State_t current_state = STATE_RECIEVE_DATA;

void loop()
{
	while(1) 
	{	
		
	switch(current_state) 
		{
		default: 
			
		case STATE_RECIEVE_DATA: recieve_data();
			break;
		case STATE_SHOW_RECIEVE_DATA: show_recieve_data();
			break;
		case STATE_SEND_DATA_TO_ROTATOR: send_to_rotator();
			break;
		}
	
	}
}

void recieve_data(void)
{
			__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
				
				if(usart1_interrupt_flag) 
				{
					orbitron_buffer[orbitron_buffer_position] = '!';
					
					ParseToDouble();
					for(int i = ORBITRON_BUFFER_SIZE; i!= 0; i--) orbitron_buffer[i] = 0;
					
					orbitron_buffer_position = 0;
					usart1_interrupt_flag = 0;
					__HAL_UART_DISABLE_IT(&huart1,UART_IT_RXNE);
					current_state = STATE_SHOW_RECIEVE_DATA;// переходим на следующее состояние
				}
}		

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	switch(RxBf[0])
	{
		case '!' : usart1_interrupt_flag = 1; break;
		default:	 orbitron_buffer[orbitron_buffer_position] = RxBf[0];
							 orbitron_buffer_position++;
	}
}

#ifdef PARSE_NON_FLOAT_VALUE
void show_recieve_data(void)
{
	char AZ_string[7];
	char EL_string[7];
	
	sprintf(AZ_string, " %d  ",Coords.azimuth);
	sprintf(EL_string, " %d  ",Coords.elevation);
	LCD_SetCursor(0,0);
	LCD_String("Obj|");
	LCD_String(AZ_string);
	LCD_String("|");
	LCD_String(EL_string);
	
	LCD_SetCursor(0,1);
	LCD_String("Rot|");
	
	//очищаем массивы символом для вывода конвертированых данных 
	for(int i = 7; i!= 0; i--)
	{
		AZ_string[i] = 0;
		EL_string[i] = 0;	
	}
	current_state = STATE_RECIEVE_DATA; // переходим на следующее состояние
}		
#endif

void send_to_rotator(void)
{

}


void SendToRotator(void) 
{
	float deltaAZ = 0;
	float deltaEL = 0;
	
	deltaAZ  = AZ_Rotator_Coords - Coords.azimuth;
	deltaEL  = EL_Rotator_Coords - Coords.elevation;
	
	moving_time_AZ = deltaAZ  / AZ_SPEED;
	moving_time_EL = deltaEL  / EL_SPEED;		
}


void PTZ_DATA_SEND(uint8_t adress, uint8_t cmd1, uint8_t cmd2, uint8_t data1, uint8_t data2)
{
	ptz_camera_txBuff[0] = 0xFF;
	ptz_camera_txBuff[1] = adress;
	ptz_camera_txBuff[2] = cmd1;
	ptz_camera_txBuff[3] = cmd2;
	ptz_camera_txBuff[4] = data1;
	ptz_camera_txBuff[5] = data2;
	ptz_camera_txBuff[6] = ptz_adress + cmd1 + cmd2 + data1 + data2;
	//USART1_SEND(ptz_camera_txBuff, sizeof(ptz_camera_txBuff));
	HAL_UART_Transmit(&hlpuart1,ptz_camera_txBuff, 7 ,0xFFF);
	
}


#ifdef PARSE_NON_FLOAT_VALUE
void ParseToDouble()  // формат принимаемых данных $[AZ] [EL]! 
{
	int buffer_position = 0; // индекс элемента для движения по буфферу
	
	Coords.azimuth = 0;
	Coords.elevation = 0;
	
	if (orbitron_buffer[0] == '$')
	{
		for (buffer_position = 1; orbitron_buffer[buffer_position] != '.'; buffer_position++);
		buffer_position--;
		
		//парсим и записываем значение в поле azimuth 
		for (int k = 1; orbitron_buffer[buffer_position] != '$'; buffer_position--, k *= 10) 
		{
			Coords.azimuth += (orbitron_buffer[buffer_position] - '0') * k;
		}

		for ( ;orbitron_buffer[buffer_position] != '!'; buffer_position++);
		buffer_position-=3;

		//парсим и записываем значение в поле elevation 
		for (int k = 1; orbitron_buffer[buffer_position] != ' ' ; buffer_position--, k *= 10) //считаем единицы, десятки, сотни значения
		{
			if (orbitron_buffer[buffer_position] == '-')
			{
				Coords.elevation *= -1;
				break;
			}
			Coords.elevation += (orbitron_buffer[buffer_position] - '0') * k;
		}
	}
}
#endif

#ifdef PARSE_FLOAT_VALUE
void ParseToDouble()  // формат принимаемых данных $[AZ] [EL]!
{
	int buffer_position = 0; // индекс элемента для движения по буфферу
	
	Coords.azimuth = 0;
	Coords.elevation = 0;
	
	if (orbitron_buffer[0] == '$')
	{
		for (buffer_position = 1; orbitron_buffer[buffer_position] != ' '; buffer_position++);
		buffer_position--;

		//парсим и записываем в поле azimuth значение
		Coords.azimuth += (double)(orbitron_buffer[buffer_position] - '0') / 10; //считаем десятичную часть AZ
		
		buffer_position -= 2;
		for (int k = 1; orbitron_buffer[buffer_position] != '$'; buffer_position--, k *= 10) //считаем единицы, десятки, сотни значения
		{
			Coords.azimuth += (orbitron_buffer[buffer_position] - '0') * k;
		}

		for ( ;orbitron_buffer[buffer_position] != '!'; buffer_position++);
		buffer_position--;

		//парсим и записываем в поле elevation значение
		Coords.elevation += (double)(orbitron_buffer[buffer_position] - '0') / 10; //считаем десятичную часть EL
		buffer_position -= 2;
		for (int k = 1; orbitron_buffer[buffer_position] != ' ' ; buffer_position--, k *= 10) //считаем единицы, десятки, сотни значения
		{
			if (orbitron_buffer[buffer_position] == '-')
			{
				Coords.elevation *= -1;
				break;
			}
			Coords.elevation += (orbitron_buffer[buffer_position] - '0') * k;
		}
	}
}
#endif
