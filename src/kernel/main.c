/*******************************************************************************
 
Программа производит измерения трех выходов с токовых трансформаторов и выдачу
данных в канал uart. 
Данные выдаются в сыром виде в отсчетах АЦП 12 бит с правым выравниванием. 
Предварительную калибровку АЦП микроконтролер проводит по внешнему опорному 
источнику. Измерения представляют собой осцилограмму тока.
Частота дискретизации фиксированная.
 
Программа работает в цикличном режиме:
1 - калибровка АЦП по внешнему опорному источнику
2 - ожидание тригера синхронизации или истечения времени синхронизации по тригеру
3 - оцифровка аналогового сигнала на заданном канале
4 - выдача результатов стандартным пакетом в uart с добавлением контрольной суммы
5 - выбор следующего канала измерения
6 - переход к п.1

Расчет CRC-16 бит по алгоритму XOR и начальным значением 0xA17C


Тактовая частота микроконтроллера 16 МГц.

 Проект:      Neocore Current Mitter                                                        
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        14 марта 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ******************************************************************************/

#include "common_conf.h"
#include "hal/hal_sysclock.h"
#include "hal/hal.h"
#include "hal/hal_uart_dbg.h"
#include "hal/hal_uart_sys.h"
#include "hal/hal_adc.h"
#include "kernel/data_packet.h"

void measure_circle(uint8_t ch);

int main(void)
{


    hal_start_sysclock();
    hal_init();

    while (true)
    {
        measure_circle(AIN1);
        measure_circle(AIN2);
        measure_circle(AIN3);
    }

	return 0;
}


void measure_circle(uint8_t ch)
{
    sPacket spacket;
    char *p_sPacket = (char*)&spacket;

    // Инициализация структуры пакета
    sPacket_init(&spacket);

    // Измеряем значение vref 
    hal_adc_vref(&spacket.config.adc_ref_val);

    // Преобразуем номера каналов АЦП к порядковым номерам каналов
    // Программа на RPI не должна знать о каналах АЦП
    if (ch == AIN1)
        spacket.config.channel = 0;
    else if (ch == AIN2)
        spacket.config.channel = 1;
    else if (ch == AIN3)
        spacket.config.channel = 2;

    // Захватываем поток данных
    hal_adc_cont_measurment(spacket.data.val_array, COUNT_OF_DISCRETS, ch, ADC_PERIOD_US);

    // Расчитываем xor8
    sPacket_calc_xor8(&spacket);

    // Выгружаем данные в последовательный интерфейс
    hal_uart_sys_send_array(p_sPacket, sizeof(sPacket));
}