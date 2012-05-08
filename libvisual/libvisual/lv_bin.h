/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _LV_BIN_H
#define _LV_BIN_H

#include <libvisual/lv_actor.h>
#include <libvisual/lv_input.h>
#include <libvisual/lv_morph.h>
#include <libvisual/lv_video.h>
#include <libvisual/lv_time.h>

/**
 * @defgroup VisBin VisBin
 * @{
 */

#define VISUAL_BIN(obj) (VISUAL_CHECK_CAST ((obj), VisBin))

typedef enum {
	VISUAL_SWITCH_STYLE_DIRECT,
	VISUAL_SWITCH_STYLE_MORPH
} VisBinSwitchStyle;

typedef enum {
	VISUAL_BIN_DEPTH_LOWEST,
	VISUAL_BIN_DEPTH_HIGHEST
} VisBinDepth;

#ifdef __cplusplus

#include <libvisual/lv_scoped_ptr.hpp>
#include <string>

namespace LV {

  class LV_API Bin
  {
  public:

	  Bin ();

	  ~Bin ();

	  void realize ();

	  void set_actor (VisActor* actor);

	  VisActor* get_actor () const;

	  void set_input (VisInput* input);

	  VisInput* get_input () const;

	  void set_morph (std::string const& morphname);

	  VisMorph* get_morph () const;

	  void connect (VisActor* actor, VisInput* input);
	  void connect (std::string const& actname, std::string const& inname);

	  void sync (bool noevent);

	  void set_video (VideoPtr const& video);

	  void set_supported_depth (VisVideoDepth depthflag);

      VisVideoDepth get_supported_depth () const;

	  void set_preferred_depth (VisBinDepth depthpreferred);

	  void set_depth (VisVideoDepth depth);

	  VisVideoDepth get_depth () const;

      // FIXME: Added to make lv-tool compile. Remove this.
      void force_actor_depth (VisVideoDepth depth);

	  bool depth_changed ();

	  Palette const& get_palette () const;

	  void switch_actor (std::string const& actname);
	  void switch_actor (VisActor *actor);

	  void switch_finalize ();

	  void switch_set_style (VisBinSwitchStyle style);

	  void switch_set_steps (int steps);

	  void switch_set_automatic (bool automatic);

	  void switch_set_rate (float rate);

	  void switch_set_mode (VisMorphMode mode);

	  void switch_set_time (Time const& time);

	  void run ();

  private:

	  class Impl;

	  ScopedPtr<Impl> m_impl;

	  Bin (Bin const&);
	  Bin& operator= (Bin const&);
  };

} // LV namespace

#endif /* __cplusplus */

#ifdef __cplusplus
typedef LV::Bin VisBin;
#else
typedef struct _VisBin VisBin;
struct _VisBin;
#endif

LV_BEGIN_DECLS

LV_API VisBin *visual_bin_new (void);
LV_API void    visual_bin_free (VisBin *bin);

LV_API void visual_bin_realize (VisBin *bin);

LV_API void      visual_bin_set_actor (VisBin *bin, VisActor *actor);
LV_API VisActor *visual_bin_get_actor (VisBin *bin);

LV_API void      visual_bin_set_input (VisBin *bin, VisInput *input);
LV_API VisInput *visual_bin_get_input (VisBin *bin);

LV_API void      visual_bin_set_morph (VisBin *bin, const char *morphname);
LV_API VisMorph *visual_bin_get_morph (VisBin *bin);

LV_API void visual_bin_connect (VisBin *bin, VisActor *actor, VisInput *input);
LV_API void visual_bin_connect_by_names (VisBin *bin, const char *actname, const char *inname);

LV_API void visual_bin_sync (VisBin *bin, int noevent);

LV_API void visual_bin_set_video (VisBin *bin, VisVideo *video);

LV_API void          visual_bin_set_supported_depth (VisBin *bin, VisVideoDepth depthflag);
LV_API VisVideoDepth visual_bin_get_supported_depth (VisBin *bin);
LV_API void          visual_bin_set_preferred_depth (VisBin *bin, VisBinDepth depthpreferred);
LV_API void          visual_bin_set_depth (VisBin *bin, int depth);
LV_API VisVideoDepth visual_bin_get_depth (VisBin *bin);

LV_API int visual_bin_depth_changed (VisBin *bin);

LV_API const VisPalette* visual_bin_get_palette (VisBin *bin);

LV_API void visual_bin_switch_actor_by_name (VisBin *bin, const char *actname);
LV_API void visual_bin_switch_actor    (VisBin *bin, VisActor *actor);
LV_API void visual_bin_switch_finalize (VisBin *bin);
LV_API void visual_bin_switch_set_style (VisBin *bin, VisBinSwitchStyle style);
LV_API void visual_bin_switch_set_steps (VisBin *bin, int steps);
LV_API void visual_bin_switch_set_automatic (VisBin *bin, int automatic);
LV_API void visual_bin_switch_set_rate (VisBin *bin, float rate);
LV_API void visual_bin_switch_set_mode (VisBin *bin, VisMorphMode mode);
LV_API void visual_bin_switch_set_time (VisBin *bin, long sec, long usec);

LV_API void visual_bin_run (VisBin *bin);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_BIN_H */
