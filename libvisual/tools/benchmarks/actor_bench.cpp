#include "benchmark.hpp"
#include <libvisual/libvisual.h>
#include <libvisual/lv_util.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

namespace {

  class ActorBench
      : public LV::Tools::Benchmark
  {
  public:

      ActorBench (std::string const& actor_name, unsigned int width, unsigned height, VisVideoDepth depth, bool forced_depth)
          : Benchmark { "ActorBench" }
          , m_actor { visual_actor_new (actor_name.c_str ()) }
      {
          if (!m_actor) {
              throw std::invalid_argument ("Cannot load actor '" + actor_name);
          }

          visual_actor_realize (m_actor);

          if (!forced_depth) {
              auto supported_depths = visual_actor_get_supported_depths (m_actor);
              depth = visual_video_depth_get_highest (supported_depths);
          }

          auto m_dest = LV::Video::create (640, 400, depth);

          visual_actor_set_video (m_actor, m_dest.get ());
          visual_actor_video_negotiate (m_actor, depth, false, false);
      }

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++)
              visual_actor_run (m_actor, &m_audio);
      }

      virtual ~ActorBench ()
      {
          visual_object_unref (VISUAL_OBJECT (m_actor));
      }

  private:

      VisActor* m_actor;
      LV::Audio m_audio;
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
          depth = visual_video_depth_enum_from_value (std::atoi (argv[1]));
          if (depth == VISUAL_VIDEO_DEPTH_NONE) {
              throw std::invalid_argument ("Invalid video depth specified");
          }

          forced_depth = true;

          argc--; argv++;
      }

      return LV::make_unique<ActorBench> (actor_name, width, height, depth, forced_depth);
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
