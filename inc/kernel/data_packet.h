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
    uint32_t crc32; 
} s_head ;

typedef struct 
{ 
    float       REF;        // Абсолютное значение опорного напряжения в вольтах.
    uint16_t    adc_ref_val;// Измеренное значение оп. напр. АЦП.
    uint8_t     channel;    // Номер канала с которого произведены измерения
    uint16_t    period_us;  // Период дискретизации в мкс
    uint16_t    val_count;  // Количество измерений
} s_conf ;

typedef struct 
{
    uint16_t    val_array[COUNT_OF_DISCRETS]; // Массив измеренных данных
} s_data ;

typedef struct 
{
    s_head head;
    s_conf config;
    s_data data;
} sPacket;


void sPacket_init(sPacket *packet);
void sPacket_calc_crc32(sPacket *packet);


