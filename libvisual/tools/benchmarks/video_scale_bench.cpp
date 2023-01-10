#include "benchmark.hpp"
#include <libvisual/libvisual.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstdlib>
#include <cstring>

namespace {

  class VideoScaleBench
      : public LV::Tools::Benchmark
  {
  public:

      VideoScaleBench (unsigned int        src_width,
                       unsigned int        src_height,
                       unsigned int        dst_width,
                       unsigned int        dst_height,
                       VisVideoDepth       depth,
                       VisVideoScaleMethod method)
          : Benchmark ("VideoScaleBench")
          , m_src    { LV::Video::create (dst_width, dst_height, depth) }
          , m_dst    { LV::Video::create (src_width, src_height, depth) }
          , m_method { method }
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++)
              m_dst->scale (m_src, m_method);
      };

      virtual ~VideoScaleBench ()
      {}

  private:

      LV::VideoPtr        m_src;
      LV::VideoPtr        m_dst;
      VisVideoScaleMethod m_method;
  };

  std::unique_ptr<VideoScaleBench> make_benchmark (int& argc, char**& argv)
  {
      unsigned int        src_width  = 320;
      unsigned int        src_height = 240;
      unsigned int        dst_width  = src_width * 2.5;
      unsigned int        dst_height = src_height * 2.5;
      VisVideoDepth       depth      = VISUAL_VIDEO_DEPTH_32BIT;
      VisVideoScaleMethod method     = VISUAL_VIDEO_SCALE_BILINEAR;

      if (argc > 2) {
          int value1 = std::atoi (argv[1]);
          int value2 = std::atoi (argv[2]);

          if (value1 <= 0 || value2 <= 0) {
              throw std::invalid_argument ("Invalid dimensions specified");
          }

          src_width  = value1;
          src_height = value2;

          argc -= 2; argv += 2;
      }

      if (argc > 2) {
          int value1 = std::atoi (argv[1]);
          int value2 = std::atoi (argv[2]);

          if (value1 <= 0 || value2 <= 0) {
              throw std::invalid_argument ("Invalid dimensions specified");
          }

          dst_width  = value1;
          dst_height = value2;

          argc -= 2; argv += 2;
      }

      if (argc > 1) {
          depth = visual_video_depth_from_bpp (std::atoi (argv[1]));
          if (depth == VISUAL_VIDEO_DEPTH_NONE) {
              throw std::invalid_argument ("Invalid video depth specified");
          }

          argc--; argv++;
      }

      if (argc > 1) {
          if (std::strcmp (argv[1], "bilinear") == 0) {
              method = VISUAL_VIDEO_SCALE_BILINEAR;
          }
          else if (std::strcmp (argv[1], "nearest") == 0) {
              method = VISUAL_VIDEO_SCALE_NEAREST;
          }
          else {
              throw std::invalid_argument ("Invalid scaling method specified");
          }

          argc--; argv++;
      }

      return std::make_unique<VideoScaleBench> (src_width, src_height, dst_width, dst_height, depth, method);
  }

} // anonymous

int main (int argc, char **argv)
{
    try {
        LV::System::init (argc, argv);

        unsigned int max_runs = 10000;

        if (argc > 1) {
            int value = std::atoi (argv[1]);
            if (value <= 0) {
                throw std::invalid_argument ("Number of runs is non-positive");
            }

            max_runs = value;

            argc--; argv++;
        }

        auto bench = make_benchmark (argc, argv);
        LV::Tools::run_benchmark (*bench, max_runs);

        return EXIT_SUCCESS;
    }
    catch (std::exception& error) {
        std::cerr << "Exception caught: " << error.what () << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown exception caught\n";
        return EXIT_FAILURE;
    }
}
