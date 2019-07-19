/********************************************************************************
 Описание структуры данных для передачи по последовательному интерфейсу.


Тактовая частота микроконтроллера 16 МГц.

 Проект:      Neocore Current Mitter                                                        
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        10 апреля 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ********************************************************************************/

#pragma once
#pragma pack(1)

#include "common_conf.h"
#include "stdbool.h"
#include "stdint.h"


typedef struct 
{
    uint8_t xor8; 
} s_head ;

#pragma pack(1)
typedef struct 
{
    uint8_t channel;
    uint16_t avr_value;
    uint16_t max_value; 
} s_chData;

#pragma pack(1)
typedef struct 
{ 
    float REF;        // Абсолютное значение опорного напряжения в вольтах.
    uint16_t adc_ref_val;// Измеренное значение оп. напр. АЦП.
} s_conf ;


#pragma pack(1)
typedef struct 
{
    s_head head;
    s_conf config;
    s_chData data[3];    // Данные измерения
} sPacket;


void sPacket_init(sPacket *packet);
void sPacket_calc_xor8(sPacket *packet);


