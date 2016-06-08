EXE       = audio-dsp-lib
CPP_FLAGS = -O3

include ./make.inc

$(EXE): main.o AudioAPI.o
	@$(CPP) $(CPP_FLAGS) $< $(LD_FLAGS) $(LIBS) -o $@
