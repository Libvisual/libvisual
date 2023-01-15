#ifndef _LV_TESTS_MATH_UTIL_HPP
#define _LV_TESTS_MATH_UTIL_HPP

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>
#include <cmath>
#include <algorithm>

namespace LV::Tests {

    // Floating point equality comparison functions
    //
    // Adapted from the original code by Bruce Dawson, Valve Software.
    // See: http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
    //
    // TODO: Generalize this to work with all C++11 supported floating
    // point types i.e. doubles and long doubles.

    union Float_t
    {
        constexpr Float_t (float num = 0.0f) : f (num) {}

        // Portable extraction of components.
        constexpr bool    is_negative()  const { return (i >> 31) != 0; }
        constexpr int32_t raw_mantissa() const { return i & ((1 << 23) - 1); }
        constexpr int32_t raw_exponent() const { return (i >> 23) & 0xFF; }

        int32_t i;
        float f;

        #ifndef NDEBUG
        struct
        {   // Bitfields for exploration. Do not use in production code.
            uint32_t mantissa : 23;
            uint32_t exponent : 8;
            uint32_t sign : 1;
        } parts;
        #endif
    };

    constexpr inline bool almost_equal_ulps_and_abs (float a, float b, float max_diff, float max_ulps_diff)
    {
        // Check if the numbers are really close -- needed when
        // comparing numbers near zero.
        float abs_diff = std::fabs (a - b);
        if (abs_diff <= max_diff)
            return true;

        Float_t u_a (a);
        Float_t u_b (b);

        // Different signs means they do not match.
        if (u_a.is_negative() != u_b.is_negative ())
            return false;

        // Find the difference in ULPs.
        int ulps_diff = std::abs (u_b.i - u_b.i);
        if (ulps_diff <= max_ulps_diff)
            return true;

        return false;
    }

    constexpr inline bool almost_equal_relative_and_abs (float a, float b, float max_diff, float max_rel_diff)
    {
        // Check if the numbers are really close -- needed when
        // comparing numbers near zero.
        float diff = std::fabs (a - b);
        if (diff <= max_diff)
            return true;

        a = std::fabs (a);
        b = std::fabs (b);

        float largest = std::max (a, b);
        if (diff <= largest * max_rel_diff)
            return true;

        return false;
    }

} // LV::Tests namespace

#endif // _LV_TESTS_MATH_UTIL_HPP
