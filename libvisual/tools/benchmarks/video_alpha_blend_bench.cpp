#include "benchmark.hpp"
#include <libvisual/libvisual.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstdlib>

namespace {

  class VideoAlphaBlendBench
      : public LV::Tools::Benchmark
  {
  public:

      VideoAlphaBlendBench (unsigned int width, unsigned int height, VisVideoDepth depth)
          : Benchmark { "VideoAlphaBlendBench" }
          , m_src     { LV::Video::create (width, height, depth) }
          , m_dest    { LV::Video::create (width, height, depth) }
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              m_dest->blit (m_src, 0, 0, true);
          }
      }

      virtual ~VideoAlphaBlendBench ()
      {}

  private:

      LV::VideoPtr m_src;
      LV::VideoPtr m_dest;
  };

  std::unique_ptr<VideoAlphaBlendBench> make_benchmark (int& argc, char**& argv)
  {
      unsigned int  width    = 1024;
      unsigned int  height   = 768;
      VisVideoDepth depth    = VISUAL_VIDEO_DEPTH_32BIT;

      if (argc > 2) {
          int value1 = std::atoi (argv[1]);
          int value2 = std::atoi (argv[2]);

          if (value1 <= 0 || value2 <= 0) {
              throw std::invalid_argument ("Invalid dimensions specified");
          }

          width  = value1;
          height = value2;

          argc -= 2; argv += 2;
      }

      if (argc > 1) {
          depth = visual_video_depth_from_bpp (std::atoi (argv[1]));
          if (depth == VISUAL_VIDEO_DEPTH_NONE) {
              throw std::invalid_argument ("Invalid video depth specified");
          }

          argc--; argv++;
      }

      return std::make_unique<VideoAlphaBlendBench> (width, height, depth);
  }

}

int main (int argc, char **argv)
{
    try {
        LV::System::init (argc, argv);

        unsigned int max_runs = 100000;

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
