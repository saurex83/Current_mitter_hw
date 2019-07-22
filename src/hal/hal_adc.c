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
#include <misc.h>
#include <stm32l1xx_dma.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include "hal/hal.h"
#include "hal/hal_tim6.h"
#include "hal/hal_adc.h"
#include "hal/hal_gpio.h"


#define SAMPLE_TIME ADC_SampleTime_96Cycles 
#define ADCR        ADC_Channel_8 // Канал ИОН
#define ADCRank1     1       //Ранг канала
#define ADCRank2     2       //Ранг канала
#define ADCRank3     3       //Ранг канала

static inline hal_retcode adc_init(void);
static inline hal_retcode adc_gpio_cfg(void);
static void adc_dma_enable(uint16_t *buf, uint16_t buf_size);
static void adc_dma_disable(void);

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

    ADC_RegularChannelConfig(ADC1, ADCR, ADCRank1, SAMPLE_TIME); 

    ADC_Cmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);

    ADC_SoftwareStartConv(ADC1);

    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    *val = ADC_GetConversionValue(ADC1);
    ADC_Cmd(ADC1, DISABLE);
    return (hal_ok);
}

//Проводим count измерений на группе каналов AIN1-3, с интервалом usec 
hal_retcode hal_adc_scan_measurment(sPacket* spacket, uint16_t size,uint16_t usec)
{
    ADC_InitTypeDef ADC_IS;
    ADC_StructInit(&ADC_IS);
    ADC_IS.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; 
    ADC_IS.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T6_TRGO;
    ADC_IS.ADC_NbrOfConversion = 3;
    ADC_IS.ADC_ScanConvMode = ENABLE; 
    ADC_IS.ADC_ContinuousConvMode = DISABLE;
    ADC_Init(ADC1, &ADC_IS); 

    // Настраиваем регулярные каналы АЦП и указываем их ранг измерений
    ADC_RegularChannelConfig(ADC1, AIN1, ADCRank1, SAMPLE_TIME); 
    ADC_RegularChannelConfig(ADC1, AIN2, ADCRank2, SAMPLE_TIME); 
    ADC_RegularChannelConfig(ADC1, AIN3, ADCRank3, SAMPLE_TIME); 

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    // Устанавливаем период дискретизации
    hal_tim6_set(usec);

    uint16_t adcData[3] = {10,10,10}; // измеренные значения из каналов
    uint32_t avrAdc[3] = {0,0,0}; // Расчетные среднии значения
    uint16_t maxAdc[3] = {0,0,0}; // Пиковые значения тока
    uint16_t d_size = size;

    /* Полезная информация при работе с DMA 
        12.11 Data management and overrun detection
        Относиться к команде:
        ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
        АЦП перестает посылать запросы по завершения scan всех регулярок.
        Команда разрешает посылать запросы после scan. 
    */

    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
    

    adc_dma_enable(adcData, 3);

    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);

    // Цикл измерения
    hal_tim6_start();

    while (size > 0)
    {
        while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
        
        DMA_ClearFlag(DMA1_FLAG_TC1);
      //  DMA_ClearFlag( DMA1_FLAG_GL1 );
    //    DMA_ClearITPendingBit( DMA1_IT_GL1 );
     //   ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
        size--;

       // TODO для отладки подергать ногу
        hal_gpioState(pinTP_All, pulse);

        for (uint8_t i=0;i<3;i++)
        {
            avrAdc[i] += adcData[i];
            if (adcData[i] > maxAdc[i])
                maxAdc[i] = adcData[i];
        }
    }
    
    hal_tim6_stop();

    ADC_Cmd(ADC1, DISABLE);
    adc_dma_disable();

    /* Из за импульсного источника питания появляются всплески напряжения
    Они не превышают 3х младших разрядов АЦП и на среднее значение тока не 
    влияют. Вляют на максимальное значение. Поэтому все максимальные значения
    ниже 3х разрядом приравниваю к 0, что бы не было вопросов. 
    */
    for (uint8_t i = 0; i < 3; i++)
    {
        if (maxAdc[i] < 10)
            maxAdc[i] = 0;
    }

    for (uint8_t i = 0; i < 3; i++)
    {
        spacket->data[i].channel = (i+1); // Сразу приводим к каналам 1,2,3
        spacket->data[i].avr_value = avrAdc[i] / d_size;
        spacket->data[i].max_value = maxAdc[i];
    }
    return (hal_ok);
}


static void adc_dma_disable(void)
{
    DMA_Cmd(DMA1_Channel1, DISABLE);
    ADC_DMACmd(ADC1, DISABLE);
}

static void adc_dma_enable(uint16_t *buf, uint16_t buf_size)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_InitTypeDef DMA_InitStructure;
    //==Configure DMA1 - Channel1==
    DMA_DeInit(DMA1_Channel1); 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR); 
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) buf; 
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = buf_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

hal_retcode hal_adc_cont_measurment(uint16_t *buf, uint16_t size, uint8_t ch, uint16_t usec) 
{
    ADC_InitTypeDef ADC_IS;
    ADC_StructInit(&ADC_IS);
    ADC_IS.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; 
    ADC_IS.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T6_TRGO;
    ADC_Init(ADC1, &ADC_IS);

    ADC_RegularChannelConfig(ADC1, ch, ADCRank1, SAMPLE_TIME); 

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