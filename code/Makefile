EXE       = audio_dsp_lib
CPP_FLAGS = -O3

DSP_INCLUDE += -I$(TARGET_ROOTDIR)/usr/share/ti/cgt-c6x/include
DSP_INCLUDE += -I$(TARGET_ROOTDIR)/usr/share/ti/opencl

CPP   = g++ 
CL6X  = cl6x -mv6600 --abi=eabi $(DSP_INCLUDE)
CLOCL = clocl

LIBS  = -lOpenCL -locl_util

UNAME_M :=$(shell uname -m)

$(EXE): main.o AudioAPI.o
	@$(CPP) $(CPP_FLAGS) $< $(LD_FLAGS) $(LIBS) -o $@

.DEFAULT_GOAL := all

ifneq ($(MAKECMDGOALS),clean)
    ifeq ($(TARGET_ROOTDIR),)
        $(error Environment variable TARGET_ROOTDIR must be defined. Set it to point at the EVM root file system)
    endif
endif

all: override CPP = arm-linux-gnueabihf-g++ 
all: CPP_FLAGS += -I$(TARGET_ROOTDIR)/usr/include -idirafter /usr/include
all: LD_FLAGS = -L$(TARGET_ROOTDIR)/lib -L$(TARGET_ROOTDIR)/usr/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/usr/lib 

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

$(EXE):

all:     $(EXE)

clean::
	@rm -f $(EXE) *.o *.obj *.out *.asm *.if *.opt *.bc *.objc *.map *.bin *.dsp_h

test: clean $(EXE)
	@echo Running   $(EXE)
	@./$(EXE) >> /dev/null
	@if [ $$? -ne 0 ] ; then echo "FAILED !!!" ; fi
