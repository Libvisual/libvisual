#include "actor_xpyre.h"
#include "draw.h"


/* TODO: Make this automatically configurable at compile time.  For now, edit
 * this #define to speed up the code a bit, or to make the colors weird. */

/* Determine byte ordering at runtime. */
#define BYTE_ORDERING 0

/* RGB color byte ordering. */
/* #define BYTE_ORDERING 1 */

/* BGR color byte ordering. */
/* #define BYTE_ORDERING 2 */

/* FIXME: no initializers allowed! */
static int  yscale;
static int  fade;
static int  title_state;
static int  title_pos;
static int  title_fade_amt;
static int  title_width;
static int  title_time;
static int  title_bg_ra;
static int  title_bg_ga;
static int  title_bg_ba;
static int* pcm_lookup;
static int* title_fade_lookup;
static int  title_fade_height;
static int  backup_height;
static BITMAP** bmp;
static int flip;
static int byte_ordering;

static void cleanup_graphics(void);

int draw_start(void)
{
    title_state = 0;
    title_pos = 5;
    title_fade_amt = 0;
    pcm_lookup = NULL;
    title_fade_lookup = NULL;
    backup_height = 0;
    bmp = NULL;
    flip = 1;

    /* Start the graphics stuff. */
    if (draw_set_gfx_mode() == 0) {
        /* Graphics failure. */
        return 0;
    }

    return 1;
}

void draw_stop(void)
{
    cleanup_graphics();
}

/* Sets the graphics mode. */
int draw_set_gfx_mode(void)
{
    int i;
    int try = 1;

    /* Deallocate old double buffers. */
    cleanup_graphics();

    /* What color depth? */
    if (desktop_color_depth() != 0)
        set_color_depth(desktop_color_depth());

    /* Set the graphics mode. */
    while (1) {
        int mode = config.fullscreen ? GFX_AUTODETECT_FULLSCREEN :
                GFX_AUTODETECT_WINDOWED;

        /* Try. */
        if (set_gfx_mode(mode, config.width, config.height, 0, 0) == 0) break;

        /* Uh oh, failure. */
        fprintf(stderr, "xpyre: (try %d) %s\n", try, allegro_error);

        if (try == 3) {
            if (config.fullscreen) {
                /* Maybe fullscreen doesn't work...?  Try three more times. */
                config.fullscreen = 0;
                try = 1;
            } else {
                /* Gack, three strikes... */
                return 0;
            }
        } else {
            ++try;
        }
    }

    /* Now create double buffers. */
    bmp = malloc(sizeof(BITMAP*) * 4);
    for (i = 0; i < 4; i++) {
        bmp[i] = create_bitmap_ex(32, config.width, config.height);
        if (bmp[i] == NULL) {
            /* Can't create bitmap. */
            cleanup_graphics();
            return 0;
        }
        clear_bitmap(bmp[i]);
    }

    /* Precalculate some junk to speed up drawing. */
    yscale = (0.25 * config.height / 240) * 65536;
    fade = MAX(1, 8 * 240 / config.height);
    free(pcm_lookup);
    pcm_lookup = malloc(config.width * sizeof(int));
    for (i = 0; i < config.width; i++)
        pcm_lookup[i] = i * 512 / config.width;

    switch (
#if   BYTE_ORDERING == 0
            getr32(0x00112233)
#elif BYTE_ORDERING == 1
            0x11
#elif BYTE_ORDERING == 2
            0x33
#endif
    ) {
    case 0x11: /* Assume RGB. */
        byte_ordering = 1;
        title_bg_ra = config.bg_color >> 16 & 255;
        title_bg_ga = config.bg_color >>  8 & 255;
        title_bg_ba = config.bg_color >>  0 & 255;
        break;
    case 0x33: /* Assume BGR. */
        byte_ordering = 2;
        title_bg_ba = config.bg_color >> 16 & 255;
        title_bg_ga = config.bg_color >>  8 & 255;
        title_bg_ra = config.bg_color >>  0 & 255;
        break;
    case 0x00: case 0x22: default:
        fprintf(stderr, "xpyre: unsupported color byte ordering.\n");
        return 0;
    }

    /* Calculate the fade gradient once and for all. */
    if (title_fade_lookup == NULL) {
        title_fade_height = text_height(font) + 10;
        title_fade_lookup = malloc(title_fade_height * sizeof(int));
        for (i = 0; i < title_fade_height; i++)
            title_fade_lookup[i] = 255 - i * 256 / title_fade_height;
    }

    return 1;
}

/* Tells the drawing code that it needs to show the track title. */
void draw_show_title(void)
{
    title_state = 1;
    title_pos = 5;
    title_width = text_length(font, track.title) + 5;
    title_time = config.fps / 2;
}

/* Prepares a single frame. */
BITMAP* draw_frame(void)
{
    BITMAP* src, *dst;
    int x, y;
    int c, v, s, o;
    unsigned* p1, *p2, *p3, *p2_beg, *p2_end, *q, *q_end;
    int adj;
    int r, g, b;

    /* Set up the bitmap pointers.  TODO: support more buffers. */
    src = bmp[flip];
    flip = 1 - flip;
    /* ++flip; if (flip == num_bitmaps) flip = 0; */
    dst = bmp[flip];

    /* Apply backup from before if necessary. */
    if (backup_height > 0) {
        blit(bmp[2], src, 0, 0, 0, 0, config.width, backup_height);
        backup_height = 0;
    }

    /* Plot new pcm data on top of old image. */
    pthread_mutex_lock(&pcm_data_mutex);

    /* First, the top. */
    if (config.draw_top) {
        if (config.random_top) {
            for (x = 0; x < config.width; x++) {
                /* Random version. */
                v = abs(pcm_data[pcm_lookup[x]] / 32);
                y = -v * yscale / 65536 + config.height - 8 + my_rand(4);

                /* Determine colors */
                v = MIN(v, 511);
                if (v < 256) c = makecol32(v, 0, 0);
                else c = makecol32(255, v - 256, 0);

                /* Draw smooth curve */
                if (x > 0) {
                    s = (o + y) / 2;
                    vline(src, x - 1, o, s, c);
                    vline(src, x, s, y, c);
                }
                o = y;
            }
        } else {
            for (x = 0; x < config.width; x++) {
                /* Nonrandom version. */
                v = abs(pcm_data[pcm_lookup[x]] / 32);
                y = -v * yscale / 65536 + config.height - 1;

                /* Determine colors */
                v = MIN(v, 511);
                if (v < 256) c = makecol32(v, 0, 0);
                else c = makecol32(255, v - 256, 0);

                /* Draw smooth curve */
                if (x > 0) {
                    s = (o + y) / 2;
                    vline(src, x - 1, o, s, c);
                    vline(src, x, s, y, c);
                }
                o = y;
            }
        }
    }

    /* Next, the fill. */
    if (config.draw_fill) {
        if (config.random_fill) {
            for (x = 0; x < config.width; x++) {
                /* Random version. */
                v = abs(pcm_data[pcm_lookup[x]] / 32);
                y = -v * yscale / 65536 + config.height - 8 + my_rand(4);

                /* Determine colors */
                v = MIN(v, 511);
                if (v < 256) c = makecol32(255, v, 0);
                else c = makecol32(255, 255, v - 256);

                /* Draw semi-vertical line */
                line(src, x + my_rand(4) - 8, config.height - 1,
                        x + my_rand(4) - 7, y, c);
            }
        } else {
            for (x = 0; x < config.width; x++) {
                /* Nonrandom version. */
                v = abs(pcm_data[pcm_lookup[x]] / 32);
                y = -v * yscale / 65536 + config.height - 1;

                /* Determine colors */
                v = MIN(v, 511);
                if (v < 256) c = makecol32(255, v, 0);
                else c = makecol32(255, 255, v - 256);

                /* Draw vertical line */
                vline(src, x, SCREEN_H - 1, y, c);
            }
        }
    }

    /* Okay, done *drawing*. */
    pthread_mutex_unlock(&pcm_data_mutex);

    /* Frob the image. */
    for (y = 1; y < config.height - 1; y++) {
        /* Determine which direction to ripple this scanline. */
        switch (my_rand(2)) {
        case 0:         adj = -1; break;    /* Ripple left  25% of the time. */
        case 1: case 2: adj =  0; break;    /* No ripple    50% of the time. */
        case 3:         adj =  1; break;    /* Ripple right 25% of the time. */
        }

        /* Initialize line pointers. */
        if (adj >= 0) {
            /* Ripple to the right. */
            p1 = (unsigned*)src->line[y - 1] + adj;
            p2 = (unsigned*)src->line[y] + adj;
            p3 = (unsigned*)src->line[y + 1] + adj;
            p2_beg = p2;
            p2_end = p2 + config.width - adj;
            q = (unsigned*)dst->line[y - 1];
        } else {
            /* Ripple to the left. */
            p1 = (unsigned*)src->line[y - 1];
            p2 = (unsigned*)src->line[y];
            p3 = (unsigned*)src->line[y + 1];
            p2_beg = p2;
            p2_end = p2 + config.width + adj;
            q = (unsigned*)dst->line[y - 1] - adj;
        }

        /* Process scanline. */
        while (p2 < p2_end) {
            /* Blur...  Please don't ask me how this works. :^) */
            if (p2 == p2_beg) {
                r = ((p1[0] >> 16) + (p1[1] >> 16) + (p2[0] >> 16) * 2 + (p2[1]
                >> 16) + (p3[0] >> 16) + (p3[1] >> 16)) / 8;

                g = ((p1[0] >> 8 & 255) + (p1[1] >> 8 & 255) + (p2[0] >> 8 &
                255) * 2 + (p2[1] >> 8 & 255) + (p3[0] >> 8 & 255) + (p3[1] >>
                8 & 255)) / 8;

                b = ((p1[0] & 255) + (p1[1] & 255) + (p2[0] & 255) * 2 + (p2[1]
                & 255) + (p3[0] & 255) + (p3[1] & 255)) / 8;
            } else if (p2 == p2_end - 1) {
                r = ((p1[-1] >> 16) + (p1[0] >> 16) + (p2[-1] >> 16) * 2 +
                (p2[0] >> 16) + (p3[-1] >> 16) + (p3[0] >> 16)) / 8;

                g = ((p1[-1] >> 8 & 255) + (p1[0] >> 8 & 255) + (p2[-1] >> 8 &
                255) * 2 + (p2[0] >> 8 & 255) + (p3[-1] >> 8 & 255) + (p3[0] >>
                8 & 255)) / 8;

                b = ((p1[-1] & 255) + (p1[0] & 255) + (p2[-1] & 255) * 2 +
                (p2[0] & 255) + (p3[-1] & 255) + (p3[0] & 255)) / 8;
            } else {
                r = ((p1[-1] >> 16) + (p1[0] >> 16) * 2 + (p1[1] >> 16) +
                (p2[-1] >> 16) * 2 + (p2[0] >> 16) * 4 + (p2[1] >> 16) * 2 +
                (p3[-1] >> 16) + (p3[0] >> 16) * 2 + (p3[1] >> 16)) / 16;

                g = ((p1[-1] >> 8 & 255) + (p1[0] >> 8 & 255) * 2 + (p1[1] >> 8
                & 255) + (p2[-1] >> 8 & 255) * 2 + (p2[0] >> 8 & 255) * 4 +
                (p2[1] >> 8 & 255) * 2 + (p3[-1] >> 8 & 255) + (p3[0] >> 8 &
                255) * 2 + (p3[1] >> 8 & 255)) / 16;

                b = ((p1[-1] & 255) + (p1[0] & 255) * 2 + (p1[1] & 255) +
                (p2[-1] & 255) * 2 + (p2[0] & 255) * 4 + (p2[1] & 255) *2 +
                (p3[-1] & 255) + (p3[0] & 255) * 2 + (p3[1] & 255)) / 16;
            }

            /* Fade. */
            /* TODO: Support different palettes. */
            /* TODO: Stop assuming RGB ordering.  Instead use different names
             * for the r, g, b variables? */
            switch (
#if   BYTE_ORDERING == 0
                    byte_ordering
#else
                    BYTE_ORDERING
#endif
            ) {
            case 1:
                if      (b > 0) b = MAX(0, b - fade);
                else if (g > 0) g = MAX(0, g - fade);
                else if (r > 0) r = MAX(0, r - fade);
                break;
            case 2:
                if      (r > 0) r = MAX(0, r - fade);
                else if (g > 0) g = MAX(0, g - fade);
                else if (b > 0) b = MAX(0, b - fade);
            }

            /* Blit the pixel and advance to the next one. */
            *q = (b << 0) | (g << 8) | (r << 16);
            p1++; p2++; p3++; q++;
        }
    }

    /* Make a backup of the top of the screen if necessary. */
    if (title_fade_amt > 0 || title_state != 0) {
        blit(dst, bmp[2], 0, 0, 0, 0, config.width, title_fade_height);
        backup_height = title_fade_height;
    }

    if (title_fade_amt > 0) {
        /* Display the song title faded background. */
        for (y = 0; y < title_fade_height; y++) {
            adj = 255 - title_fade_lookup[y] * title_fade_amt / 256;
            q = (unsigned*)dst->line[y];
            q_end = q + config.width;
            while (q < q_end) {
                r = ((*q >> 16) - title_bg_ra) * adj / 256 + title_bg_ra;
                g = ((*q >> 8 & 0xff) - title_bg_ga) * adj / 256 + title_bg_ga;
                b = ((*q & 0xff) - title_bg_ba) * adj / 256 + title_bg_ba;
                *q = b | (g << 8) | (r << 16);
                q++;
            }
        }

        /* Decrease the title fade if there's no title onscreen. */
        if (title_state == 0) {
            title_fade_amt -= 16;
            if (title_fade_amt < 0) title_fade_amt = 0;
        }
    }

    if (title_state != 0) {
        /* Display the song title text. */
        textout(dst, font, track.title, title_pos + 1, 6, 0);
        textout(dst, font, track.title, title_pos, 5, ~0);

        if (title_state == 1) {
            /* Count down the time until moving the title. */
            title_time--;
            if (title_time <= 0) title_state = 2;
        } else if (title_state == 2) {
            /* Move the title offscreen. */
            title_pos -= 2;
            if (title_pos <= -title_width) title_state = 0;
        }

        /* Increase the title fade a bit if it's not fully onscreen. */
        if (title_fade_amt < 255) {
            title_fade_amt += 16;
            if (title_fade_amt > 255) title_fade_amt = 255;
        }
    }

    /* Return the BITMAP* we just finished drawing. */
    return dst;
}

/* Frees some bitmaps. */
static void cleanup_graphics(void)
{
    int i;
    if (bmp != NULL) {
        for (i = 0; i < 4; i++)
            if (bmp[i] != NULL) destroy_bitmap(bmp[i]);
        free(bmp);
        bmp = NULL;
    }
}

/* vim: set ts=4 sts=4 sw=4 tw=80 et: */
/* EOF */
