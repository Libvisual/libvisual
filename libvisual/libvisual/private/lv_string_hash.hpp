#ifndef _LV_STRING_HASH_HPP
#define _LV_STRING_HASH_HPP

#include <string>
#include <string_view>

namespace LV {

  // Transparent string hash functor.
  struct StringHash
  {
      using is_transparent = void;

      using HashType = std::hash<std::string_view>;

      std::size_t operator() (char const* str) const
      {
          return HashType {} (str);
      }

      std::size_t operator() (std::string_view str) const
      {
          return HashType {} (str);
      }

      std::size_t operator() (std::string const& str) const
      {
          return HashType {} (str);
      }
  };

} // LV namespace

#endif // defined(_LV_STRING_HASH_HPP)
