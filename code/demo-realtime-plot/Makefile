TARGET = demo-realtime-plot

CPP = g++
CPP_FLAGS = -g -Wall -std=c++11
LFLAGS = -Wl,-O1
LIBS  = -lSDL -ljack ../libdsp-x15/libdsp-x15.so
# -L/usr/lib/arm-linux-gnueabihf -lSDL2
#sdl_cflags := $(shell pkg-config --cflags sdl2 SDL2_mixer)
#CPP_FLAGS += $(sdl_flags)
#sdl_libs := $(shell pkg-config --libs sdl2 SDL2_mixer)
#LIBS += $(sdl_libs)

OBJECTS = jackclient.o display.o main.o

%.o: %.cpp
	@echo Compiling $<
	@$(CPP) -c $(CPP_FLAGS) $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CPP) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean::
	@rm -f *.o *.o *.obj *.out *.asm *.if *.opt *.bc *.objc *.map *.bin *.dsp_h *.a
