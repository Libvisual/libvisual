#ifndef LV_VIDEO_TEST_COMMON_HPP
#define LV_VIDEO_TEST_COMMON_HPP

#include <libvisual/libvisual.h>
#include <filesystem>
#include <optional>

LV::VideoPtr load_raw_image (std::filesystem::path const& path, int width, int height, VisVideoDepth depth);

std::optional<LV::Palette> load_raw_palette (std::filesystem::path const& path);

LV::VideoPtr load_raw_indexed_image (std::filesystem::path const& image_path,
                                     std::filesystem::path const& palette_path,
                                     int width,
                                     int height);

#endif // LV_VIDEO_TEST_COMMON_HPP
