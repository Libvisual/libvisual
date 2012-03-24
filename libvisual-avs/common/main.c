#include <stdlib.h>

#include <libvisual/libvisual.h>

#include "lvavs_pipeline.h"
#include "lvavs_preset.h"

int main (int argc, char *argv[])
{	
	LVAVSPreset *preset = lvavs_preset_new_from_preset("/home/scott/Work/libvisual/branches/libvisual-avs/testpresets/simple.pip");

	return 0;
}

