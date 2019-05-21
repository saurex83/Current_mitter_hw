/*******************************************************************************
 Порты ввода/вывода

 Все сделано по минимуму только для отладки прибора.
 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        21 мая 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ******************************************************************************/

#include <stm32l1xx_gpio.h>
#include "hal/hal.h"
#include "hal/hal_gpio.h"

#define TP_PORT 	GPIOB

hal_retcode hal_gpio_init(void);
void hal_gpioState(pinID_t pin, pinAction_t action);


void hal_gpioState(pinID_t pin, pinAction_t action)
{
	if (action == set)
	{
		GPIO_SetBits(TP_PORT, pin);
		return;
	} 
	else if (action == clear)
	{
		GPIO_ResetBits(TP_PORT, pin);
		return;
	}
	else if (action == toggle)
	{
		GPIO_ToggleBits(TP_PORT, pin);
		return;
	}
	else if (action == pulse)
	{
		GPIO_ToggleBits(TP_PORT, pin);
		GPIO_ToggleBits(TP_PORT, pin);
		return;
	}
}

hal_retcode hal_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_StructInit(&GPIO_InitStruct);
  	GPIO_InitStruct.GPIO_Pin  = pinTP_All;
  	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  
    GPIO_Init(TP_PORT, &GPIO_InitStruct);
    hal_gpioState(pinTP_All, clear);
    return hal_ok;
}
