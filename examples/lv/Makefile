


TARGET=lv
SOURCES= \
	display/display.c \
	display/niftyled_driver.c \
	lv.c

WARN_FLAGS=-Wall -g -O0
SDL_FLAGS=`sdl-config --libs --cflags`
NIFTYLED_FLAGS=`pkg-config --cflags --libs niftyled`
LV_FLAGS=`pkg-config --cflags --libs libvisual-0.5`


all:
	gcc $(WARN_FLAGS) -o $(TARGET) $(SOURCES) $(LV_FLAGS) $(SDL_FLAGS) $(NIFTYLED_FLAGS)
