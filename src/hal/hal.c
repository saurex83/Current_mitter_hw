/*******************************************************************************
 Слой аппаратных абстракций.



 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        14 марта 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ******************************************************************************/

#include "hal/hal.h"
#include "hal/hal_uart_dbg.h"
#include "hal/hal_uart_sys.h"
#include "hal/hal_gpio.h"
#include "hal/hal_adc.h"
#include "hal/hal_tim6.h"

//Базовая инициализация аппаратных модулей
hal_retcode hal_init(void)
{
    hal_uart_dbg_init();
    hal_uart_sys_init();
    hal_tim6_init();    
    hal_adc_init();
    hal_gpio_init();
    return hal_ok;
}


/*
Холодный коридор : 17 - 23 
Горячий от : 29 - 40 
Влажность от 40-60
Мощность линия А,B: не более 7квт в сумме
Шкала бар в нутри стойки для энергии.
Сдеть дробные части.
Цвет символа с температурами такой же как правого меню.
Подключить правые штуку.
Шкала мощности 150. уровень 110 +-15
Левые нижнии 3 стойки отключить. они ИБП.
Температура под фальш полом на графиках и узлов к в холодном коридоре.
и температуру узла.
Альфа каналы в dglux для сведения температурных полей
*/