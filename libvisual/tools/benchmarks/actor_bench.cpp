#include "benchmark.hpp"
#include <libvisual/libvisual.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <cstdlib>

using namespace std::string_literals;

namespace {

  class ActorBench
      : public LV::Tools::Benchmark
  {
  public:

      ActorBench (std::string_view actor_name, unsigned int width, unsigned height, VisVideoDepth depth, bool forced_depth)
          : Benchmark { "ActorBench" }
          , m_actor { LV::Actor::load (actor_name) }
      {
          (void)width;
          (void)height;

          if (!m_actor) {
              throw std::invalid_argument {"Cannot load actor '"s + std::string {actor_name}};
          }

          m_actor->realize ();

          if (!forced_depth) {
              auto supported_depths = m_actor->get_supported_depths ();
              depth = visual_video_depth_get_highest (supported_depths);
          }

          auto m_dest = LV::Video::create (640, 400, depth);

          m_actor->set_video (m_dest);
          m_actor->video_negotiate (depth, false, false);
      }

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++)
              m_actor->run (m_audio);
      }

      virtual ~ActorBench ()
      {
          // nothing
      }

  private:

      LV::ActorPtr m_actor;
      LV::Audio    m_audio;
  };

  std::unique_ptr<ActorBench> make_benchmark (int& argc, char**& argv)
  {
      std::string   actor_name   = "blursk";
      unsigned int  width        = 640;
      unsigned int  height       = 480;
      VisVideoDepth depth        = VISUAL_VIDEO_DEPTH_32BIT;
      bool          forced_depth = false;

      if (argc > 1) {
          actor_name = argv[1];
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

          forced_depth = true;

          argc--; argv++;
      }

      return std::make_unique<ActorBench> (actor_name, width, height, depth, forced_depth);
  }

} // anonymous

int main (int argc, char **argv)
{
    try {
        LV::System::init (argc, argv);

        unsigned int max_runs = 500;

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
