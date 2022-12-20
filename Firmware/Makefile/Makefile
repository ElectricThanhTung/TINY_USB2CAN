
######################################
# target
######################################
TARGET = TINY_USB2CAN


######################################
# building variables
######################################
# debug build?
DEBUG = 0
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_EXE = $(TARGET)/Exe
BUILD_OBJ = $(TARGET)/Obj

######################################
# source
######################################
# C sources
C_SOURCES =  \
../Core/Src/main.c \
../Core/Src/stm32f0xx_it.c \
../Core/Src/stm32f0xx_hal_msp.c \
../Core/Src/system_stm32f0xx.c \
../USB2CAN/Src/can.c \
../USB2CAN/Src/usb_device.c \
../USB2CAN/Src/usb_ctrl_req.c \
../USB2CAN/Src/usb_desc.c \
../USB2CAN/Src/usb_to_can.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_can.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cortex.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dma.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_exti.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash_ex.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd_ex.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr_ex.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc_ex.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim_ex.c \
../Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usb.c \

# ASM sources
ASM_SOURCES =  \
startup_stm32f072xb.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m0

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F072xB


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-I../Core/Inc \
-I../USB2CAN/Inc \
-I../Drivers/STM32F0xx_HAL_Driver/Inc \
-I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy \
-I../Drivers/CMSIS/Device/ST/STM32F0xx/Include \
-I../Drivers/CMSIS/Include


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F072C8Tx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_EXE)/$(TARGET).elf $(BUILD_EXE)/$(TARGET).hex $(BUILD_EXE)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_OBJ)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_OBJ)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_OBJ)/%.o: %.c Makefile | $(BUILD_OBJ) 
	@echo Compiling: $<
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_OBJ)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_OBJ)/%.o: %.s Makefile | $(BUILD_OBJ)
	@echo Compiling: $<
	@$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_EXE)/$(TARGET).elf: $(OBJECTS) Makefile | $(BUILD_EXE)
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@$(SZ) $@

$(BUILD_EXE)/%.hex: $(BUILD_EXE)/%.elf | $(BUILD_EXE)
	@$(HEX) $< $@
	@echo Hex file: $@
	
$(BUILD_EXE)/%.bin: $(BUILD_EXE)/%.elf | $(BUILD_EXE)
	@$(BIN) $< $@
	@echo Bin file: $@

$(TARGET):
	@mkdir $@
	
$(BUILD_OBJ): $(TARGET)
	@mkdir $@

$(BUILD_EXE): $(TARGET)
	@mkdir $@

#######################################
# clean up
#######################################
clean:
	@-rm -fR $(TARGET)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***