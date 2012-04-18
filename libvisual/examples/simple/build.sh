#!/bin/sh

g++ -o vistest libvisual.cpp `pkg-config --libs --cflags sdl` `pkg-config --libs --cflags libvisual-0.5`
