include meta/gmsl

rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

TARGET   = "Sonic 3.app/Contents/MacOS/Sonic 3"
OBJS     = main.o

SRC_C   = $(call rwildcard, source/, *.c)
SRC_CPP = $(call rwildcard, source/, *.cpp)
SRC_WAV = $(call rwildcard, source/, *.wav)

OBJ_DIRS := $(sort $(addprefix outMac/, $(dir $(SRC_C:source/%.c=%.o))) \
			$(addprefix outMac/, $(dir $(SRC_CPP:source/%.cpp=%.o))))

OBJS     := $(addprefix outMac/, $(SRC_C:source/%.c=%.o)) \
			$(addprefix outMac/, $(SRC_CPP:source/%.cpp=%.o))

all:
	@mkdir -p $(OBJ_DIRS)
	make sonicwin

debug: sonicmac_debug

test:
	@echo $(OBJS)
	@@rm -rf $(OBJS)
	@echo $(SRC_CPP:source/%.cpp=%.o)
	make all

clean:
	@@rm -r $(OBJS)
	make all

clear:
	@echo $(OBJS)
	@@rm -r $(OBJS)

sonicwin: $(OBJS)
	@mkdir -p $(OBJ_DIRS)
	@g++ $^ \
		-Wall -lz -std=c++11 \
		-F/Library/Frameworks/ \
		-F/System/Library/Frameworks/ \
		-I/Library/Frameworks/SDL2.frameworks/Headers/ \
		-I/Library/Frameworks/SDL2_image.framework/Headers/ \
		-I/Library/Frameworks/SDL2_mixer.framework/Headers/ \
		-I/System/Library/Frameworks/OpenGL.framework/Headers/ \
	 	-I/Users/Justin/Dropbox/sonic3/source \
		-framework SDL2 \
		-framework SDL2_image \
		-framework SDL2_mixer \
		-framework OpenGL \
		-framework Cocoa \
		-lopencv_core -lopencv_highgui \
		-o $(TARGET)
	"Sonic 3.app/Contents/MacOS/Sonic 3"

sonicmac_debug: $(OBJS)
	@mkdir -p $(OBJ_DIRS)
	@g++ $^ \
		-Wall -lz -std=c++11 \
		-F/Library/Frameworks/ \
		-F/System/Library/Frameworks/ \
		-I/Library/Frameworks/SDL2.frameworks/Headers/ \
		-I/Library/Frameworks/SDL2_image.framework/Headers/ \
		-I/Library/Frameworks/SDL2_mixer.framework/Headers/ \
		-I/System/Library/Frameworks/OpenGL.framework/Headers/ \
	 	-I/Users/Justin/Dropbox/sonic3/source \
		-framework SDL2 \
		-framework SDL2_image \
		-framework SDL2_mixer \
		-framework OpenGL \
		-framework Cocoa \
		-lopencv_core -lopencv_highgui \
		-o $(TARGET)
	lldb -f "Sonic 3.app/Contents/MacOS/Sonic 3"

$(OBJ_DIRS):
	mkdir -p $@

outMac/%.o: source/%.cpp
	@g++ -c \
		-Wall -std=c++11 \
		-F/Library/Frameworks/ \
		-F/System/Library/Frameworks/ \
		-F/usr/local/lib \
		-I/Library/Frameworks/SDL2.framework/Headers/ \
		-I/Library/Frameworks/SDL2_image.framework/Headers/ \
		-I/Library/Frameworks/SDL2_mixer.framework/Headers/ \
		-I/System/Library/Frameworks/OpenGL.framework/Headers/ \
	 	-I/Users/Justin/Dropbox/sonic3/source \
		-DPLATFORM_MAC \
		-DDEBUGGLE \
		-o "$@" "$<"

outMac/%.o: source/%.c
	@g++ -c \
		-Wall \
		-F/Library/Frameworks/ \
		-F/System/Library/Frameworks/ \
		-F/usr/local/lib \
		-I/Library/Frameworks/SDL2.framework/Headers/ \
		-I/Library/Frameworks/SDL2_image.framework/Headers/ \
		-I/Library/Frameworks/SDL2_mixer.framework/Headers/ \
		-I/System/Library/Frameworks/OpenGL.framework/Headers/ \
		-DPLATFORM_MAC \
		-DDEBUGGLE \
		-o "$@" "$<"
