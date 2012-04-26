#ifndef _LV_INT_TRAITS_HPP
#define _LV_INT_TRAITS_HPP

namespace LV {

  template <typename T>
  struct is_integral
  {
	  static const bool value = false;
  };

  template <> struct is_integral<uint8_t>  { static const bool value = true; };
  template <> struct is_integral<uint16_t> { static const bool value = true; };
  template <> struct is_integral<uint32_t> { static const bool value = true; };
  template <> struct is_integral<uint64_t> { static const bool value = true; };
  template <> struct is_integral<int8_t>   { static const bool value = true; };
  template <> struct is_integral<int16_t>  { static const bool value = true; };
  template <> struct is_integral<int32_t>  { static const bool value = true; };
  template <> struct is_integral<int64_t>  { static const bool value = true; };

  template <typename T>
  struct is_signed {};

  template <> struct is_signed<uint8_t>    { static const bool value = false; };
  template <> struct is_signed<uint16_t>   { static const bool value = false; };
  template <> struct is_signed<uint32_t>   { static const bool value = false; };
  template <> struct is_signed<uint64_t>   { static const bool value = false; };
  template <> struct is_signed<int8_t >    { static const bool value = true; };
  template <> struct is_signed<int16_t>    { static const bool value = true; };
  template <> struct is_signed<int32_t>    { static const bool value = true; };
  template <> struct is_signed<int64_t>    { static const bool value = true; };

  template <typename T>
  struct is_unsigned_integral
  {
	  static const bool value = is_integral<T>::value && !is_signed<T>::value;
  };

  template <typename T>
  struct is_signed_integral
  {
	  static const bool value = is_integral<T>::value && is_signed<T>::value;
  };

} // LV namespace

#endif // _LV_INT_TRAITS_HPP
