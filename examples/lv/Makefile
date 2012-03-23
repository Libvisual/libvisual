


TARGET="lv"
SOURCES="lv.c"

WARN_FLAGS="-Wall"
SDL_FLAGS=`sdl-config --libs --cflags`
LV_FLAGS=`pkg-config --cflags --libs libvisual-0.5`


all:
	gcc $(WARN_FLAGS) -o $(TARGET) $(SOURCES) $(LV_FLAGS) $(SDL_FLAGS)
