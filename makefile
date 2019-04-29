# Имя проекта
#-------------------------------------------------------------------------------
TARGET  = template

# Дефайны
#-------------------------------------------------------------------------------
DEFINES += USE_STDPERIPH_DRIVER
DEFINES += STM32L1XX_MD
DEFINES += STM32L151xB

# Включение персональных настроек файлов 
#-------------------------------------------------------------------------------
MAKEFILE_INC += src/kernel/makefile
MAKEFILE_INC += stdperiph/src/makefile
MAKEFILE_INC += cmsis/makefile
MAKEFILE_INC += src/hal/makefile

include $(MAKEFILE_INC)

# Инструменты
#-------------------------------------------------------------------------------
AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
CP = arm-none-eabi-objcopy
SZ = arm-none-eabi-size
RM = rm

# Пути к CMSIS, StdPeriph Lib
#-------------------------------------------------------------------------------
CMSIS_PATH         = cmsis
STDPERIPH_INC_PATH = stdperiph/inc
STDPERIPH_SRC_PATH = stdperiph/src

# startup файл
#-------------------------------------------------------------------------------
STARTUP = startup/startup_stm32l1xx_md.s

# Пути поиска исходных файлов
#-------------------------------------------------------------------------------
SOURCEDIRS := src
SOURCEDIRS += src/kernel
SOURCEDIRS += src/hal
SOURCEDIRS += $(STDPERIPH_SRC_PATH)
SOURCEDIRS += $(CMSIS_PATH)

# 
OBJ_DIR = obj 
EXE_DIR = bin

# Пути поиска хидеров
#-------------------------------------------------------------------------------
INCLUDES += inc
INCLUDES += $(SOURCEDIRS) 
INCLUDES += $(CMSIS_PATH)
INCLUDES += $(STDPERIPH_INC_PATH)

# Библиотеки
#-------------------------------------------------------------------------------
LIBPATH +=
LIBS    +=

# Настройки оптимизации компилятора
#-------------------------------------------------------------------------------
DEFAULT_OPTIMIZATION += -Os

# Настройки компилятора
#-------------------------------------------------------------------------------
CFLAGS += -mthumb -mcpu=cortex-m3 # архитектура и система комманд
CFLAGS += -std=gnu99              # стандарт языка С
CFLAGS += -Wall -pedantic         # Выводить все предупреждения
CFLAGS += -ggdb                   # Генерировать отладочную информацию для gdb
CFLAGS += -fno-builtin
CFLAGS += -Wfatal-errors
CFLAGS += $(DEFAULT_OPTIMIZATION) 
CFLAGS += $(addprefix -I, $(INCLUDES))
CFLAGS += $(addprefix -D, $(DEFINES))

# Скрипт линкера
#-------------------------------------------------------------------------------
LDSCR_PATH = ld-scripts
LDSCRIPT   = stm32_flash.ld

# Настройки линкера
#-------------------------------------------------------------------------------
LDFLAGS += -nostartfiles
LDFLAGS += -mthumb 
LDFLAGS += -L$(LDSCR_PATH)
LDFLAGS += -T$(LDSCR_PATH)/$(LDSCRIPT)
LDFLAGS += $(addprefix -L, $(LIBPATH))
LDFLAGS += $(LIBS)

# Настройки ассемблера
#-------------------------------------------------------------------------------
AFLAGS += -alh
AFLAGS += -mapcs-32

# Список объектных файлов
#-------------------------------------------------------------------------------
OBJS += $(patsubst %.c, %.o, $(wildcard  $(addsuffix /*.c, $(SOURCEDIRS))))
#OBJS += $(addprefix $(STDPERIPH_SRC_PATH)/, $(addsuffix .o, $(STDPERIPH_SRC_PATH)))
OBJS += $(patsubst %.s, %.o, $(STARTUP))


# Пути поиска make
#-------------------------------------------------------------------------------
VPATH := $(SOURCEDIRS)

# Список файлов к удалению командой "make clean"
#-------------------------------------------------------------------------------
TOREMOVE += *.elf *.hex
TOREMOVE += $(addsuffix /*.o, $(SOURCEDIRS))
TOREMOVE += $(addsuffix /*.d, $(SOURCEDIRS))
TOREMOVE += $(STDPERIPH_SRC_PATH)/*.o
TOREMOVE += $(patsubst %.s, %.o, $(STARTUP))
TOREMOVE += $(TARGET)

# Собрать все
#-------------------------------------------------------------------------------
all: $(TARGET).hex size 

# Очистка
#-------------------------------------------------------------------------------
clean:
	@$(RM) -f $(TOREMOVE)  

# Создание .hex файла
#-------------------------------------------------------------------------------
$(TARGET).hex: $(TARGET).elf
	@$(CP) -Oihex $(EXE_DIR)/$(TARGET).elf $(EXE_DIR)/$(TARGET).hex
        
# Показываем размер
#-------------------------------------------------------------------------------
size:
	@echo "---------------------------------------------------"
	@$(SZ) $(EXE_DIR)/$(TARGET).elf

# Линковка
#------------------------------------------------------------------------------- 
$(TARGET).elf: $(OBJS)
	@$(LD) $(LDFLAGS) $^ -o $(EXE_DIR)/$@

OPT = $(notdir $@)

# Компиляция
#------------------------------------------------------------------------------- 
%.o: %.c
	@echo "\e[1;34mBuilding $<  user cflags:  $($(notdir $@))\e[0m"
	@$(CC) $(CFLAGS) $($(notdir $@)) -c $< -o $@
 
%.o: %.s
	@echo "\e[1;34mBuilding $<\e[0m"
	@echo "OPTIMIZATION: $(AFLAGS)"
	@$(AS) $(AFLAGS) -c $< -o $@ > /dev/null

# Сгенерированные gcc зависимости
#-------------------------------------------------------------------------------
include $(wildcart *.d)
#@echo "CFLAGS: $(CFLAGS)"
#@echo "OPTIMIZATION: $($(notdir $@))"
