/********************************************************************************
Таймер.
Таймер используется для тактирования АЦП по событий TRG0
Таймер настраиваем на 2 кГц (500 мкс).
Системная частота 16 Мгц, прескалером опускаем до 1 Мгц.
Период таймера по умолчанию 500 тактов

 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        8 апреля 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ********************************************************************************/


#include "hal/hal.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_rcc.h"


hal_retcode hal_tim6_init(void)
{
  // Тактируем таймер */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  TIM_TimeBaseInitTypeDef base_timer;
  TIM_TimeBaseStructInit(&base_timer);
  // Делитель учитывается как TIM_Prescaler + 1, поэтому отнимаем 1 
  base_timer.TIM_Prescaler = 16 - 1;
  base_timer.TIM_Period = 500;

  TIM_TimeBaseInit(TIM6, &base_timer);

  // Запрещаем прерывания
  TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);

  // Включаем TRGO
  TIM_SelectOutputTrigger(TIM6,TIM_TRGOSource_Update);

  return hal_ok;
}

hal_retcode hal_tim6_start(void)
{
  // Сбрасываем таймер
  TIM_SetCounter(TIM6, 0x00);
  // Включаем таймер
  TIM_Cmd(TIM6, ENABLE);

  return hal_ok;
}

hal_retcode hal_tim6_set(uint16_t usec)
{
  TIM_SetAutoreload(TIM6, usec);
  return hal_ok;
}

hal_retcode hal_tim6_stop(void)
{
  TIM_Cmd(TIM6, DISABLE);
  return hal_ok;
}