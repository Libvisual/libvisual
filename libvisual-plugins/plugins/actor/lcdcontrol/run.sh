#!/bin/sh

valgrind --track-origins=yes --leak-check=full `which lv-tool` -a lcdcontrol -x jess:gdkpixbuf:oinksie:lv_flower:dancingparticles:gforce --dimension 512x128 -m checkers

