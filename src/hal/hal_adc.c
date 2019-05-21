/*******************************************************************************
 Работа с АЦП.
 Использованны порты АЦП:
  - AIN0 (вывод PA0)
  - AIN1 (вывод PA1)
  - AIN4 (вывод PB9) 


 Основная частота аналогового сигнала 50 Гц, период 20мс.
 Ширину спектра прийму равной 1 кГц.
 Частота дискретизации 2 кГц. Перод дискретизации сигнала 500мкс.
 ADCSample time прийму равным 96.
 Тогда скорость преобразования АЦП составит 6.7815 мкс
 Разрешение АЦП 12 бит, 4096 отсчетов.
 Источником тактового сигнала для АЦП является генератор HSI.
 Частота 16 МГц. TODO RCC_ADCCLKConfig(RCC_PCLK2_Div4) тут должен быть делитель.

 Источником сигнала запуска АЦП является таймер.

 Общее время преобразования вычисляется следующим образом:
 Tconv = Sampling time + 12.5 cycles

 Особенности работы с ацп:
 При переключении режима запуска ацп(по тригеру или программно) приходится
 выключать АЦП. Если этого не делать, результаты измерения не верны.
 При переключении каналов, ацп выключается. (из документации этот факт не 
 очивиден).

 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        8 апреля 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ******************************************************************************/

#include <stm32l1xx_adc.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include "hal/hal.h"
#include "hal/hal_tim6.h"
#include "hal/hal_adc.h"
#include "hal/hal_gpio.h"

#define SAMPLE_TIME ADC_SampleTime_96Cycles 
#define ADCR        ADC_Channel_8 // Канал ИОН
#define ADCRank     1   //Ранг канала


static inline hal_retcode adc_init(void);
static inline hal_retcode adc_gpio_cfg(void);


hal_retcode hal_adc_init(void)
{
    adc_gpio_cfg();
    adc_init();
    return hal_ok;
}

// Производит измерение значения опорного напряжения 2.048В
hal_retcode hal_adc_vref(uint16_t *val)
{
    ADC_InitTypeDef ADC_IS;
    ADC_StructInit(&ADC_IS);
    ADC_IS.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; 
    ADC_Init(ADC1, &ADC_IS);

    ADC_RegularChannelConfig(ADC1, ADCR, ADCRank, SAMPLE_TIME); 

    ADC_Cmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);

    ADC_SoftwareStartConv(ADC1);

    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    *val = ADC_GetConversionValue(ADC1);
    ADC_Cmd(ADC1, DISABLE);
    return (hal_ok);
}

hal_retcode hal_adc_cont_measurment(uint16_t *buf, uint16_t size, uint8_t ch, uint16_t usec) 
{

    ADC_InitTypeDef ADC_IS;
    ADC_StructInit(&ADC_IS);
    ADC_IS.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; 
    ADC_IS.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T6_TRGO;
    ADC_Init(ADC1, &ADC_IS);

    ADC_RegularChannelConfig(ADC1, ch, ADCRank, SAMPLE_TIME); 

    // Устанавливаем период дискретизации
    hal_tim6_set(usec);
    
    ADC_Cmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);

    hal_tim6_start();

    while (size > 0)
    {
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        // TODO для отладки подергать ногу
        hal_gpioState(pinTP_All, pulse);

        *buf = ADC_GetConversionValue(ADC1);
        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
        buf++;
        size--;
    }

    hal_tim6_stop();
    ADC_Cmd(ADC1, DISABLE);
    return (hal_ok);
}

static inline hal_retcode adc_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    ADC_DeInit(ADC1);
    return hal_ok;
}

static inline hal_retcode adc_gpio_cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    return hal_ok;
}