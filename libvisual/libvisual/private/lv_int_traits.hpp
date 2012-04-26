#ifndef _LV_INT_TRAITS_HPP
#define _LV_INT_TRAITS_HPP

namespace LV {

  template <typename T>
  struct is_signed { static const bool value = false; };

  template <> struct is_signed<int8_t >    { static const bool value = true; };
  template <> struct is_signed<int16_t>    { static const bool value = true; };
  template <> struct is_signed<int32_t>    { static const bool value = true; };
  template <> struct is_signed<int64_t>    { static const bool value = true; };

  template <typename T>
  struct is_unsigned { static const bool value = false; };

  template <> struct is_unsigned<uint8_t > { static const bool value = true; };
  template <> struct is_unsigned<uint16_t> { static const bool value = true; };
  template <> struct is_unsigned<uint32_t> { static const bool value = true; };
  template <> struct is_unsigned<uint64_t> { static const bool value = true; };

  template <typename T>
  struct is_integral
  {
      static const bool value = is_unsigned<T>::value || is_signed<T>::value;
  };

} // LV namespace

#endif // _LV_INT_TRAITS_HPP
