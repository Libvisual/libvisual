#ifndef _OINK_OINKSIE_H
#define _OINK_OINKSIE_H

#include <time.h>

#include <libvisual/libvisual.h>

/* FIXME indent all likewise eventually */
typedef struct {
	float r;
	float g;
	float b;
	float r_cur;
	float g_cur;
	float b_cur;
} OinksiePalFade;

typedef struct {
	int pal_new;

	int fade_start;
	int fade_steps;
	int fade_stepsdone;
	int fade_poststeps;
	int fade_poststop;
} OinksiePalData;

typedef struct {
	int blurmode;
	int scopemode;
	int backgroundmode;

	int beatdots;

	int audiodebug;

	int palfunky;

	int scenenew;
} OinksieConfig;

typedef struct {
	int basssens;
	int tripplesens;

	int bass;
	int bassleft;
	int bassright;

	int tripple;
	int trippleleft;
	int trippleright;

	int highest;

	short freq[3][256];
	short pcm[3][512];

	int musicmood;
	char beat;
	int energy;

	int volume;
} OinksieAudio;

typedef struct {
	int scopestereo_enabled;
	int scopestereo_start;
	int scopestereo_space;
	float scopestereo_adder;

	/* shooting balls vars */
	int ball_enabled;
	int ball_xstart;
	int ball_ystart;
	int ball_distance;
	int ball_adder;
	int ball_spread;

	/* towards screen twisting whirling ball vars */
	int whirl_enabled;
	int whirl_dia;
	int whirl_diadec;
	int whirl_size;
	int whirl_sizeadd;
	int whirl_rot;
	int whirl_rotadder;

	/* turning circles vars */
	int circles_enabled;
	int circles_direction;
	int circles_nr;
	int circles_turn;

	/* flashing balls */
	int flashball_enabled;

	/* ball sine */
	int ballsine_enabled;
	int ballsine_scroll;
	int ballsine_rotate;
	int ballsine_infade;
	int ballsine_direction;

        /* floaters vars */
	int floaters_direction;
	int floaters_turn;

	int rotate;
} OinksieScene;

typedef struct {
	/* Major vars */
	uint8_t		*drawbuf;
	short			 pcmdata[2][512];

	/* Palette vars */
	OinksiePalFade		 pal_fades[256];
	OinksiePalData		 pal_data;	
	VisPalette		 pal_cur;
	VisPalette		 pal_old;
	int			 pal_startup;

	/* Screen vars */
	int			 screen_size;
	int			 screen_width;
	int			 screen_height;
	int			 screen_halfwidth;
	int			 screen_halfheight;
	int			 screen_xybiggest;
	int			 screen_xysmallest;

	time_t			 timing;
	time_t			 timing_prev;

	/* Config vars */
	OinksieConfig		 config;
	OinksieAudio		 audio;
	OinksieScene		 scene;
} OinksiePrivate;

void oinksie_init (OinksiePrivate *priv, int width, int height);
void oinksie_quit (OinksiePrivate *priv);

void oinksie_sample (OinksiePrivate *priv);

void oinksie_palette_change (OinksiePrivate *priv, uint8_t funky);

void oinksie_render (OinksiePrivate *priv);
int oinksie_size_set (OinksiePrivate *priv, int width, int height);

VisPalette *oinksie_palette_get (OinksiePrivate *priv);

#endif /* _OINK_OINKSIE_H */
