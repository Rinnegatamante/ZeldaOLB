TARGET		:= Zelda
SOURCES		:= src src/vita
INCLUDES	:= include

LIBS = -lvita2d -lvitashaders -lSceLibKernel_stub -lSceCtrl_stub -lSceTouch_stub \
	-lm -lScePgf_stub -ljpeg -lfreetype -lc -lScePower_stub -lSceCommonDialog_stub -lpng16 -lz \
	-lSceSysmodule_stub -lSceGxm_stub -lSceDisplay_stub -lSceAppUtil_stub \
	-lvorbisfile -lvorbis -logg -lspeexdsp -lSceAudio_stub -lSceIofilemgr_stub

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
OBJS     := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = -fno-lto -g -Wl,-q -O3 -D_PSP2_NPDRMPACKAGE_H_ -DHAVE_LIBSPEEXDSP \
			-DWANT_FMMIDI=1 -DUSE_AUDIO_RESAMPLER -DHAVE_OGGVORBIS
CXXFLAGS  = $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11 -fpermissive -findirect-inlining
ASFLAGS = $(CFLAGS)

all: $(TARGET).velf

%.velf: %.elf
	vita-mksfoex -s TITLE_ID=ZELDAOLB1 "Zelda: Oni Link Begins" param.sfo
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@
	vita-make-fself -s $@ eboot.bin
	
$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS)
