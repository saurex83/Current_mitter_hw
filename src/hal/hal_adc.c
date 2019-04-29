/********************************************************************************
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

 Источником тактового сигнала для АЦП является генератор HSI.
 Частота 16 МГц. TODO RCC_ADCCLKConfig(RCC_PCLK2_Div4) тут должен быть делитель.

 Источником сигнала запуска АЦП является таймер.

 Общее время преобразования вычисляется следующим образом:
 Tconv = Sampling time + 12.5 cycles



 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        8 апреля 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ********************************************************************************/

#include <stm32l1xx_adc.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include "hal/hal.h"
#include "hal/hal_tim6.h"
#include "hal/hal_adc.h"

#define SAMPLE_TIME ADC_SampleTime_96Cycles 
#define ADCR    ADC_Channel_5 // К этому каналу подключен источник опорного напряжения
#define ADCRank 1   //Ранг канала


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

    ADC_RegularChannelConfig(ADC1, ADCR, ADCRank, SAMPLE_TIME); 
    ADC_SoftwareStartConv(ADC1);
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    *val = ADC_GetConversionValue(ADC1);

    return (hal_ok);
}

hal_retcode hal_adc_cont_measurment(uint16_t *buf, uint16_t size, uint8_t ch, uint16_t usec) 
{
    ADC_InitTypeDef ADC_InitStructure;

    // Переводим АЦП в режим непрерывных измерений с запуском по внешнему тригеру 
    // от таймера 6
    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;       // Выбираем одноканальный режим работы
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // Выбираем однократный режим запуска
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; //Запуск по событию от таймера 6
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T6_TRGO;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1; 
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ch, ADCRank, SAMPLE_TIME); 

    // Устанавливаем период дискретизации
    hal_tim6_set(usec);
    
    // Запускаем таймер 6.
    hal_tim6_start();

    while (size > 0)
    {
        // Ожидаем завершения преобразования
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        // TODO для отладки подергать ногу
        *buf = ADC_GetConversionValue(ADC1);
        buf++;
        size--;
    }

    // Останов таймера
    hal_tim6_stop();

    // Возвращаем режим работы АЦП по програмному запуску
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //Не используем тригер запуска

    return (hal_ok);
}

static inline hal_retcode adc_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    // Разрешаем тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // Перезапускаем АЦП
    ADC_DeInit(ADC1);

    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;       // Выбираем одноканальный режим работы
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // Выбираем однократный режим преобразования
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //Не используем тригер запуска
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1; 
    ADC_Init(ADC1, &ADC_InitStructure);

    //* ADC1 regular  configuration */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_96Cycles);      
    ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_None);

    //ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);
  
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Wait until ADC1 ON status */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);

    return hal_ok;
}

static inline hal_retcode adc_gpio_cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    // Настройка портов
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    return hal_ok;
}