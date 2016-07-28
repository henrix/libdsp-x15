TARGET_LIB = libdsp-x15.so
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

LIBS  = -lOpenCL -locl_util
#LIBS += -lbfd

CL6X  = cl6x -mv6600 --abi=eabi $(DSP_INCLUDE)
CLOCL = clocl
CPP = g++
CPP_FLAGS = -fPIC -O3 -g -Wall -std=c++11
CPP_FLAGS += -I$(TI_OCL_INSTALL)/usr/include
LD_FLAGS  += -L$(TI_OCL_INSTALL)/usr/lib

OBJECTS = CallbackResponse.o API.o

UNAME_M :=$(shell uname -m)

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

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJECTS) #audiokernel.out
	$(CPP) -shared -Wall $(OBJECTS) $(LD_FLAGS) -o $(TARGET_LIB) $(LIBS)

clean::
	@rm -f *.o *.o *.obj *.out *.asm *.if *.opt *.bc *.objc *.map *.bin *.dsp_h *.a

test: clean $(TARGET_LIB)
	@echo Running   $(TARGET_LIB)
	@./$(TARGET_LIB) >> /dev/null
	@if [ $$? -ne 0 ] ; then echo "FAILED !!!" ; fi
