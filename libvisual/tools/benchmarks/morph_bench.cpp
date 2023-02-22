#include "benchmark.hpp"
#include <libvisual/libvisual.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <cstdlib>

namespace {

  using namespace std::string_literals;

  class MorphBench
      : public LV::Tools::Benchmark
  {
  public:

      MorphBench (std::string_view morph_name, unsigned int width, unsigned int height, VisVideoDepth depth)
          : Benchmark { "MorphBench" }
          , m_morph { LV::Morph::load (morph_name) }
      {
          if (!m_morph) {
              throw std::invalid_argument {"Cannot load morph "s + std::string {morph_name}};
          }

          m_morph->realize ();

          m_dest = LV::Video::create (width, height, depth);
          m_src1 = LV::Video::create (width, height, depth);
          m_src2 = LV::Video::create (width, height, depth);

          m_morph->set_video (m_dest);
      }

      virtual void operator() (unsigned int max_runs)
      {
          float progress = 0.0;

          for (unsigned int i = 0; i < max_runs; i++) {
              m_morph->set_progress (progress);
              m_morph->run (m_audio, m_src1, m_src2);

              progress = std::min (progress + 0.1, 1.0);
          }
      }

      virtual ~MorphBench ()
      {
          // nothing
      }

  private:

      LV::MorphPtr m_morph;
      LV::Audio    m_audio;
      LV::VideoPtr m_dest;
      LV::VideoPtr m_src1;
      LV::VideoPtr m_src2;
  };

  std::unique_ptr<MorphBench> make_benchmark (int& argc, char** argv)
  {
      std::string   morph_name = "slide_up";
      unsigned int  width      = 640;
      unsigned int  height     = 480;
      VisVideoDepth depth      = VISUAL_VIDEO_DEPTH_32BIT;

      if (argc > 1) {
          morph_name = argv[1];
          argc--; argv++;
      }

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

      return std::make_unique<MorphBench> (morph_name, width, height, depth);
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
