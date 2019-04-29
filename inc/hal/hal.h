#include "stdint.h"
#include "stdbool.h"
#include "hal/hal_configuration.h"

#pragma once

typedef enum hal_retcode
{
    hal_ok = 0x00
} hal_retcode;

hal_retcode hal_init(void);