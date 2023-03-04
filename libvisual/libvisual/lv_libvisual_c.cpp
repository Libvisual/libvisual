#include "config.h"
#include "lv_libvisual.h"
#include "lv_error.h"

extern "C" {

  const char *visual_get_version ()
  {
      return LV::System::instance ()->get_version ().c_str ();
  }

  int visual_get_api_version ()
  {
      return LV::System::instance()->get_api_version ();
  }

  VisParamList *visual_get_params ()
  {
      return &LV::System::instance()->get_params ();
  }

  void visual_init (int *argc, char ***argv)
  {
      LV::System::init (*argc, *argv);
  }

  int visual_is_initialized ()
  {
      return LV::System::instance() != nullptr;
  }

  void visual_quit ()
  {
      LV::System::destroy ();
  }

  void visual_set_rng_seed (VisRandomSeed seed)
  {
      LV::System::instance()->set_rng_seed (seed);
  }

  VisRandomContext *visual_get_rng (void)
  {
      return &LV::System::instance()->get_rng ();
  }

} // extern C
