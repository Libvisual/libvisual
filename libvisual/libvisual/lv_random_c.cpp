#include "lv_random.h"
#include "lv_common.h"

using namespace LV;

extern "C" {

  typedef RandomContext VisRandomContext;

  VisRandomContext *visual_random_context_new (VisRandomSeed seed)
  {
      return new RandomContext (seed);
  }

  void visual_random_context_free (VisRandomContext *rcontext)
  {
      delete rcontext;
  }

  void visual_random_context_set_seed (VisRandomContext *rcontext, VisRandomSeed seed)
  {
      visual_return_if_fail (rcontext != nullptr);

      rcontext->set_seed (seed);
  }

  uint32_t visual_random_context_int (VisRandomContext *rcontext)
  {
      visual_return_val_if_fail (rcontext != nullptr, 0);

      return rcontext->get_int ();
  }

  uint32_t visual_random_context_int_range (VisRandomContext *rcontext, unsigned int min, unsigned int max)
  {
      visual_return_val_if_fail (rcontext != nullptr, 0);

      return rcontext->get_int (min, max);
  }

  double visual_random_context_double (VisRandomContext *rcontext)
  {
      visual_return_val_if_fail (rcontext != nullptr, 0.0);

      return rcontext->get_double ();
  }

  float visual_random_context_float (VisRandomContext *rcontext)
  {
      visual_return_val_if_fail (rcontext != nullptr, 0.0);

      return rcontext->get_float ();
  }

} // C extern
