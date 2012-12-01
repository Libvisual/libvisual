/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "etoile.h"
#include <pthread.h>
#define BEAT_MAX	200

beatdetector detector;

beatdetector::beatdetector()
{
	filterpower=0;
	for(int i=0;i<NUM_BANDS;i++)
	{
		uint16_t f = i<128 ? (i-128) :0;
		f = f*f/100;
		f = f*f/30;
		filter[i] =f;
		//      cout << f<<endl;
		filterpower+=f;
	}
    freq = 0;
    variance = 0;
    loudness = 0;
    beat = 0;
	avloudness=0;
    uplimit = 0;
    downlimit = 0;
	state = 0;
    beatbase = 0;
    beatquiet = 0;
	for(int i = 0;i<10;i++)
		lastbeats[i]=0;
}
void beatdetector::learnbeat(float data[3][256])
{
	filterpower=0;
	for(int i = 0; i < NUM_BANDS; i++)
	{
		float y0, y1;
		uint16_t f = i<128 ? (i-128) :0;
		f = f*f/100;
		f = f*f/30;
		y0 =  data[0][i];
		y1 =  data[1][i];
		y0*=y0;
		y1*=y1;
		y0 = (int)fastsqrt(y0+y1);

		filter[i] = (filter[i]*48 + f + ((y0)-filter2[i]))/50 ;
		filterpower+=filter[i];
	}
}
void beatdetector::calc_loudness(float data[3][256])
{
	int    i;
	float y0, y1;
	float cloudness;
	cloudness = 0;
	for(i = 0 ; i < NUM_BANDS; i++)
	{
		y0 =  data[0][i];
		y1 =  data[1][i];
		y0*=y0;
		y1*=y1;
		y0 = (int)sqrtf(y0+y1);

		cloudness += y0 *  filter[i] /filterpower;
		filter2[i] = (filter2[i]*49 + (y0))/50 ;
	}
	loudness= cloudness;

}
int maxgloudness ;
int mingloudness ;
int avgloudness ;
int gloudnesses[200];
int curloudness = 0;
int gloudness=0;
void dp_render_freq(float spectrum[3][256])
{
	int i;

	for(i = 0; i < 32; i++) 
		heights[i] = (heights[i] + spectrum[2][i]) / 2;

	detector.calc_loudness(spectrum);
	if(detector.avloudness)
	{
		gloudness = 50 * detector.loudness / detector.avloudness;
		maxgloudness = 50 * detector.uplimit / detector.avloudness;
		mingloudness = 50 * detector.downlimit / detector.avloudness;
		avgloudness = 50 * detector.avloudness / detector.avloudness;

	}
	else
		gloudness = detector.loudness;


	detector.detect_beat();

	if(detector.beat)
	{
		beat=2;
		detector.learnbeat(spectrum);
		beatcnt++;

	}
	gloudnesses[curloudness] = gloudness;
	if(beat)
		gloudnesses[curloudness] = -gloudness;

	curloudness = (curloudness + 1)%200;

}

void beatdetector::detect_beat()
{
	beat = 0;
	avloudness = (avloudness * 19 + loudness)/20;

	if(uplimit < avloudness)
		uplimit = avloudness;
	else
		uplimit = (59*uplimit + avloudness)/60;

	if(downlimit > avloudness)
		downlimit = avloudness;
	else
		downlimit = (9*downlimit + avloudness)/10;

	if(loudness > uplimit)
	{
		uplimit = loudness;
		if(state == 0)
		{
			state = 1;
			beat = 1;
		}
	}
	if(loudness < downlimit)
	{
		downlimit = loudness;
		if(state == 1)
			state = 0;
	}

}
