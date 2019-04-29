    /********************************************************************************
 Работа с отладочным последовательным интерфейсом
 Отладочный интерфейс uart2
 Выводы МК 12 (PA2) - Tx, 13(PA3) - RX


 Проект:      Neocore                                                         
 Автор:       Макшанов Олег Васильевич                                     
 Дата:        8 апреля 2019г                                               
 Версия:      0.1                                                          
 Компания:    ООО "ДиСиСи"                                                 
 mail:        pvp@dcconsult.ru 
 ********************************************************************************/

#include "hal/hal.h"
#include <stm32l1xx_usart.h>
#include <stm32l1xx_gpio.h>


#define UART_DBG_BAUD_RATE  115200


static inline hal_retcode uart_gpio_cfg(void);
static inline hal_retcode uart_conf(uint32_t BaudRate);


hal_retcode hal_uart_dbg_init(void)
{
    uart_gpio_cfg();
    uart_conf(UART_DBG_BAUD_RATE);
    return (hal_ok);   
}


hal_retcode hal_uart_dbg_put_char(char chr)
{
    while (USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, chr);

    return hal_ok;
}

hal_retcode hal_uart_dbg_send_array(char *arr, uint16_t sz)
{
    while (sz>0)
    {
        while (USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
        USART_SendData(USART2, *arr);
        sz--;
    }

    return hal_ok;
}

static inline hal_retcode uart_conf(uint32_t BaudRate)
{
    USART_InitTypeDef USART_InitStructure;

    // Разрешаем тактирование
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE); //Включаем UART
    return (hal_ok); 
}



static inline hal_retcode uart_gpio_cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    // Настройка портов
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);


    // Выбор альтернативной функции
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2 );
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2 );

    //Конфигурируем PA2 как альтернативную функцию -> TX UART. Подробнее об конфигурации можно почитать во втором уроке.
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    //Конфигурируем PA3 как альтернативную функцию -> RX UART. Подробнее об конфигурации можно почитать во втором уроке.
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    return (hal_ok);
}

