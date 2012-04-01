#include "config.h"
#include "lv_libvisual.h"
#include "lv_error.h"

extern "C" {

  const char *visual_get_version ()
  {
      return LV::System::instance()->get_version ().c_str ();
  }

  int visual_get_api_version ()
  {
      return LV::System::instance()->get_api_version ();
  }

  VisParamContainer *visual_get_params ()
  {
      return LV::System::instance()->get_params ();
  }

  int visual_init (int *argc, char ***argv)
  {
      LV::System::init(*argc, *argv);

      return VISUAL_OK;
  }

  int visual_is_initialized ()
  {
      return LV::System::instance() != 0;
  }

  int visual_quit ()
  {
      LV::System::deinit ();

      return VISUAL_OK;
  }

} // extern C
