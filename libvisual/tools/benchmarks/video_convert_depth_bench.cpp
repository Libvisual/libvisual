#include "benchmark.hpp"
#include <libvisual/libvisual.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstdlib>

namespace {

  class VideoConvertDepthBench
      : public LV::Tools::Benchmark
  {
  public:

      VideoConvertDepthBench (unsigned int width, unsigned int height, VisVideoDepth src_depth, VisVideoDepth dst_depth)
          : Benchmark { "VideoConvertDepthBench" }
          , m_src { LV::Video::create (width, height, src_depth) }
          , m_dst { LV::Video::create (width, height, dst_depth) }
      {
          if (src_depth == VISUAL_VIDEO_DEPTH_8BIT)
              m_src->set_palette (LV::Palette (256));

          if (dst_depth == VISUAL_VIDEO_DEPTH_8BIT)
              m_dst->set_palette (LV::Palette (256));
      }

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++)
              m_dst->convert_depth (m_src);
      }

      virtual ~VideoConvertDepthBench ()
      {}

  private:

      LV::VideoPtr m_src;
      LV::VideoPtr m_dst;
  };

  std::unique_ptr<VideoConvertDepthBench> make_benchmark (int& argc, char**& argv)
  {
      unsigned int  width     = 640;
      unsigned int  height    = 480;
      VisVideoDepth src_depth = VISUAL_VIDEO_DEPTH_24BIT;
      VisVideoDepth dst_depth = VISUAL_VIDEO_DEPTH_16BIT;

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

      if (argc > 2) {
          src_depth = visual_video_depth_from_bpp (std::atoi (argv[1]));
          dst_depth = visual_video_depth_from_bpp (std::atoi (argv[2]));

          if (src_depth == VISUAL_VIDEO_DEPTH_NONE || dst_depth == VISUAL_VIDEO_DEPTH_NONE) {
              throw std::invalid_argument ("Invalid bit depths specified");
          }

          if (src_depth == dst_depth) {
              throw std::invalid_argument ("Bit depths are the same, Nothing to test!\n");
          }

          argc -= 2; argv += 2;
      }

      return std::make_unique<VideoConvertDepthBench> (width, height, src_depth, dst_depth);
  }

} // anonymous

int main (int argc, char **argv)
{
    try {
        LV::System::init (argc, argv);

        unsigned int max_runs = 1000;

        if (argc > 1) {
            int value = std::atoi (argv[1]);
            if (value <= 0) {
                throw std::invalid_argument ("Number of runs is non-positive");
            }

            max_runs = value;

            argc--; argv++;
        }

        auto benchmark = make_benchmark (argc, argv);
        LV::Tools::run_benchmark (*benchmark, max_runs);
    }
    catch (std::exception& error) {
        std::cerr << "Exception caught: " << error.what () << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown exception caught\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
