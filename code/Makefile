EXE = X15_lib_audio
TARGET_ROOTDIR = /opt/ti-processor-sdk-linux-am57xx-evm-02.00.02.11/targetNFS/
BIN = /opt/ti-processor-sdk-linux-am57xx-evm-02.00.02.11/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/

ifneq ($(MAKECMDGOALS),clean)
    ifeq ($(TARGET_ROOTDIR),)
        $(error Environment variable TARGET_ROOTDIR must be defined. Set it to point at the EVM root file system)
    endif
endif

DSP_INCLUDE  = -I$(TI_OCL_CGT_INSTALL)/include 
DSP_INCLUDE = -I$(TARGET_ROOTDIR)/usr/share/ti/cgt-c6x/include
DSP_INCLUDE += -I$(TARGET_ROOTDIR)/usr/share/ti/opencl

CPP = g++ 
CPP_FLAGS = -mfpu=neon -O3 -g
CL6X  = cl6x -mv6600 --abi=eabi $(DSP_INCLUDE)
CLOCL = clocl

LIBS  = -lOpenCL -locl_util -ljack 

UNAME_M :=$(shell uname -m)

# Compile on X15
ifneq ($(TI_OCL_INSTALL),)
    CPP        = g++
    CPP_FLAGS += -I$(TI_OCL_INSTALL)/usr/include
    LD_FLAGS  += -L$(TI_OCL_INSTALL)/usr/lib 
    LIBS      += -lbfd -

# Cross compile on x86
else ifneq (,$(findstring 86, $(UNAME_M)))
    .DEFAULT_GOAL := all
    CL6X  = $(BIN)/cl6x -mv6600 --abi=eabi $(DSP_INCLUDE)
	CLOCL = $(BIN)/clocl

    # In a cross compile environment we are assuming that the EVM file system
    # is located on the build host and necessary ARM libraries are installed
    # on that file system. 
    ifneq ($(MAKECMDGOALS),clean)
       ifeq ($(TARGET_ROOTDIR),)
         $(error Environment variable TARGET_ROOTDIR must be defined. Set it to point at the EVM root file system)
       endif
    endif

    # gcc ARM cross compiler will not, by default, search the host's
    # /usr/include.  Explicitly specify here to find dependent vendor headers
    all: override CPP = $(BIN)/arm-linux-gnueabihf-g++ 
    all: CPP_FLAGS += -I$(TARGET_ROOTDIR)/usr/include -idirafter /usr/include

    # If cross-compilineg, provide path to dependent ARM libraries on the 
    # target filesystem
    all: LD_FLAGS = -L$(TARGET_ROOTDIR)/lib -L$(TARGET_ROOTDIR)/usr/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/usr/lib 
endif

OBJ = JACKClient.o AudioAPI.o main.o

%.o: %.cpp
	@echo Compiling $<
	@$(CPP) -c $(CPP_FLAGS) $<

%.o: %.c
	@echo Compiling $<
	@$(CPP) -c $(CPP_FLAGS) $<

%.obj: %.c
	@echo Compiling $<
	@$(CL6X) -c $(CL6X_FLAGS) $<

%.out: %.cl
	@echo Compiling $< 
	@$(CLOCL) $(CLOCL_FLAGS) $^

%.dsp_h: %.cl
	@echo Compiling $< 
	@$(CLOCL) -t $(CLOCL_FLAGS) $^

all: $(EXE)

$(EXE): $(OBJ) #audiokernel.out
	@$(CPP) $(CPP_FLAGS) -o $(EXE) $(OBJ) $(LD_FLAGS) $(LIBS)

clean::
	@rm -f $(EXE) *.o *.obj *.out *.asm *.if *.opt *.bc *.objc *.map *.bin *.dsp_h

test: clean $(EXE)
	@echo Running   $(EXE)
	@./$(EXE) >> /dev/null
	@if [ $$? -ne 0 ] ; then echo "FAILED !!!" ; fi	
