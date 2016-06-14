EXE = X15_lib_audio
TARGET_ROOTDIR = /opt/ti-processor-sdk-linux-am57xx-evm-02.00.02.11/targetNFS/
BIN = /opt/ti-processor-sdk-linux-am57xx-evm-02.00.02.11/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/

ifneq ($(MAKECMDGOALS),clean)
    ifeq ($(TARGET_ROOTDIR),)
        $(error Environment variable TARGET_ROOTDIR must be defined. Set it to point at the EVM root file system)
    endif
endif

CPP = $(BIN)/arm-linux-gnueabihf-g++ 
CPP_FLAGS = -O3 -I$(TARGET_ROOTDIR)/usr/include -idirafter /usr/include
LD_FLAGS = -L$(TARGET_ROOTDIR)/lib -L$(TARGET_ROOTDIR)/usr/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/usr/lib

DSP_INCLUDE = -I$(TARGET_ROOTDIR)/usr/share/ti/cgt-c6x/include
DSP_INCLUDE += -I$(TARGET_ROOTDIR)/usr/share/ti/opencl

CL6X  = $(BIN)/cl6x -mv6600 --abi=eabi $(DSP_INCLUDE)
CLOCL = $(BIN)/clocl

LIBS  = -lOpenCL -locl_util

UNAME_M :=$(shell uname -m)

OBJ = main.o AudioAPI.o 

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

$(EXE): $(OBJ) audiokernel.out
	@$(CPP) $(CPP_FLAGS) -o $(EXE) $(OBJ) $(LD_FLAGS) $(LIBS)

clean::
	@rm -f $(EXE) *.o *.obj *.out *.asm *.if *.opt *.bc *.objc *.map *.bin *.dsp_h

test: clean $(EXE)
	@echo Running   $(EXE)
	@./$(EXE) >> /dev/null
	@if [ $$? -ne 0 ] ; then echo "FAILED !!!" ; fi	
