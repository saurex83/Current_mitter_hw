/*******************************************************************************
 Порты ввода/вывода

 Минимально необходимые функции для работы с выводами.
 Использую их при отладке.
 
 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        21 мая 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ******************************************************************************/
#pragma once

#include <stm32l1xx_gpio.h>

hal_retcode hal_gpio_init(void);

typedef uint16_t pin_t;

// Используемые выводы на плате
typedef enum pinID_t 
{
	pinTP_1 = GPIO_Pin_4,
	pinTP_2 = GPIO_Pin_5, 
	pinTP_3 = GPIO_Pin_6, 
	pinTP_4 = GPIO_Pin_7, 
	pinTP_5 = GPIO_Pin_8,
	pinTP_All = (pinTP_1|pinTP_2|pinTP_3|pinTP_4|pinTP_5)
} pinID_t;

// Действия над выводом
typedef enum pinAction_t
{
	set,
	clear,
	toggle,
	pulse
} pinAction_t;

void hal_gpioState(pinID_t pin, pinAction_t action);