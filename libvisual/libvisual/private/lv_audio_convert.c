#include "config.h"
#include "lv_audio_convert.h"
#include "lv_audio.h"

static unsigned int byte_max_numeric (int bytes)
{
    int result = 256;
    int i;

    if (bytes == 0)
        return 0;

    for (i = 1; i < bytes; i++)
        result *= 256;

    return result;
}

/* FIXME use lv_math acceleration here! */
#define FORMAT_BUFFER_FROM_FLOAT(a,b)                           \
    {                                                           \
        if (sign) {                                             \
            a *dbuf = visual_buffer_get_data (src);             \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = sbuf[i] * signedcorr;                 \
            }                                                   \
        } else {                                                \
            b *dbuf = visual_buffer_get_data (src);             \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] * signedcorr) + signedcorr;  \
            }                                                   \
        }                                                       \
    }

void audio_sample_convert_from_float (VisBuffer *dest, VisBuffer *src, int size, int sign)
{
    float *sbuf = visual_buffer_get_data (src);
    int entries = visual_buffer_get_size (dest) / size;
    int signedcorr = 0;
    int i;

    signedcorr += byte_max_numeric (size) / 2;

    if (size == 1)
        FORMAT_BUFFER_FROM_FLOAT(int8_t, uint8_t)
    else if (size == 2)
        FORMAT_BUFFER_FROM_FLOAT(int16_t, uint16_t)
    else if (size == 4)
        FORMAT_BUFFER_FROM_FLOAT(int32_t, uint32_t)
}

#define FORMAT_BUFFER_TO_FLOAT(a,b)                             \
    {                                                           \
        float multiplier = 1.0 / signedcorr;                    \
        if (sign) {                                             \
            a *sbuf = visual_buffer_get_data (src);             \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = sbuf[i] * multiplier;                 \
            }                                                   \
        } else {                                                \
            b *sbuf = visual_buffer_get_data (src);             \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] - signedcorr) * multiplier;  \
            }                                                   \
        }                                                       \
    }

void audio_sample_convert_to_float (VisBuffer *dest, VisBuffer *src, int size, int sign)
{
    float *dbuf = visual_buffer_get_data (dest);
    int entries = visual_buffer_get_size (dest) /
        visual_audio_sample_format_get_size (VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT);
    int signedcorr;
    int i;

    signedcorr = byte_max_numeric (size) / 2;

    if (size == 1)
        FORMAT_BUFFER_TO_FLOAT(int8_t, uint8_t)
    else if (size == 2)
        FORMAT_BUFFER_TO_FLOAT(int16_t, uint16_t)
    else if (size == 4)
        FORMAT_BUFFER_TO_FLOAT(int32_t, uint32_t)
}

#define FORMAT_BUFFER(a,b,c,d)                              \
    {                                                       \
        if (signedcorr == 0) {                              \
            visual_buffer_put (dest, src, 0);               \
        } else {                                            \
            if (signedcorr < 0) {                           \
                a *dbuf = visual_buffer_get_data (dest);    \
                b *sbuf = visual_buffer_get_data (src);     \
                for (i = 0; i < entries; i++) {             \
                    dbuf[i] = sbuf[i] + signedcorr;         \
                }                                           \
            } else {                                        \
                c *dbuf = visual_buffer_get_data (dest);    \
                d *sbuf = visual_buffer_get_data (src);     \
                for (i = 0; i < entries; i++) {             \
                    dbuf[i] = sbuf[i] + signedcorr;         \
                }                                           \
            }                                               \
        }                                                   \
    }

#define FORMAT_BUFFER_INCREASE(a,b,c,d)                         \
    {                                                           \
        if (signedcorr < 0) {                                   \
            a *dbuf = visual_buffer_get_data (dest);            \
            b *sbuf = visual_buffer_get_data (src);             \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] << shifter) + signedcorr;    \
            }                                                   \
        } else {                                                \
            c *dbuf = visual_buffer_get_data (dest);            \
            d *sbuf = visual_buffer_get_data (src);             \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] << shifter) + signedcorr;    \
            }                                                   \
        }                                                       \
    }

#define FORMAT_BUFFER_DECREASE(a,b,c,d)                         \
    {                                                           \
        if (signedcorr < 0) {                                   \
            a *dbuf = visual_buffer_get_data (dest);            \
            b *sbuf = visual_buffer_get_data (src);             \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] >> shifter) + signedcorr;    \
            }                                                   \
        } else {                                                \
            c *dbuf = visual_buffer_get_data (dest);            \
            d *sbuf = visual_buffer_get_data (src);             \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] >> shifter) + signedcorr;    \
            }                                                   \
        }                                                       \
    }

void audio_sample_convert (VisBuffer *dest, VisBuffer *src, int dsize, int ssize, int dsigned, int ssigned)
{
    int signedcorr = 0;
    int entries = visual_buffer_get_size (dest) / dsize;
    int shifter = 0;
    int i;

    if (dsigned && !ssigned)
        signedcorr -= byte_max_numeric (ssize) / 2;
    else if (!dsigned && ssigned)
        signedcorr += byte_max_numeric (dsize) / 2;

    if (dsize > ssize)
        shifter = dsize - ssize;
    else if (dsize < ssize)
        shifter = ssize - dsize;

    /* FIXME simd versions of every conversion */
    if (dsize == 1 && ssize == 1)       /* 8 to 8 */
        FORMAT_BUFFER(int8_t, uint8_t, uint8_t, int8_t)
    else if (dsize == 2 && ssize == 1)  /* 8 to 16 */
        FORMAT_BUFFER_INCREASE(int16_t, int8_t, uint16_t, uint8_t)
    else if (dsize == 4 && ssize == 1)  /* 8 to 32 */
        FORMAT_BUFFER_INCREASE(int32_t, int8_t, uint32_t, uint8_t)
    else if (dsize == 2 && ssize == 2)  /* 16 to 16 */
        FORMAT_BUFFER(int16_t, uint16_t, uint16_t, int16_t)
    else if (dsize == 4 && ssize == 2)  /* 32 to 16 */
        FORMAT_BUFFER_INCREASE(int32_t, int16_t, uint32_t, uint16_t)
    else if (dsize == 4 && ssize == 4)  /* 32 to 32 */
        FORMAT_BUFFER(int32_t, uint32_t, uint32_t, int32_t)
    else if (dsize == 1 && ssize == 2)  /* 16 to 8 */
        FORMAT_BUFFER_DECREASE(int8_t, int16_t, uint8_t, uint16_t)
    else if (dsize == 2 && ssize == 4)  /* 32 to 16 */
        FORMAT_BUFFER_DECREASE(int16_t, int32_t, uint16_t, uint32_t)
    else if (dsize == 1 && ssize == 4)  /* 32 to 8 */
        FORMAT_BUFFER_DECREASE(int8_t, int32_t, uint8_t, uint32_t)
}
