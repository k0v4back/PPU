
#ifndef _MAIN_LOOP_H_
#define _MAIN_LOOP_H_

#include "main.h"


//определяем какой формат данных собираем 
#define PARSE_NON_FLOAT_VALUE
//#define PARSE_FLOAT_VALUE


//структура для приема данных из орбитрона
#ifdef PARSE_NON_FLOAT_VALUE
typedef struct 
{
	int azimuth;
	int elevation;
	
} orbitronDate_t;
#endif

#ifdef PARSE_FLOAT_VALUE
typedef struct 
{
	double azimuth;
	double elevation;
	
} orbitronDate_t;
#endif

//размер принимаемого буффера
#define ORBITRON_BUFFER_SIZE 13



#define AZ_START_COORDS 0
#define AZ_END_COORDS   350
#define EL_START_COORDS 35
#define EL_END_COORDS   165 // не точно( исходя из эксперимента)

#define AZ_SPEED 7 // горизонтальная скорость( 7 градусов в секунду)
#define EL_SPEED 2 // вертикальная скорость( 2 градуса в секунду)


void loop(void); // суперцикл программы
void recieve_data(void);
void show_recieve_data(void);
void send_to_rotator(void);
void ParseToDouble(void); 

#endif 
