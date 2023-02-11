/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
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

typedef enum {
	VISUAL_BIN_DEPTH_LOWEST,
	VISUAL_BIN_DEPTH_HIGHEST
} VisBinDepth;

#ifdef __cplusplus

#include <memory>
#include <string_view>

namespace LV {

  class LV_API Bin
  {
  public:

	  Bin ();

	  Bin (Bin const&) = delete;

	  ~Bin ();

	  Bin& operator= (Bin const&) = delete;

	  void realize ();

	  ActorPtr const& get_actor () const;
	  InputPtr const& get_input () const;
	  MorphPtr const& get_morph () const;

	  void set_morph (std::string_view morph_name);

	  bool connect (std::string_view actor_name, std::string_view input_name);

	  void sync (bool noevent);

	  void set_video (VideoPtr const& video);

	  void set_supported_depth (VisVideoDepth depthflag);

      VisVideoDepth get_supported_depth () const;

	  void set_preferred_depth (VisBinDepth depthpreferred);

	  void set_depth (VisVideoDepth depth);

	  VisVideoDepth get_depth () const;

	  bool depth_changed ();

	  Palette const& get_palette () const;

	  void switch_actor (std::string_view actname);

	  void switch_finalize ();

	  void use_morph (bool use);

	  void switch_set_time (Time const& time);

	  void run ();

  private:

	  class Impl;

      const std::unique_ptr<Impl> m_impl;

      // FIXME: Remove
	  bool connect (ActorPtr const& actor, InputPtr const& input);
	  void switch_actor (ActorPtr const& actor);
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

LV_NODISCARD LV_API VisBin *visual_bin_new (void);

LV_API void visual_bin_free (VisBin *bin);

LV_API void visual_bin_realize (VisBin *bin);

LV_API VisActor *visual_bin_get_actor (VisBin *bin);
LV_API VisInput *visual_bin_get_input (VisBin *bin);

LV_API void      visual_bin_set_morph (VisBin *bin, const char *morphname);
LV_API VisMorph *visual_bin_get_morph (VisBin *bin);

LV_API void visual_bin_connect (VisBin *bin, const char *actname, const char *inname);

LV_API void visual_bin_sync (VisBin *bin, int noevent);

LV_API void visual_bin_set_video (VisBin *bin, VisVideo *video);

LV_API void          visual_bin_set_supported_depth (VisBin *bin, VisVideoDepth depthflag);
LV_API VisVideoDepth visual_bin_get_supported_depth (VisBin *bin);
LV_API void          visual_bin_set_preferred_depth (VisBin *bin, VisBinDepth depthpreferred);
LV_API void          visual_bin_set_depth (VisBin *bin, int depth);
LV_API VisVideoDepth visual_bin_get_depth (VisBin *bin);

LV_API int visual_bin_depth_changed (VisBin *bin);

LV_API const VisPalette* visual_bin_get_palette (VisBin *bin);

LV_API void visual_bin_switch_actor (VisBin *bin, const char *name);
LV_API void visual_bin_switch_finalize (VisBin *bin);
LV_API void visual_bin_switch_set_time (VisBin *bin, long sec, long usec);

LV_API void visual_bin_run (VisBin *bin);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_BIN_H */
