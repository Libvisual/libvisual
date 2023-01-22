#include <config.h>

#include <stdlib.h>
#include <stdio.h>

#include "renderer.h"

t_effect _inf_effects[] = {
	{
		.num_effect = 6,
		.x_curve = 1842944,
		.curve_color = 0,
		.curve_amplitude = 59,
		.spectral_amplitude = 14,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 82
	},
	{
		.num_effect = 6,
		.x_curve = 1824128,
		.curve_color = 0,
		.curve_amplitude = 59,
		.spectral_amplitude = 11,
		.spectral_color = 255,
		.mode_spectre = 1,
		.spectral_shift = 40
	},
	{
		.num_effect = 6,
		.x_curve = 1676608,
		.curve_color = 0,
		.curve_amplitude = 59,
		.spectral_amplitude = 29,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 0
	},
	{
		.num_effect = 6,
		.x_curve = 1635200,
		.curve_color = 0,
		.curve_amplitude = 59,
		.spectral_amplitude = 4,
		.spectral_color = 255,
		.mode_spectre = 3,
		.spectral_shift = 73
	},
	{
		.num_effect = 6,
		.x_curve = 1598592,
		.curve_color = 255,
		.curve_amplitude = 75,
		.spectral_amplitude = 4,
		.spectral_color = 0,
		.mode_spectre = 3,
		.spectral_shift = 113
	},
	{
		.num_effect = 5,
		.x_curve = 1545728,
		.curve_color = 255,
		.curve_amplitude = 59,
		.spectral_amplitude = 4,
		.spectral_color = 255,
		.mode_spectre = 1,
		.spectral_shift = 98
	},
	{
		.num_effect = 3,
		.x_curve = 1471744,
		.curve_color = 255,
		.curve_amplitude = 82,
		.spectral_amplitude = 41,
		.spectral_color = 0,
		.mode_spectre = 4,
		.spectral_shift = 397
	},
	{
		.num_effect = 4,
		.x_curve = 1466816,
		.curve_color = 255,
		.curve_amplitude = 82,
		.spectral_amplitude = 41,
		.spectral_color = 0,
		.mode_spectre = 4,
		.spectral_shift = 397
	},
	{
		.num_effect = 3,
		.x_curve = 1405824,
		.curve_color = 0,
		.curve_amplitude = 51,
		.spectral_amplitude = 36,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 0
	},
	{
		.num_effect = 4,
		.x_curve = 1360256,
		.curve_color = 0,
		.curve_amplitude = 51,
		.spectral_amplitude = 15,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 82
	},
	{
		.num_effect = 4,
		.x_curve = 1345600,
		.curve_color = 0,
		.curve_amplitude = 51,
		.spectral_amplitude = 9,
		.spectral_color = 255,
		.mode_spectre = 3,
		.spectral_shift = 61
	},
	{
		.num_effect = 3,
		.x_curve = 1265344,
		.curve_color = 0,
		.curve_amplitude = 51,
		.spectral_amplitude = 9,
		.spectral_color = 255,
		.mode_spectre = 1,
		.spectral_shift = 73
	},
	{
		.num_effect = 3,
		.x_curve = 1239040,
		.curve_color = 0,
		.curve_amplitude = 51,
		.spectral_amplitude = 9,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 76
	},
	{
		.num_effect = 3,
		.x_curve = 1108480,
		.curve_color = 0,
		.curve_amplitude = 51,
		.spectral_amplitude = 9,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 77
	},
	{
		.num_effect = 3,
		.x_curve = 1084736,
		.curve_color = 0,
		.curve_amplitude = 51,
		.spectral_amplitude = 6,
		.spectral_color = 255,
		.mode_spectre = 3,
		.spectral_shift = 63
	},
	{
		.num_effect = 3,
		.x_curve = 1000000,
		.curve_color = 0,
		.curve_amplitude = 51,
		.spectral_amplitude = 6,
		.spectral_color = 255,
		.mode_spectre = 2,
		.spectral_shift = 56
	},
	{
		.num_effect = 2,
		.x_curve = 814848,
		.curve_color = 0,
		.curve_amplitude = 51,
		.spectral_amplitude = 6,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 82
	},
	{
		.num_effect = 0,
		.x_curve = 715328,
		.curve_color = 255,
		.curve_amplitude = 51,
		.spectral_amplitude = 6,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 102
	},
	{
		.num_effect = 2,
		.x_curve = 706112,
		.curve_color = 255,
		.curve_amplitude = 51,
		.spectral_amplitude = 6,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 102
	},
	{
		.num_effect = 2,
		.x_curve = 657408,
		.curve_color = 255,
		.curve_amplitude = 58,
		.spectral_amplitude = 29,
		.spectral_color = 0,
		.mode_spectre = 4,
		.spectral_shift = 397
	},
	{
		.num_effect = 2,
		.x_curve = 655808,
		.curve_color = 255,
		.curve_amplitude = 58,
		.spectral_amplitude = 29,
		.spectral_color = 0,
		.mode_spectre = 4,
		.spectral_shift = 397
	},
	{
		.num_effect = 2,
		.x_curve = 612416,
		.curve_color = 0,
		.curve_amplitude = 84,
		.spectral_amplitude = 29,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 0
	},
	{
		.num_effect = 1,
		.x_curve = 566272,
		.curve_color = 0,
		.curve_amplitude = 84,
		.spectral_amplitude = 23,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 0
	},
	{
		.num_effect = 1,
		.x_curve = 516736,
		.curve_color = 255,
		.curve_amplitude = 84,
		.spectral_amplitude = 10,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 0
	},
	{
		.num_effect = 1,
		.x_curve = 495296,
		.curve_color = 255,
		.curve_amplitude = 84,
		.spectral_amplitude = 10,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 0
	},
	{
		.num_effect = 0,
		.x_curve = 375616,
		.curve_color = 255,
		.curve_amplitude = 84,
		.spectral_amplitude = 4,
		.spectral_color = 0,
		.mode_spectre = 3,
		.spectral_shift = 45
	},
	{
		.num_effect = 0,
		.x_curve = 297280,
		.curve_color = 0,
		.curve_amplitude = 0,
		.spectral_amplitude = 11,
		.spectral_color = 255,
		.mode_spectre = 3,
		.spectral_shift = 62
	},
	{
		.num_effect = 0,
		.x_curve = 253888,
		.curve_color = 0,
		.curve_amplitude = 0,
		.spectral_amplitude = 11,
		.spectral_color = 255,
		.mode_spectre = 4,
		.spectral_shift = 76
	},
	{
		.num_effect = 0,
		.x_curve = 151488,
		.curve_color = 0,
		.curve_amplitude = 0,
		.spectral_amplitude = 43,
		.spectral_color = 255,
		.mode_spectre = 0,
		.spectral_shift = 0
	},
};

void _inf_load_random_effect(InfinitePrivate *priv, t_effect *effect)
{
	const size_t effect_count = sizeof(_inf_effects) / sizeof(_inf_effects[0]);
	const int index = visual_random_context_int(priv->rcontext) % effect_count;
	*effect = _inf_effects[index];
}
