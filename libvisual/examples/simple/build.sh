#!/bin/sh

gcc -o vistest libvisual.c `pkg-config --libs --cflags sdl` `pkg-config --libs --cflags libvisual-0.5`
