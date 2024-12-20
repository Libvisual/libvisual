#ifndef _LV_TESTS_VIDEO_RANDOM_HPP
#define _LV_TESTS_VIDEO_RANDOM_HPP

#include <libvisual/libvisual.h>

namespace LV::Tests
{
  LV::VideoPtr create_random_video (int width, int height, VisVideoDepth depth);

} // LV::Tests namespace

#endif // defined(_LV_TESTS_VIDEO_COMMON_HPP)
