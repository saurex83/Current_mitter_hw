/********************************************************************************
Работа с системным последовательным интерфейсом

 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        20 апреля 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ********************************************************************************/

#pragma once

#include "hal/hal.h"

hal_retcode hal_uart_sys_init(void);
hal_retcode hal_uart_sys_put_char(char chr);
hal_retcode hal_uart_sys_send_array(char *arr, uint16_t sz);
