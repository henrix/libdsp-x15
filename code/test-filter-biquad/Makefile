TARGET = test-filter-biquad

CPP = g++
CPP_FLAGS = -g -Wall -std=c++11
LFLAGS = -Wl,-O1
LIBS = ../libdsp-x15/libdsp-x15.so -lOpenCL -locl_util

OBJECTS = main.o

%.o: %.cpp
	@echo Compiling $<
	@$(CPP) -c $(CPP_FLAGS) $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CPP) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean::
	@rm -f *.o *.o *.obj *.out *.asm *.if *.opt *.bc *.objc *.map *.bin *.dsp_h *.a
