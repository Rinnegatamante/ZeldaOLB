TARGET		:= ZeldaOLB
SOURCES		:= src src/vita
INCLUDES	:= include

LIBS = -lSDL_gfx -lSDL_image -lSDL -lvitaGL -limgui -lSceLibKernel_stub -lSceCtrl_stub -lSceTouch_stub \
	-lm -lScePgf_stub -ljpeg -lfreetype -lc -lScePower_stub -lSceCommonDialog_stub -lpng16 -lz \
	-lSceSysmodule_stub -lSceGxm_stub -lSceDisplay_stub -lSceAppUtil_stub -lSceHid_stub \
	-lvorbisfile -lvorbis -logg -lspeexdsp -lSceAudio_stub -lSceIofilemgr_stub

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
OBJS     := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = -flto -g -Wl,-q -O2 -D_PSP2_NPDRMPACKAGE_H_ -DHAVE_LIBSPEEXDSP \
			-DWANT_FMMIDI=1 -DUSE_AUDIO_RESAMPLER -DHAVE_OGGVORBIS
CXXFLAGS  = $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)
	
all: $(TARGET).vpk

$(TARGET).vpk: $(TARGET).velf
	vita-mksfoex -s TITLE_ID=ZELDAOLB1 "Zelda: Oni Link Begins" param.sfo
	cp -f param.sfo sce_sys/param.sfo
	
	#------------ Comment this if you don't have 7zip ------------------
	7z a -tzip $(TARGET).vpk -r images/* map/* music/* sound/* sce_sys/* eboot.bin 
	#-------------------------------------------------------------------

%.velf: %.elf
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@
	vita-make-fself -s $@ eboot.bin
	
$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS) param.sfo $(TARGET).vpk
