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
#include "kernel/data_packet.h"

void sPacket_init(sPacket *packet)
{
    packet->config.REF = REF_VOLT;
    packet->config.period_us = ADC_PERIOD_US;
    packet->config.val_count = COUNT_OF_DISCRETS;
}

// Вот не понял сам зачем название crc32 в uint16 и байтовое xor?
// Это результат моей тупости, но поправлять сейчас не буду
// Расчитывает CRC32 всего пакета и размещает в заголовке
void sPacket_calc_xor8(sPacket *packet)
{
    // При подсчете CRC учитывается только секция config и data
    uint16_t sz = sizeof(s_conf) + sizeof(s_data);
    
    uint8_t *ptr = (uint8_t*)&(packet->config); 

    uint8_t xor8 = 0;

    while ( sz > 0 )
    {
        xor8 ^= *ptr;
        ptr++;
        sz--;
    }

    packet->head.xor8 = xor8;
}