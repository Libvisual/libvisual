#include "config.h"
#include "lv_audio_convert.h"
#include "lv_audio.h"
#include "lv_mem.h"
#include "lv_enable_if.hpp"
#include "lv_int_traits.hpp"
#include <limits>

using namespace LV;

namespace {

  template <typename D, typename S>
  struct same_signedness
  {
      static const bool value = (is_signed<D>::value && is_signed<S>::value) ||
                                (is_unsigned<D>::value && is_unsigned<S>::value);
  };

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

  template <typename D, typename S>
  inline int shifter()
  {
      if (sizeof(S) > sizeof(D))
          return int (sizeof(S) - sizeof(D)) << 3;
      else
          return int (sizeof(D) - sizeof(S)) << 3;
  }

  // same format conversion
  template <typename T>
  inline void convert_sample_array (T* dst, T const* src, std::size_t count)
  {
      visual_mem_copy (dst, src, sizeof (T) * count);
  }

  // signed->unsigned int conversion (same width)
  template <typename D, typename S>
  typename enable_if_c<is_unsigned<D>::value && is_signed<S>::value && sizeof(D) == sizeof(S)>::type
  inline convert_sample_array (D* dst, S const* src, std::size_t count)
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
  inline convert_sample_array (D* dst, S const* src, std::size_t count)
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
  inline convert_sample_array (float* dst, S const* src, std::size_t count)
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
  inline convert_sample_array (D* dst, float const* src, std::size_t count)
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

  // narrowing/widening int conversion (same signedness)
  template <typename D, typename S>
  typename enable_if_c<same_signedness<D, S>::value && sizeof(D) != sizeof(S) >::type
  inline convert_sample_array (D* dst, S const* src, std::size_t count)
  {
      const int shift = shifter<D, S> ();

      S const* src_end = src + count;

      if (sizeof(S) > sizeof(D)) {
          // narrowing
          while (src != src_end) {
              *dst = *src >> shift;
              dst++;
              src++;
          }
      } else {
          // widening
          while (src != src_end) {
              *dst = *src << shift;
              dst++;
              src++;
          }
      }
  }

  // narrowing/widening unsigned->signed int conversion
  template <typename D, typename S>
  typename enable_if_c<is_signed<D>::value && is_unsigned<S>::value && sizeof(D) != sizeof(S)>::type
  inline convert_sample_array (D* dst, S const* src, std::size_t count)
  {
      D a = zero<D>();
      const int shift = shifter<D, S> ();

      S const* src_end = src + count;

      if (sizeof(D) < sizeof(S)) {
          // narrowing
          while (src != src_end) {
              *dst = D(*src >> shift) - a;
              dst++;
              src++;
          }
      } else {
          // widening
          while (src != src_end) {
              *dst = D(*src << shift) - a;
              dst++;
              src++;
          }
      }
  }

  // narrowing/widening signed->unsigned int conversion
  template <typename D, typename S>
  typename enable_if_c<is_unsigned<D>::value && is_signed<S>::value && sizeof(D) != sizeof(S)>::type
  inline convert_sample_array (D* dst, S const* src, std::size_t count)
  {
      D a = zero<D>();
      const int shift = shifter<D, S> ();

      S const* src_end = src + count;

      if (sizeof(D) < sizeof(S)) {
          // narrowing
          while (src != src_end) {
              *dst = D(*src >> shift) + a;
              dst++;
              src++;
          }
      } else {
          // widening
          while (src != src_end) {
              *dst = D(*src << shift) + a;
              dst++;
              src++;
          }
      }
  }

  template <typename D, typename S>
  void convert (void* dst, void const* src, std::size_t size)
  {
      convert_sample_array (static_cast<D*> (dst), static_cast<S const*> (src), size / sizeof (S));
  }

  typedef void (*ConvertFunc)(void*, void const*, std::size_t);

  ConvertFunc const convert_func_table[7][7] = {
      {
          convert<uint8_t, uint8_t>,
          convert<uint8_t, int8_t>,
          convert<uint8_t, uint16_t>,
          convert<uint8_t, int16_t>,
          convert<uint8_t, uint32_t>,
          convert<uint8_t, int32_t>,
          convert<uint8_t, float>
      },

      {
          convert<int8_t, uint8_t>,
          convert<int8_t, int8_t>,
          convert<int8_t, uint16_t>,
          convert<int8_t, int16_t>,
          convert<int8_t, uint32_t>,
          convert<int8_t, int32_t>,
          convert<int8_t, float>
      },

      {
          convert<uint16_t, uint8_t>,
          convert<uint16_t, int8_t>,
          convert<uint16_t, uint16_t>,
          convert<uint16_t, int16_t>,
          convert<uint16_t, uint32_t>,
          convert<uint16_t, int32_t>,
          convert<uint16_t, float>
      },

      {
          convert<int16_t, uint8_t>,
          convert<int16_t, int8_t>,
          convert<int16_t, uint16_t>,
          convert<int16_t, int16_t>,
          convert<int16_t, uint32_t>,
          convert<int16_t, int32_t>,
          convert<int16_t, float>
      },

      {
          convert<uint32_t, uint8_t>,
          convert<uint32_t, int8_t>,
          convert<uint32_t, uint16_t>,
          convert<uint32_t, int16_t>,
          convert<uint32_t, uint32_t>,
          convert<uint32_t, int32_t>,
          convert<uint32_t, float>
      },

      {
          convert<int32_t, uint8_t>,
          convert<int32_t, int8_t>,
          convert<int32_t, uint16_t>,
          convert<int32_t, int16_t>,
          convert<int32_t, uint32_t>,
          convert<int32_t, int32_t>,
          convert<int32_t, float>
      },

      {
          convert<float, uint8_t>,
          convert<float, int8_t>,
          convert<float, uint16_t>,
          convert<float, int16_t>,
          convert<float, uint32_t>,
          convert<float, int32_t>,
          convert<float, float>
      }
  };

  template <typename T>
  inline void deinterleave_stereo_sample_array (T* dest1, T* dest2, T const* src, std::size_t count)
  {
      T const* src_end = src + count * 2;

      while (src != src_end)
      {
          *dest1 = src[0];
          *dest2 = src[1];

          dest1++;
          dest2++;
          src += 2;
      }
  }

  template <typename T>
  void deinterleave_stereo (void* dest1, void* dest2, void const* src, std::size_t size)
  {
      deinterleave_stereo_sample_array (static_cast<T*> (dest1), static_cast<T*> (dest2), static_cast<T const*> (src), size / sizeof(T));
  }

  typedef void (*DeinterleaveStereoFunc)(void*, void*, void const*, std::size_t);

  DeinterleaveStereoFunc const deinterleave_stereo_func_table[7] = {
      deinterleave_stereo<uint8_t>,
      deinterleave_stereo<int8_t>,
      deinterleave_stereo<uint16_t>,
      deinterleave_stereo<int16_t>,
      deinterleave_stereo<uint32_t>,
      deinterleave_stereo<int32_t>,
      deinterleave_stereo<float>
  };

} // anonymous namespace

void visual_audio_sample_convert (VisBuffer *dest, VisAudioSampleFormatType dest_format, VisBuffer *src, VisAudioSampleFormatType src_format)
{
    void* dbuf = visual_buffer_get_data (dest);
    void const* sbuf = visual_buffer_get_data (src);
    std::size_t size = visual_buffer_get_size (src);

    int i = int (dest_format) - 1;
    int j = int (src_format)  - 1;

    convert_func_table[i][j] (dbuf, sbuf, size);
}

void visual_audio_sample_deinterleave_stereo (VisBuffer *dest1, VisBuffer *dest2, VisBuffer *src, VisAudioSampleFormatType format)
{
    void* dbuf1 = visual_buffer_get_data (dest1);
    void* dbuf2 = visual_buffer_get_data (dest2);
    void const* sbuf = visual_buffer_get_data (src);
    std::size_t size = visual_buffer_get_size (src);

    int i = int (format) - 1;

    deinterleave_stereo_func_table[i] (dbuf1, dbuf2, sbuf, size);
}
