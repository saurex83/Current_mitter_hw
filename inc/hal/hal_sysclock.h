/********************************************************************************
 Модуль управления тактовой подсистемой микроконтроллера



 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        14 марта 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ********************************************************************************/
#include "hal/hal.h"

#pragma once

hal_retcode hal_start_sysclock(void);
hal_retcode hal_start_systick(void);
uint64_t hal_get_systick(void);