/********************************************************************************
 Работа с АЦП.

 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        8 апреля 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ********************************************************************************/
#pragma once

#include <stm32l1xx_adc.h>
#include "kernel/data_packet.h"
#include "hal/hal.h"

#define AIN1    ADC_Channel_0
#define AIN2    ADC_Channel_1
#define AIN3    ADC_Channel_4

hal_retcode hal_adc_init(void);
hal_retcode hal_adc_vref(uint16_t *val);
hal_retcode hal_adc_cont_measurment(uint16_t *buf, uint16_t size, uint8_t ch, uint16_t usec);
hal_retcode hal_adc_scan_measurment(sPacket* spacket, uint16_t size,uint16_t usec);