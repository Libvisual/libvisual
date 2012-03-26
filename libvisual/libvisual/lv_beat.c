/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Scott Sibley <scott@starlon.net>
 * Adapted from Winamp's AVS plugin. See below.
 *
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
/*
  LICENSE
  -------
Copyright 2005 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Nullsoft nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "config.h"
#include "lv_beat.h"
#include "lv_common.h"
#include "lv_time.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int beat_song_changed(VisBeat *beat);
void beat_insert_hist_step(VisBeat *beat, VisBeatType *t, clock_t TC, int type, int i);
void beat_double_beat(VisBeat *beat);
void beat_half_beat(VisBeat *beat);
int beat_TC_hist_step(VisBeat *beat, VisBeatType *t, int *_hdPos, clock_t TC, int type);
int beat_ready_to_learn(VisBeat *beat);
int beat_ready_to_guess(VisBeat *beat);
void beat_new_bpm(VisBeat *beat, int thisBpm);
int beat_get_bpm(VisBeat *beat);
void beat_calc_bpm(VisBeat *beat);
void beat_slider_step(VisBeat *beat, int Ctl, int *slide);
int beat_get_time_offset_since_start(VisBeat *beat, clock_t TC);

#define min(a, b) a < b ? a : b;
#define max(a, b) a > b ? a : b;

static int beat_dtor(VisObject *obj)
{
    VisBeat *beat = VISUAL_BEAT(obj);

    if(beat->txt != NULL)
        visual_mem_free(beat->txt);

    beat->txt = NULL;

    if(beat->TCHist != NULL)
        visual_mem_free(beat->TCHist);

    beat->TCHist = NULL;

    if(beat->smoother != NULL)
        visual_mem_free(beat->smoother);

    beat->smoother = NULL;

    if(beat->half_discriminated != NULL)
        visual_mem_free(beat->half_discriminated);

    beat->half_discriminated = NULL;

    if(beat->half_discriminated2 != NULL)
        visual_mem_free(beat->half_discriminated2);

    beat->half_discriminated2 = NULL;

    if(beat->adv != NULL)
        visual_object_unref(VISUAL_OBJECT(beat->adv));

    beat->adv = NULL;

    return TRUE;
}

VisBeat *visual_beat_new()
{
    VisBeat *beat = visual_mem_new0(VisBeat, 1);

    visual_object_initialize (VISUAL_OBJECT(beat), TRUE, beat_dtor);

    visual_beat_init(beat);

    return beat;
}

int visual_beat_init(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, -VISUAL_ERROR_BEAT_NULL);

    beat->TCUsed=0;
    beat->topConfidenceCount=0;
    beat->forceNewBeat=0;
    beat->hdPos=0;
    beat->hdPos2=0;
    beat->inSlide=0;
    beat->outSlide=0;
    beat->oldDisplayBpm=-1;
    beat->oldDisplayConfidence=-1;
    beat->oldInSlide=0;
    beat->oldOutSlide=0;
    beat->outInc = 1;
    beat->inInc = 1;
    beat->bpm = 0;
    beat->avg = 0;
    beat->avg2 = 0;
    beat->smPtr = 0;
    beat->smSize = 8;
    beat->offIMax = 8;
    beat->insertionCount = 0;
    beat->predictionLastTC = 0;
    beat->confidence = 0;
    beat->confidence1 = 0;
    beat->confidence2 = 0;
    beat->halfCount=0;
    beat->doubleCount=0;
    beat->TCHistSize = 8;
    beat->predictionBpm=0;
    beat->stickyConfidenceCount=0;
    beat->sticked=0;
    beat->oldsticked=-1;
    beat->new_song = TRUE;
    beat->cfg_smartbeat = FALSE;
    beat->cfg_smartbeatsticky = TRUE;
    beat->cfg_smartbeatresetnewsong = TRUE;
    beat->cfg_smartbeatonlysticky = FALSE;
    beat->lastTC = 0;
    beat->startTC = visual_time_get_now();//clock() / (float)CLOCKS_PER_SEC * 60000;
    beat->txt = visual_mem_malloc0(256);
    beat->TCHist = visual_mem_malloc0(beat->TCHistSize*sizeof(VisBeatType));
    beat->smoother = visual_mem_malloc0(beat->smSize * sizeof(int));
    beat->half_discriminated = visual_mem_malloc0(beat->TCHistSize*(sizeof(int)));
    beat->half_discriminated2 = visual_mem_malloc0(beat->TCHistSize*(sizeof(int)));
    beat->adv = visual_beat_adv_new();

	int x;
	for (x = 0; x < 256; x ++)
	{
		double a=log(x*60.0/255.0 + 1.0)/log(60.0);
		int t=(int)(a*255.0);
		if (t<0)t=0;
		if (t>255)t=255;
		beat->logtab[x]=(unsigned char )t;
	}

    return VISUAL_OK;
}

static int beat_adv_dtor(VisObject *obj)
{
    VisBeatAdv *adv = VISUAL_BEAT_ADV(obj);

    if(adv->beathistory != NULL)
        visual_mem_free(adv->beathistory);

    adv->beathistory = NULL;

    return TRUE;
}

VisBeatAdv *visual_beat_adv_new()
{
    VisBeatAdv *adv = visual_mem_new0(VisBeatAdv, 1);

    visual_object_initialize (VISUAL_OBJECT(adv), TRUE, beat_adv_dtor);

    visual_beat_adv_init(adv);

    return adv;
}

int visual_beat_adv_init(VisBeatAdv *adv)
{
    visual_return_val_if_fail(adv != NULL, -VISUAL_ERROR_BEAT_ADV_NULL);

    adv->beathistory = visual_mem_malloc0(BEAT_ADV_MAX * sizeof(int32_t));
    adv->aged = 0;
    adv->lowest = 0;
    adv->elapsed = 0;
    adv->isquiet = 0;
    adv->prevbeat = 0;
    adv->beatbase = 0;
    adv->beatquiet = 0;

    // defaults
    adv->cfg_sensitivity = 15;
    adv->cfg_max_detect = 200;
    adv->cfg_thick_on_beats = 0;

    visual_time_init(&adv->lastDetect);

    return VISUAL_OK;
}

int visual_beat_set_config(VisBeat *beat, int smartbeat, int smartbeatsticky, int smartbeatresetnewsong, int smartbeatonlysticky)
{
    visual_return_val_if_fail(beat != NULL, -VISUAL_ERROR_BEAT_NULL);

    beat->cfg_smartbeat = smartbeat;
    beat->cfg_smartbeatsticky = smartbeatsticky;
    beat->cfg_smartbeatresetnewsong = smartbeatresetnewsong;
    beat->cfg_smartbeatonlysticky = smartbeatonlysticky;

    return VISUAL_OK;
}

int visual_beat_set_smartbeat(VisBeat *beat, int smartbeat)
{
    visual_return_val_if_fail(beat != NULL, -VISUAL_ERROR_BEAT_NULL);

    beat->cfg_smartbeat = smartbeat;

    return VISUAL_OK;
}

int visual_beat_set_smartbeat_sticky(VisBeat *beat, int smartbeatsticky)
{
    visual_return_val_if_fail(beat != NULL, -VISUAL_ERROR_BEAT_NULL);

    beat->cfg_smartbeatsticky = smartbeatsticky;

    return VISUAL_OK;
}

int visual_beat_set_smartbeat_reset_on_newsong(VisBeat *beat, int smartbeatresetnewsong)
{
    visual_return_val_if_fail(beat != NULL, -VISUAL_ERROR_BEAT_NULL);

    beat->cfg_smartbeatresetnewsong = smartbeatresetnewsong;

    return VISUAL_OK;
}

int visual_beat_set_smartbeat_only_sticky(VisBeat *beat, int smartbeatonlysticky)
{
    visual_return_val_if_fail(beat != NULL, -VISUAL_ERROR_BEAT_NULL);

    beat->cfg_smartbeatonlysticky = smartbeatonlysticky;

    return VISUAL_OK;
}

int visual_beat_change_song(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, -VISUAL_ERROR_BEAT_NULL);

    beat->new_song = TRUE;

    return VISUAL_OK;
}

char *visual_beat_get_info(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, NULL);

    visual_return_val_if_fail(beat->txt != NULL, NULL);

    /*if(beat->oldDisplayBpm != beat->predictionBpm || beat->oldsticked != beat->sticked)
    {
        beat->oldDisplayBpm = beat->predictionBpm;
        beat->oldsticked = beat->sticked;
    }

    if(beat->oldDisplayConfidence != beat->confidence)
    {
        beat->oldDisplayConfidence = beat->confidence;
    }*/

    snprintf(beat->txt, 255, beat->predictionBpm ? "Current BPM: %d%s" : "Learning...", beat->predictionBpm, beat->cfg_smartbeatsticky && beat->sticked ? " Got it!": "");
    char *tmp = strdup(beat->txt);
    snprintf(beat->txt, 255, "%s -- Confidence: %d%%", tmp, beat->confidence);
    visual_mem_free(tmp);

    return beat->txt;
}

int visual_beat_reset_adapt(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, -VISUAL_ERROR_BEAT_NULL);

    // Reset adaptive learning
    beat->TCUsed=0;
    beat->hdPos=0;
    beat->avg = 0;
    beat->confidence=0;
    beat->confidence1=0;
    beat->confidence2=0;
    beat->topConfidenceCount=0;
    beat->bpm = 0;
    beat->smPtr = 0;
    beat->smSize = 8;
    beat->offIMax = 8;
    beat->insertionCount = 0;
    beat->predictionLastTC = 0;
    beat->halfCount=0;
    beat->doubleCount=0;
    beat->lastTC= visual_time_get_now();//clock() / (float)CLOCKS_PER_SEC * 60000;
    beat->TCHistSize = 8;
    beat->predictionBpm=0;
    beat->bestConfidence=0;
    beat->sticked=0;
    beat->oldsticked=-1;
    beat->stickyConfidenceCount=0;
    memset(beat->TCHist, 0, beat->TCHistSize*sizeof(VisBeatType));
    memset(beat->smoother, 0, beat->smSize*sizeof(int));
    memset(beat->half_discriminated, 0, beat->TCHistSize*sizeof(int));

    return VISUAL_OK;
}

int visual_beat_slider_get(VisBeat *beat, VisBeatSlider slider)
{
    visual_return_val_if_fail(beat != NULL, 0);

    return slider == VISUAL_BEAT_SLIDE_IN ? beat->inInc : beat->outInc;
}

int visual_beat_refine_beat(VisBeat *beat, int isBeat)
{
    visual_return_val_if_fail(beat != NULL, 0);

    int accepted=FALSE;
    int predicted=FALSE;
    int resyncin=FALSE;
    int resyncout=FALSE;
    VisTime now;
    clock_t TCNow;

    visual_time_init(&now);

    visual_time_get(&now);

    TCNow = visual_time_get_now();//clock() / (float)CLOCKS_PER_SEC * 60000;

    if (isBeat) // Show the beat received from AVS
        beat_slider_step(beat, VISUAL_BEAT_SLIDE_IN, &beat->inSlide);

    if (beat_song_changed(beat))
    {
        beat->bestConfidence=(int)((float)beat->bestConfidence*0.5);
        beat->sticked=0;
        beat->stickyConfidenceCount=0;
        if (beat->cfg_smartbeatresetnewsong)
            visual_beat_reset_adapt(beat);
    }

    // Try to predict if this frame should be a beat
    if (beat->bpm && TCNow > beat->predictionLastTC + (60000 / beat->bpm))
        predicted = TRUE;


    if (isBeat) // If it is a real beat, do discrimination/guessing and computations, then see if it is accepted
        accepted = beat_TC_hist_step(beat, beat->TCHist, &beat->hdPos, TCNow, BEAT_REAL);

    // Calculate current Bpm
    beat_calc_bpm(beat);

    // If prediction Bpm has not yet been set
    // or if prediction bpm is too high or too low
    // or if 3/4 of our history buffer contains beats within the range of typical drift
    // the accept the calculated Bpm as the new prediction Bpm
    // This allows keeping the beat going on when the music fades out, and readapt to the new beat as soon as
    // the music fades in again
    if ((accepted || predicted) && !beat->sticked && (!beat->predictionBpm || beat->predictionBpm > BEAT_MAX_BPM || beat->predictionBpm < BEAT_MIN_BPM))
    {
        if (beat->confidence >= beat->bestConfidence)
        {
            beat->forceNewBeat=1;
        }
        if (beat->confidence >= 50)
        {
            beat->topConfidenceCount++;
            if (beat->topConfidenceCount == BEAT_TOP_CONF_ADOPT)
            {
                beat->forceNewBeat=1;
                beat->topConfidenceCount=0;
            }
        }
        if (beat->forceNewBeat)
        {
            beat->forceNewBeat=0;
            beat->bestConfidence = beat->confidence;
            beat->predictionBpm=beat->bpm;
        }
    }

    if (!beat->sticked) beat->predictionBpm = beat->bpm;
    beat->bpm=beat->predictionBpm;


    if (beat->predictionBpm && accepted && !predicted)
    {
        int b;
        if (TCNow > beat->predictionLastTC + (60000 / beat->predictionBpm)*0.7)
        {
            resyncin = TRUE;
            b = (int)((float)beat->predictionBpm * 1.01);
        }
        if (TCNow < beat->predictionLastTC + (60000 / beat->predictionBpm)*0.3)
        {
            resyncout = TRUE;
            b = (int)((float)beat->predictionBpm * 0.98);
        }
        if (!beat->sticked && beat->doResyncBpm && (resyncin || resyncout))
        {
            beat_new_bpm(beat, b);
            beat->predictionBpm = beat_get_bpm(beat);
        }
    }

    if (resyncin)
    {
        beat->predictionLastTC = TCNow;
        beat_slider_step(beat, VISUAL_BEAT_SLIDE_OUT, &beat->outSlide);
        beat->doResyncBpm=TRUE;
        return ((beat->cfg_smartbeat && !beat->cfg_smartbeatonlysticky) ||
            (beat->cfg_smartbeat && beat->cfg_smartbeatonlysticky && beat->sticked)) ?
            1 : isBeat;
    }
    if (predicted)
    {
        beat->predictionLastTC = TCNow;
        if (beat->confidence > 25)
        //accepted = beat_TC_hist_step(beat, beat->TCHist, &beat->hdPos, TCNow, BEAT_REAL);
            beat_TC_hist_step(beat, beat->TCHist, &beat->hdPos, TCNow, BEAT_GUESSED);
        beat_slider_step(beat, VISUAL_BEAT_SLIDE_OUT, &beat->outSlide);
        beat->doResyncBpm=FALSE;
        return ((beat->cfg_smartbeat && !beat->cfg_smartbeatonlysticky) ||
            (beat->cfg_smartbeat && beat->cfg_smartbeatonlysticky && beat->sticked)) ?
            1 : isBeat;
    }
    if (resyncout)
    {
        beat->predictionLastTC = TCNow;
        beat->doResyncBpm=TRUE;
        return ((beat->cfg_smartbeat && !beat->cfg_smartbeatonlysticky) ||
            (beat->cfg_smartbeat && beat->cfg_smartbeatonlysticky && beat->sticked)) ?
            0 : isBeat;
    }

    return ((beat->cfg_smartbeat && !beat->cfg_smartbeatonlysticky) ||
        (beat->cfg_smartbeat && beat->cfg_smartbeatonlysticky && beat->sticked)) ?
        (beat->predictionBpm ? 0 : isBeat) : isBeat;

}

VisBeatPeak *visual_beat_get_peak(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, NULL);

    return &beat->peak;
}

VisBeatAdv *visual_beat_get_adv(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, NULL);

    return beat->adv;
}

int visual_beat_adv_set_config(VisBeatAdv *adv, int sensitivity, int max_bpm, int thick_on_beats)
{
    visual_return_val_if_fail(adv != NULL, -VISUAL_ERROR_BEAT_ADV_NULL);

    adv->cfg_sensitivity = sensitivity;
    adv->cfg_max_detect = max_bpm;
    adv->cfg_thick_on_beats = thick_on_beats;

    return VISUAL_OK;
}

int visual_beat_adv_set_sensitivity(VisBeatAdv *adv, int sensitivity)
{
    visual_return_val_if_fail(adv != NULL, -VISUAL_ERROR_BEAT_ADV_NULL);

    adv->cfg_sensitivity = sensitivity;

    return VISUAL_OK;
}

int visual_beat_adv_set_max_detect(VisBeatAdv *adv, int max_bpm)
{
    visual_return_val_if_fail(adv != NULL, -VISUAL_ERROR_BEAT_ADV_NULL);

    adv->cfg_max_detect = max_bpm;

    return VISUAL_OK;
}

int visual_beat_adv_set_thick_on_beats(VisBeatAdv *adv, int thick_on_beats)
{
    visual_return_val_if_fail(adv != NULL, -VISUAL_ERROR_BEAT_ADV_NULL);

    adv->cfg_thick_on_beats = thick_on_beats;

    return VISUAL_OK;
}

// The song changed or not. User sets flag with visual_beat_change_song()
int beat_song_changed(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, FALSE);

    if(beat->new_song)
    {
	beat->avg = 0;
        beat->new_song = FALSE;
        return TRUE;
    }
    return FALSE;
}

// Insert a beat in history table. May be either real beat or guessed
void beat_insert_hist_step(VisBeat *beat, VisBeatType *t, clock_t TC, int type, int i)
{
    visual_return_if_fail(beat != NULL);

    if (i >= beat->TCHistSize) return;
    if (beat->insertionCount < beat->TCHistSize*2)
        beat->insertionCount++;
    memmove(t+i+1, t+i, sizeof(VisBeatType)*(beat->TCHistSize-(i+1)));
    t[0].TC = TC;
    t[0].type = type;
}

// Doubles current beat
void beat_double_beat(VisBeat *beat)
{
    visual_return_if_fail(beat != NULL)

    int i;
    int iv[8];

    if (beat->sticked && beat->bpm > BEAT_MIN_BPM)
        return;

    for (i=0;i<beat->TCHistSize-1;i++)
        iv[i] = beat->TCHist[i].TC - beat->TCHist[i+1].TC;

    for (i=1;i<beat->TCHistSize;i++)
        beat->TCHist[i].TC = beat->TCHist[i-1].TC-iv[i-1]/2;

    beat->avg /= 2;
    beat->bpm *= 2;
    beat->doubleCount=0;
    memset(beat->smoother, 0, beat->smSize*sizeof(int));
    memset(beat->half_discriminated, 0, beat->TCHistSize*sizeof(int));
}

// Halfs current beat
void beat_half_beat(VisBeat *beat)
{
    visual_return_if_fail(beat != NULL);

    int i;
    int iv[8];

    if (beat->sticked && beat->bpm < BEAT_MIN_BPM) return;

    for (i=0;i<beat->TCHistSize-1;i++)
        iv[i] = beat->TCHist[i].TC - beat->TCHist[i+1].TC;

    for (i=1;i<beat->TCHistSize;i++)
        beat->TCHist[i].TC = beat->TCHist[i-1].TC-iv[i-1]*2;

    beat->avg *= 2;
    beat->bpm /= 2;
    beat->halfCount=0;
    memset(beat->smoother, 0, beat->smSize*sizeof(int));
    memset(beat->half_discriminated, 0, beat->TCHistSize*sizeof(int));
}

// Called whenever isBeat was true in render
//        accepted = beat_TC_hist_step(beat, beat->TCHist, &beat->hdPos, TCNow, BEAT_REAL);
//            beat_TC_hist_step(beat, beat->TCHist, &beat->hdPos, TCNow, BEAT_GUESSED);

int beat_TC_hist_step(VisBeat *beat, VisBeatType *t, int *_hdPos, clock_t TC, int type)
{
    visual_return_val_if_fail(beat != NULL, FALSE);

    int offI;
    clock_t thisLen;
    int learning = beat_ready_to_learn(beat);
    thisLen = TC - beat->lastTC;
    thisLen = thisLen>=0?thisLen:0;

    // If this beat is sooner than half the average - 20%, throw it away
    if (thisLen < beat->avg/2 - beat->avg*0.1)
    {
        if (learning)
        {
            if (abs(beat->avg - (TC - t[1].TC)) < abs(beat->avg - (t[0].TC - t[1].TC)))
            {
                printf("inside level 3\n");
                t->TC = TC;
                t->type = type;
                return TRUE;
            }
        }
        return FALSE;
    }
    if (learning)
      for (offI = 2; offI < beat->offIMax; offI++) // Try to see if this beat is in the middle of our current Bpm, or maybe 1/3, 1/4 etc... to offIMax
        if ((float)abs((beat->avg/offI)-thisLen) < (float)(beat->avg/offI)*0.2)
        {
            beat->half_discriminated[(*_hdPos)++]=1; // Should test if offI==2 before doing that, but seems to have better results ? I'll have to investigate this
            (*_hdPos)%=8;
            return FALSE;
        }

    // This beat is accepted, so set this discrimination entry to false
    beat->half_discriminated[beat->hdPos++]=0;
    (*_hdPos)%=8;

    // Remember this tick count
    beat->lastTC = TC + beat->startTC;

    // Insert this beat.
    beat_insert_hist_step(beat, t, TC, type, 0);
    return TRUE;
}

// Am i ready to learn ?
int beat_ready_to_learn(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, FALSE);

    int i;
    for (i=0; i < beat->TCHistSize;i++)
        if (beat->TCHist[i].TC==0)
            return FALSE;
    return TRUE;
}

// Am i ready to guess ?
int beat_ready_to_guess(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, FALSE);

    return beat->insertionCount == beat->TCHistSize*2;
}

void beat_new_bpm(VisBeat *beat, int thisBpm)
{
    visual_return_if_fail(beat != NULL);

    beat->smoother[beat->smPtr++] = thisBpm;
    beat->smPtr %= beat->smSize;
}

int beat_get_bpm(VisBeat *beat)
{
    visual_return_val_if_fail(beat != NULL, 0);

    int i;
    int smN=0;
    int smSum=0;
    // Calculate smoothed Bpm
    for (i=0;i<beat->smSize;i++)
        if (beat->smoother[i] > 0)
        {
            smSum += beat->smoother[i];
            smN++;
        }
    if (smN)
        return smSum / smN;
    return 0;
}

// Calculate BPM according to beat history
void beat_calc_bpm(VisBeat *beat)
{
    visual_return_if_fail(beat != NULL);

    int i;
    int hdCount=0;
    int r=0;
    int totalTC=0, totalN=0;
    float rC, etC;
    int v=0;
    double sc=0;
    int mx=0;
    float et;

    if (!beat_ready_to_learn(beat))
        return;

    // First calculate average beat
    for (i=0;i<beat->TCHistSize-1;i++)
        totalTC += beat->TCHist[i].TC - beat->TCHist[i+1].TC;

    beat->avg = totalTC/(beat->TCHistSize-1);

    // Count how many of then are real as opposed to guessed
    for (i=0;i<beat->TCHistSize;i++)
        if (beat->TCHist[i].type == BEAT_REAL)
            r++;

    // Calculate part 1 of confidence
    rC = (float)min((float)((float)r / (float)beat->TCHistSize) * 2, 1);

    // Calculate typical drift
    for (i=0;i<beat->TCHistSize-1;i++)
    {
        v = beat->TCHist[i].TC - beat->TCHist[i+1].TC;
        mx = max(mx, v);
        sc += v*v;
    }
    et = (float)sqrt(sc / (beat->TCHistSize-1) - beat->avg*beat->avg);
    // Calculate confidence based on typical drift and max derivation
    etC = 1 - ((float)et / (float)mx);

    // Calculate confidence
    beat->confidence = max(0, (int)(((rC * etC) * 100.0) - 50) * 2);
    beat->confidence1 = (int)(rC * 100);
    beat->confidence2 = (int)(etC * 100);

    // Now apply second layer, recalculate average using only beats within range of typical drift
    // Also, count how many of them we are keeping
    totalTC=0;
    for (i=0;i<beat->TCHistSize-1;i++)
    {
        v += beat->TCHist[i].TC - beat->TCHist[i+1].TC;
        if (abs(beat->avg-v) < et)
        {
            totalTC += v;
            totalN++;
            v = 0;
        }
        else
            if ((float)v > beat->avg)
                v = 0;
    }
    beat->TCUsed = totalN;
    // If no beat was within typical drift (how would it be possible? well lets cover our ass) then keep the simple
    // average calculated earlier, else recalculate average of beats within range
    if (totalN)
        beat->avg = totalTC/totalN;

    if (beat_ready_to_guess(beat))
    {
        if (beat->avg) // Avg = 0 ? Ahem..
            beat->bpm = 60000 / beat->avg;


        if (beat->bpm != beat->lastBPM)
        {
            beat_new_bpm(beat, beat->bpm); // If realtime Bpm has changed since last time, then insert it in the smoothing tab;e
            beat->lastBPM = beat->bpm;

            if (beat->cfg_smartbeatsticky && beat->predictionBpm && beat->confidence >= ((beat->predictionBpm < 90) ? BEAT_STICKY_THRESHOLD_LOW : BEAT_STICKY_THRESHOLD))
            {
                beat->stickyConfidenceCount++;
                if (beat->stickyConfidenceCount >= BEAT_MIN_STICKY)
                    beat->sticked=1;
            }
            else
                beat->stickyConfidenceCount=0;
        }

        beat->bpm = beat_get_bpm(beat);

        // Count how many beats we discriminated
        for (i=0;i<beat->TCHistSize;i++)
            if (beat->half_discriminated[i]) hdCount++;

        if (hdCount >= beat->TCHistSize/2) // If we removed at least half of our beats, then we are off course. We should double our bpm
        {
            if (beat->bpm * 2 < BEAT_MAX_BPM) // Lets do so only if the doubled bpm is < BEAT_MAX_BPM
            {
                beat_double_beat(beat);
                memset(beat->half_discriminated, 0, beat->TCHistSize*sizeof(int)); // Reset discrimination table
            }
        }
        if (beat->bpm > 500 || beat->bpm < 0)
        {
            visual_beat_reset_adapt(beat);
        }
        if (beat->bpm < BEAT_MIN_BPM)
        {
            if (++beat->doubleCount > 4) // We're going too slow, lets double our bpm
                beat_double_beat(beat);
        }
        else
            beat->doubleCount=0;

        if (beat->bpm > BEAT_MAX_BPM) // We're going too fast, lets slow our bpm by a factor of 2
        {
            if (++beat->halfCount > 4)
                beat_half_beat(beat);
        }
        else
            beat->halfCount=0;
    }
}

void beat_slider_step(VisBeat *beat, int Ctl, int *slide)
{
    visual_return_if_fail(beat != NULL);

    *slide += Ctl == VISUAL_BEAT_SLIDE_IN ? beat->inInc : beat->outInc;

    if (!*slide || *slide == 8) {
        if(Ctl == VISUAL_BEAT_SLIDE_IN) {
            beat->inInc *= -1;
        } else {
            beat->outInc *= -1;
        }
    }
}

