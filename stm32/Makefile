# ------------------------------------------------
# 	Copyright	2024 University of Washington
#	
#	This software was developed at the University of Washington using funds
# 	generously provided by the US Office of Naval Research, the National
#	Science Foundation, and NOAA.
#  
#  	This library is free software; you can redistribute it and/or modify it
#  	under the terms of the GNU Lesser General Public License as published by
#  	the Free Software Foundation; either version 2.1 of the License, or (at
#  	your option) any later version.
#  
#  	This library is distributed in the hope that it will be useful, but
#  	WITHOUT ANY WARRANTY; without even the implied warranty of
#  	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
#  	General Public License for more details.
#  
#  	You should have received a copy of the GNU Lesser General Public License
#  	along with this library; if not, write to the Free Software Foundation,
#  	Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
# 
# 	Author	Aadithya Prakash <aadu@uw.edue>
# ------------------------------------------------

SHELL=/bin/tcsh
.DEFAULT:
.SUFFIXES:
.PRECIOUS: 
.PHONY: force

# read definitions of global compilation switches
-include .makefile.defs

# directory definitions
incDir = ./Drivers/STM32L4xx_HAL_Driver/Inc#
libDir = ./lib#
halDir = ./Drivers/STM3L41xx_HAL_Driver/Src#
fatDir = ./Middlewares/Third_Party/FatFs/src#
srcDir = ./kernel#

Stm32_cFiles := $(wildcard $(halDir)/*.c)
Stm32_oFiles := $(subst .c,.o, $(Stm32_cFiles))
Stm32_dFiles := $(subst .c,.d, $(Stm32_cFiles))
Stm32_hFiles := $(notdir $(shell find ./Drivers -name \*.h))


FatFs_cFiles := $(shell find $(fatDir) -name \*.c)
FatFs_oFiles := $(subst .c,.o, $(FatFs_cFiles))
FatFs_dFiles := $(subst .c,.d, $(FatFs_cFiles))
FatFs_hFiles := $(notdir $(shell find $(fatDir) -name \*.h))

Kernel_cFiles := $(wildcard $(srcDir)/*.c)
Kernel_oFiles := $(subst .c,.o, $(Kernel_cFiles))
Kernel_dFiles := $(subst .c,.d, $(Kernel_cFiles))
Kernel_hFiles := $(notdir $(shell find ./Inc -name \*.h))

# default target
all: headers libstm32.a libkernel.a
	@echo Done making $@ ...

# clean both Stm32, FatFs, and Kernel library components
clean: clean-headers clean-stm32 clean-kernel clean-fatfs
	@echo Done making $@ ...

# clean the Stm32 library components
clean-stm32: force
	-rm -f $(Stm32_oFiles) $(Stm32_dFiles) $(libDir)/libstm32.a
	@echo Done making $@ ...

# clean the Stm32 library components
clean-fatfs: force
	-rm -f $(FatFs_oFiles) $(FatFs_dFiles) $(libDir)/libfatfs.a
	@echo Done making $@ ...

# clean the Kernel library components
clean-kernel: force
	-rm -f $(Kernel_oFiles) $(Kernel_dFiles) $(libDir)/libkernel.a
	-rm -f SW4STM32/startup_stm32l4r5xx.o
	@echo Done making $@ ...

# clean both Stm32 and Kernel library components and remove the lib directory
realclean: clean
	-rm -rf $(libDir) $(incDir) 
	@echo Done making $@ ...

# debugging target that lists components of both Stm32, FatFs, and Kernel libraries
test: force
	@echo libDir = $(libDir)
	@echo 
	@echo Stm32_cFiles = $(Stm32_cFiles)
	@echo Stm32_oFiles = $(Stm32_oFiles)
	@echo Stm32_dFiles = $(Stm32_dFiles)
	@echo Stm32_hFiles = $(Stm32_hFiles)
	@echo 
	@echo FatFs_cFiles = $(FatFs_cFiles)
	@echo FatFs_oFiles = $(FatFs_oFiles)
	@echo FatFs_dFiles = $(FatFs_dFiles)
	@echo FatFs_hFiles = $(FatFs_hFiles)
	@echo 
	@echo Kernel_cFiles = $(Kernel_cFiles)
	@echo Kernel_oFiles = $(Kernel_oFiles)
	@echo Kernel_dFiles = $(Kernel_dFiles)
	@echo Kernel_hFiles = $(Kernel_hFiles)

# assemble the pre-main() startup code
SW4STM32/%.o: SW4STM32/%.s 
	arm-none-eabi-as $(ARMFLAGS) $(DBG) -o $@ $<

# compile one component of the Kernel library
$(srcDir)/%.o: $(srcDir)/%.c
	-@rm -f $@
	arm-none-eabi-gcc $(ARMFLAGS) \
      $(STM32FLAGS) -DSTM32F103xG -DUSE_STM3210E_EVAL -I$(fatDir){,/drivers} \
      -I./Drivers/CMSIS/{,Device/ST/STM32L4xx/}Include -I./Drivers/STM32L4xx_HAL_Driver/Inc -I./Inc\
      $(CFLAGS) $(DBG) \
      -MMD -MP -MF"$(subst .o,.d,$@)" -MT"$@" -o $@ $<

# list the dependencies of the Kernel library
libkernel.a: $(libDir) $(libDir)/libkernel.a
	@echo Done making $@ ...

# create the Kernel library from object components
$(libDir)/libkernel.a: $(Kernel_oFiles) SW4STM32/startup_stm32l4r5xx.o
	arm-none-eabi-ar rvu $@ $?
	arm-none-eabi-ranlib $@
	@echo Done making $@ ...


# compile one component of the Stm32 library
$(halDir)/%.o: $(halDir)/%.c
	-@rm -f $@
	arm-none-eabi-gcc $(ARMFLAGS) \
      $(STM32FLAGS) -DUSE_HAL_DRIVER -DSTM32F103xG \
      -I./Drivers/CMSIS/{,Device/ST/STM32L4xx/}Include -I./Drivers/STM32L4xx_HAL_Driver/Inc -I./Inc \
      $(CFLAGS) -MMD -MP -MF"$(subst .o,.d,$@)" \
      -MT"$@" \
      -o $@ $<

# list the dependencies of the Stm32 library
libstm32.a: $(libDir) $(libDir)/libstm32.a
	@echo Done making $@ ...

# create the Stm32 library from object components
$(libDir)/libstm32.a: $(Stm32_oFiles)
	arm-none-eabi-ar rvu $@ $?
	arm-none-eabi-ranlib $@
	@echo Done making $@ ...


# compile one component of the FatFs library
$(fatDir)/%.o: $(fatDir)/%.c
	-@rm -f $@
	arm-none-eabi-gcc $(ARMFLAGS) \
      $(STM32FLAGS) -DUSE_HAL_DRIVER -DSTM32F103xG \
      -I./Inc -I./Drivers/STM32L4xx_HAL_Driver/Inc -I./Drivers/CMSIS/{,Device/ST/STM32L4xx/}Include \
      -I$(fatDir) -I./$(@D) \
      $(CFLAGS) $(DBG) -MMD -MP -MF"$(subst .o,.d,$@)" \
      -MT"$@" \
      -o $@ $<

# list the dependencies of the FatFs library
libfatfs.a: $(libDir) $(libDir)/libfatfs.a
	@echo Done making $@ ...

# create the FatFs library from object components
$(libDir)/libfatfs.a: $(FatFs_oFiles)
	arm-none-eabi-ar rvu $@ $?
	arm-none-eabi-ranlib $@
	@echo Done making $@ ...

# create the directory to contain the Stm32, FatFs, and Kernel libraries
$(libDir):
	mkdir $@


# manage creation of symbolic links to header files
headers: $(incDir) $(incDir)/Legacy \
	      $(addprefix $(incDir)/, $(Stm32_hFiles)) \
	      $(addprefix $(incDir)/, $(FatFs_hFiles)) \
	      $(addprefix $(incDir)/, $(Kernel_hFiles))
	@echo Done making $@ ...

# clean the header files from the include directory
clean-headers: force
	-rm -f $(incDir)/Legacy \
          $(addprefix $(incDir)/, $(Stm32_hFiles)) \
	       $(addprefix $(incDir)/, $(FatFs_hFiles)) \
	       $(addprefix $(incDir)/, $(Kernel_hFiles))
	@echo Done making $@ ...

# create the include directory
$(incDir):
	mkdir $@

# create a symbolic link to the legacy directory
$(incDir)/Legacy: 
	cd $(incDir); ln -s . Legacy

# pattern-target for include files
$(incDir)/%.h: ./Inc/%.h
	ln -st $(incDir) ../$<

# pattern-target for include files
$(incDir)/%.h: ./Drivers/CMSIS/Device/ST/STM32L4xx/Include/%.h
	ln -st $(incDir) ../$<

# pattern-target for include files
$(incDir)/%.h: ./Drivers/CMSIS/Include/%.h
	ln -st $(incDir) ../$<

# pattern-target for include files
$(incDir)/%.h: ./Drivers/STM32L4xx_HAL_Driver/Inc/%.h
	ln -st $(incDir) ../$<

# pattern-target for include files
$(incDir)/%.h: ./Drivers/STM32L4xx_HAL_Driver/Inc/Legacy/%.h
	ln -st $(incDir) ../$<

# pattern-target for include files
$(incDir)/%.h: ./Middlewares/Third_Party/FatFs/src/%.h
	ln -st $(incDir) ../$<

# pattern-target for include files
$(incDir)/%.h: ./Middlewares/Third_Party/FatFs/src/drivers/%.h
	ln -st $(incDir) ../$<

STM32F103ZGTx_FLASH.ld:
	ln -s SW4STM32/stm32/$@

