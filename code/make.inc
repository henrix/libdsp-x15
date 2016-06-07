DSP_INCLUDE  = -I$(TI_OCL_CGT_INSTALL)/include 
DSP_INCLUDE += -I$(TARGET_ROOTDIR)/usr/share/ti/cgt-c6x/include
DSP_INCLUDE += -I$(TARGET_ROOTDIR)/usr/share/ti/opencl

CPP   = g++ 
CL6X  = cl6x -mv6600 --abi=eabi $(DSP_INCLUDE)
CLOCL = clocl

LIBS  = -lOpenCL -locl_util

UNAME_M :=$(shell uname -m)

# ----------------------------------------------------------------------------
# If TI_OCL_INSTALL is set, setup make from that location
# ----------------------------------------------------------------------------
ifneq ($(TI_OCL_INSTALL),)
    CPP        = g++
    CPP_FLAGS += -I$(TI_OCL_INSTALL)/usr/include
    LD_FLAGS  += -L$(TI_OCL_INSTALL)/usr/lib 
    LIBS      += -lbfd

# ----------------------------------------------------------------------------
# Otherwise, if making on x86 assume cross compile for Arm host
# ----------------------------------------------------------------------------
else ifneq (,$(findstring 86, $(UNAME_M)))
    .DEFAULT_GOAL := cross

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
    cross: override CPP = arm-linux-gnueabihf-g++ 
    cross: CPP_FLAGS += -I$(TARGET_ROOTDIR)/usr/include -idirafter /usr/include

    # If cross-compilineg, provide path to dependent ARM libraries on the 
    # target filesystem
    cross: LD_FLAGS = -L$(TARGET_ROOTDIR)/lib -L$(TARGET_ROOTDIR)/usr/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/usr/lib 
endif

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

cross:     $(EXE)

clean::
	@rm -f $(EXE) *.o *.obj *.out *.asm *.if *.opt *.bc *.objc *.map *.bin *.dsp_h

test: clean $(EXE)
	@echo Running   $(EXE)
	@./$(EXE) >> /dev/null
	@if [ $$? -ne 0 ] ; then echo "FAILED !!!" ; fi
