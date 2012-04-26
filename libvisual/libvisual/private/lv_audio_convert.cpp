#include "config.h"
#include "lv_audio_convert.h"
#include "lv_audio.h"
#include "lv_mem.h"
#include "lv_enable_if.hpp"
#include "lv_int_traits.hpp"
#include <limits>

using namespace LV;

namespace {

  inline unsigned int byte_max_numeric (int bytes)
  {
      return bytes == 0 ? 0 : (1 << (bytes << 3));
  }

  template <typename T>
  inline typename enable_if<is_signed<T>, T>::type
  half_range ()
  {
      return std::numeric_limits<T>::max ();
  }

  template <typename T>
  inline typename enable_if<is_unsigned<T>, T>::type
  half_range ()
  {
      return std::numeric_limits<T>::max () / 2;
  }

  template <typename T>
  inline typename enable_if<is_signed<T>, T>::type
  zero ()
  {
      return 0;
  }

  template <typename T>
  inline typename enable_if<is_unsigned<T>, T>::type
  zero ()
  {
      return std::numeric_limits<T>::max () / 2 + 1;
  }

} // anonymous namespace


// same format conversion
template <typename T>
void convert_sample_array (T* dst, T const* src, std::size_t count)
{
    visual_mem_copy (dst, src, sizeof (T) * count);
}

// signed->unsigned int conversion (same width)
template <typename D, typename S>
typename enable_if_c<is_unsigned<D>::value && is_signed<S>::value && sizeof(D) == sizeof(S)>::type
convert_sample_array (D* dst, S const* src, std::size_t count)
{
    D a = zero<D> ();

    S const* src_end = src + count;

    while (src != src_end) {
        *dst = *src + a;
        dst++;
        src++;
    }
}

// unsigned->signed int conversion (same width)
template <typename D, typename S>
typename enable_if_c<is_signed<D>::value && is_unsigned<S>::value && sizeof(D) == sizeof(S)>::type
convert_sample_array (D* dst, S const* src, std::size_t count)
{
    S a = zero<S> ();

    S const* src_end = src + count;

    while (src != src_end) {
        *dst = *src - a;
        dst++;
        src++;
    }
}

// int->float conversions
template <typename S>
typename enable_if<is_integral<S> >::type
convert_sample_array (float* dst, S const* src, std::size_t count)
{
    float a = 1.0 / (half_range<S> () + 1);
    float b = -zero<S>() * a;

    S const* src_end = src + count;

    while (src != src_end) {
        *dst = *src * a + b;

        dst++;
        src++;
    }
}

// float->int conversions
template <typename D>
typename enable_if<is_integral<D> >::type
convert_sample_array (D* dst, float const* src, std::size_t count)
{
    float a = half_range<D> ();
    float b = zero<D> ();

    float const* src_end = src + count;

    while (src != src_end) {
        *dst = *src * a + b;

        dst++;
        src++;
    }
}

void audio_sample_convert_from_float (VisBuffer *dest, VisBuffer *src, int int_size, int is_signed)
{
    void* d = visual_buffer_get_data (dest);
    float const* s = static_cast<float const*> (visual_buffer_get_data (src));

    unsigned int count = visual_buffer_get_size (src) / sizeof (float);

    if (is_signed) {
        switch (int_size) {
            case 1: convert_sample_array (static_cast<int8_t*>  (d), s, count); return;
            case 2: convert_sample_array (static_cast<int16_t*> (d), s, count); return;
            case 4: convert_sample_array (static_cast<int32_t*> (d), s, count); return;
        }
    } else {
        switch (int_size) {
            case 1: convert_sample_array (static_cast<uint8_t*>  (d), s, count); return;
            case 2: convert_sample_array (static_cast<uint16_t*> (d), s, count); return;
            case 4: convert_sample_array (static_cast<uint32_t*> (d), s, count); return;
        }
    }
}

void audio_sample_convert_to_float (VisBuffer *dest, VisBuffer *src, int int_size, int is_signed)
{
    float* d = static_cast<float*> (visual_buffer_get_data (dest));
    void const* s = visual_buffer_get_data (src);

    unsigned int count = visual_buffer_get_size (src) / int_size;

    if (is_signed) {
        switch (int_size) {
            case 1: convert_sample_array (d, static_cast<int8_t const*>  (s), count); return;
            case 2: convert_sample_array (d, static_cast<int16_t const*> (s), count); return;
            case 4: convert_sample_array (d, static_cast<int32_t const*> (s), count); return;
        }
    } else {
        switch (int_size) {
            case 1: convert_sample_array (d, static_cast<uint8_t const*>  (s), count); return;
            case 2: convert_sample_array (d, static_cast<uint16_t const*> (s), count); return;
            case 4: convert_sample_array (d, static_cast<uint32_t const*> (s), count); return;
        }
    }
}

#define FORMAT_BUFFER_INCREASE(a,b,c,d)                         \
    {                                                           \
        if (signedcorr < 0) {                                   \
            a *dbuf = static_cast<a*> (visual_buffer_get_data (dest));  \
            b *sbuf = static_cast<b*> (visual_buffer_get_data (src));   \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] << shifter) + signedcorr;    \
            }                                                   \
        } else {                                                \
            c *dbuf = static_cast<c*> (visual_buffer_get_data (dest));  \
            d *sbuf = static_cast<d*> (visual_buffer_get_data (src));   \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] << shifter) + signedcorr;    \
            }                                                   \
        }                                                       \
    }

#define FORMAT_BUFFER_DECREASE(a,b,c,d)                         \
    {                                                           \
        if (signedcorr < 0) {                                   \
            a *dbuf = static_cast<a*> (visual_buffer_get_data (dest));  \
            b *sbuf = static_cast<b*> (visual_buffer_get_data (src));   \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] >> shifter) + signedcorr;    \
            }                                                   \
        } else {                                                \
            c *dbuf = static_cast<c*> (visual_buffer_get_data (dest));  \
            d *sbuf = static_cast<d*> (visual_buffer_get_data (src));   \
            for (i = 0; i < entries; i++) {                     \
                dbuf[i] = (sbuf[i] >> shifter) + signedcorr;    \
            }                                                   \
        }                                                       \
    }

void audio_sample_convert (VisBuffer *dest, VisBuffer *src, int dsize, int ssize, int dsigned, int ssigned)
{
    if (dsize == ssize) {
        void* dbuf = visual_buffer_get_data (dest);
        void const* sbuf = visual_buffer_get_data (src);
        std::size_t size = visual_buffer_get_size (src);

        if (dsigned == ssigned) {
            if (dsigned) {
                switch (dsize) {
                    case 1: convert_sample_array (static_cast<uint8_t*>  (dbuf), static_cast<uint8_t  const*> (sbuf), size); return;
                    case 2: convert_sample_array (static_cast<uint16_t*> (dbuf), static_cast<uint16_t const*> (sbuf), size); return;
                    case 4: convert_sample_array (static_cast<uint32_t*> (dbuf), static_cast<uint32_t const*> (sbuf), size); return;
                }
            } else {
                switch (dsize) {
                    case 1: convert_sample_array (static_cast<int8_t*>  (dbuf), static_cast<int8_t  const*> (sbuf), size); return;
                    case 2: convert_sample_array (static_cast<int16_t*> (dbuf), static_cast<int16_t const*> (sbuf), size); return;
                    case 4: convert_sample_array (static_cast<int32_t*> (dbuf), static_cast<int32_t const*> (sbuf), size); return;
                }
            }
        } else {
            if (dsigned) {
                switch (dsize) {
                    case 1: convert_sample_array (static_cast<int8_t*>  (dbuf), static_cast<uint8_t  const*> (sbuf), size); return;
                    case 2: convert_sample_array (static_cast<int16_t*> (dbuf), static_cast<uint16_t const*> (sbuf), size); return;
                    case 4: convert_sample_array (static_cast<int32_t*> (dbuf), static_cast<uint32_t const*> (sbuf), size); return;
                }
            } else {
                switch (dsize) {
                    case 1: convert_sample_array (static_cast<uint8_t*>  (dbuf), static_cast<int8_t  const*> (sbuf), size); return;
                    case 2: convert_sample_array (static_cast<uint16_t*> (dbuf), static_cast<int16_t const*> (sbuf), size); return;
                    case 4: convert_sample_array (static_cast<uint32_t*> (dbuf), static_cast<int32_t const*> (sbuf), size); return;
                }
            }
        }
    }

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
    else if (dsize == 2 && ssize == 1)  /* 8 to 16 */
        FORMAT_BUFFER_INCREASE(int16_t, int8_t, uint16_t, uint8_t)
    else if (dsize == 4 && ssize == 1)  /* 8 to 32 */
        FORMAT_BUFFER_INCREASE(int32_t, int8_t, uint32_t, uint8_t)
    else if (dsize == 4 && ssize == 2)  /* 32 to 16 */
        FORMAT_BUFFER_INCREASE(int32_t, int16_t, uint32_t, uint16_t)
    else if (dsize == 1 && ssize == 2)  /* 16 to 8 */
        FORMAT_BUFFER_DECREASE(int8_t, int16_t, uint8_t, uint16_t)
    else if (dsize == 2 && ssize == 4)  /* 32 to 16 */
        FORMAT_BUFFER_DECREASE(int16_t, int32_t, uint16_t, uint32_t)
    else if (dsize == 1 && ssize == 4)  /* 32 to 8 */
        FORMAT_BUFFER_DECREASE(int8_t, int32_t, uint8_t, uint32_t)
}
