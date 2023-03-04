/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
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

#include "config.h"
#include "lv_morph.h"
#include "lv_common.h"
#include "lv_plugin_registry.h"
#include <algorithm>
#include <stdexcept>
#include <string>

namespace LV {

  class Morph::Impl
  {
  public:

      VisPluginData* plugin;
      VideoPtr       dest;
      float          progress;
      Palette*       morphpal;
      Time           morphtime;
      Timer          timer;

      Impl ();

      ~Impl ();

      VisMorphPlugin* get_morph_plugin () const;
  };

  Morph::Impl::Impl ()
      : plugin   {nullptr}
      , progress {0}
      , morphpal {nullptr}
  {
      // nothing
  }

  Morph::Impl::~Impl ()
  {
      if (plugin) {
          visual_plugin_unload (plugin);
      }

      delete morphpal;
  }

  VisMorphPlugin* Morph::Impl::get_morph_plugin () const
  {
      return static_cast<VisMorphPlugin*> (visual_plugin_get_info (plugin)->plugin);
  }

  VisPluginData* Morph::get_plugin ()
  {
      return m_impl->plugin;
  }

  bool Morph::available(std::string_view name) {
      return LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_MORPH, name);
  }

  MorphPtr Morph::load (std::string_view name)
  {
      try {
          return {new Morph {name}, false};
      }
      catch (std::exception& error)
      {
          visual_log (VISUAL_LOG_ERROR, "%s", error.what ());
          return nullptr;
      }
  }

  Morph::Morph (std::string_view name)
      : m_impl      (new Impl)
      , m_ref_count (1)
  {
      if (!available (name)) {
          throw std::runtime_error {"Morph plugin not found"};
      }

      m_impl->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_MORPH, std::string {name}.c_str ());
      if (!m_impl->plugin) {
          throw std::runtime_error {"Failed to load morph plugin"};
      }

      m_impl->morphpal = new LV::Palette {256};
  }

  Morph::~Morph ()
  {
      // nothing
  }

  bool Morph::realize ()
  {
      return visual_plugin_realize (m_impl->plugin);
  }

  VisVideoDepth Morph::get_supported_depths ()
  {
      return m_impl->get_morph_plugin ()->vidoptions.depth;
  }

  VisVideoAttrOptions const* Morph::get_video_attribute_options ()
  {
      return &m_impl->get_morph_plugin ()->vidoptions;
  }

  void Morph::set_video (VideoPtr const& video)
  {
      m_impl->dest = video;
  }

  void Morph::set_time (Time const& time)
  {
      m_impl->morphtime = time;
  }

  void Morph::set_progress (float progress)
  {
      m_impl->progress = std::clamp (progress, 0.0f, 1.0f);
  }

  Palette const* Morph::get_palette ()
  {
      // FIXME: This should return nullptr if there is no palette
      return m_impl->morphpal;
  }

  bool Morph::is_done ()
  {
      if (m_impl->progress >= 1.0) {
          m_impl->timer.stop ();
          return true;
      }

      return false;
  }

  bool Morph::run (Audio const& audio, VideoPtr const& src1, VideoPtr const& src2)
  {
      visual_return_val_if_fail (src1, FALSE);
      visual_return_val_if_fail (src2, FALSE);

      auto morph_plugin = m_impl->get_morph_plugin ();

      // If we're morphing using the timer, start the timer
      if (!m_impl->timer.is_active ()) {
          m_impl->timer.start ();
      }

      if (morph_plugin->palette) {
          morph_plugin->palette (m_impl->plugin, m_impl->progress, const_cast<Audio*> (&audio), m_impl->morphpal, src1.get (), src2.get ());
      }
      else {
          auto const& src1_pal = src1->get_palette ();
          auto const& src2_pal = src2->get_palette ();

          //if (src1_pal && src2_pal) {
          m_impl->morphpal->blend (src1_pal, src2_pal, m_impl->progress);
          //}
      }

      morph_plugin->apply (m_impl->plugin, m_impl->progress, const_cast<Audio*> (&audio), m_impl->dest.get (), src1.get (), src2.get ());

      m_impl->dest->set_palette (*get_palette ());

      // Update morph progression

      double usec_elapsed = m_impl->timer.elapsed ().to_usecs ();
      double usec_morph   = m_impl->morphtime.to_usecs ();

      m_impl->progress = std::clamp (usec_elapsed / usec_morph, 0.0, 1.0);

      return true;
  }

} // LV namespace
