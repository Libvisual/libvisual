/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
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

#ifndef _LV_PARAM_H
#define _LV_PARAM_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_color.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_event.h>
#include <libvisual/lv_param_value.h>
#include <libvisual/lv_param_validators.h>

/**
 * @defgroup VisParam VisParam
 * @{
 */

#ifdef __cplusplus

#include <initializer_list>
#include <memory>
#include <string_view>

namespace LV {

  class Param;

  class LV_API ParamList
  {
  public:

      /**
       * Creates a new empty ParamList.
       */
      ParamList ();

      /**
       * Creates a new ParamList with entries drawn from an initializer list.
       */
      ParamList (std::initializer_list<Param*> params);

      /**
       * Move constructor
       */
      ParamList (ParamList&& list);

      /** Destructor */
      ~ParamList ();

      /**
       * Adds a new parameter entry.
       *
       * @param param Parameter to add
       */
      void add (Param&& param);

      /**
       * Adds a new parameter entry.
       *
       * @note Entry will be managed and automatically freed.
       *
       * @param param Parameter to add
       */
      void add (Param* param);

      /**
       * Adds a list of parameters.
       *
       * @param params List of parameters
       */
      template <class Container>
      void add (Container const& params)
      {
          for (auto const& param : params) {
              add (param);
          }
      }

      /**
       * Removes a parameter by name.
       *
       * @param name Name of parameter to remove
       *
       * @return true on success, false otherwise
       */
      bool remove (std::string_view name);

      /**
       * Returns a parameter by name
       *
       * @param name Name of parameter to return
       *
       * @return Parameter of the given name, or nullptr if no such
       *         parameter exists
       */
      Param* get (std::string_view name) const;

      /**
       * Sets the event queue.
       *
       * @param event_queue Event queue to hook send parameter update events to
       */
      void set_event_queue (EventQueue& event_queue);

      /**
       * Returns the event queue.
       *
       * @see set_event_queue()
       *
       * @return Event queue if set, nullptr otherwise
       */
      EventQueue* get_event_queue () const;

  private:

      class Impl;
      std::unique_ptr<Impl> m_impl;
  };

} // LV namespace

typedef LV::Param     VisParam;
typedef LV::ParamList VisParamList;

#else

typedef struct _VisParam     VisParam;
typedef struct _VisParamList VisParamList;

#endif

#define VISUAL_PARAM(obj)       ((VisParam *) (obj))
#define VISUAL_PARAM_LIST(obj)  ((VisParamList *) (obj))

typedef int  (*VisParamValidateFunc) (VisParamValue *value, void *priv);
typedef void (*VisParamChangedFunc)  (VisParam *param, void *priv);
typedef void (*VisDestroyFunc)       (void *data);

LV_BEGIN_DECLS

/* VisClosure API */

LV_NODISCARD LV_API VisClosure *visual_closure_new (void *func, void *data, VisDestroyFunc destroy_func);

LV_API void visual_closure_free (VisClosure *self);

/* VisParamList API */

LV_NODISCARD LV_API VisParamList *visual_param_list_new  (void);

LV_API void visual_param_list_free (VisParamList *self);

LV_API void         visual_param_list_add         (VisParamList *list, VisParam *param);
LV_API void         visual_param_list_add_array   (VisParamList *list, VisParam **params, unsigned int nparams);
LV_API void         visual_param_list_add_many    (VisParamList *list, ...);
LV_API VisParam **  visual_param_list_get_entries (VisParamList *list);
LV_API int          visual_param_list_remove      (VisParamList *list, const char *name);
LV_API VisParam *   visual_param_list_get         (VisParamList *list, const char *name);

LV_API void           visual_param_list_set_event_queue (VisParamList *list, VisEventQueue *eventqueue);
LV_API VisEventQueue *visual_param_list_get_event_queue (VisParamList *list);

/* VisParam API */

/**
 * Creates a new parameter entry.
 *
 * @param name          Name
 * @param type          Type
 * @param description   Description
 * @param default_value Default Value
 * @param validator     Validator function (closure)
 *
 * @return A newly allocated VisParam
 */
LV_NODISCARD LV_API VisParam *visual_param_new (const char * name,
                                                const char * description,
                                                VisParamType type,
                                                void *       default_value,
                                                VisClosure * validator);

/**
 * Frees a parameter entry.
 *
 * @param param Param to free
 */
LV_API void visual_param_free (VisParam *param);

/**
 * Adds a change notification callback.
 *
 * @param param        VisParam object to add notification callback
 * @param func         The notification callback, called on value changes
 * @param data         Additional data to be passed to callback
 * @param destroy_func Function called to destroy data (may be NULL)
 */
LV_API VisClosure *visual_param_add_callback (VisParam *          param,
                                              VisParamChangedFunc func,
                                              void *              data,
                                              VisDestroyFunc      destroy_func);

/**
 * Removes a change notification callback.
 *
 * @param param   VisParam object to remove callback from
 * @param closure The closure pointer that was given by the visual_param_add_callback().
 *
 * @return TRUE on successful removal, FALSE otherwise
 */
LV_API int visual_param_remove_callback (VisParam *param, VisClosure *closure);

/**
 * Notifies all callbacks.
 *
 * @param param VisParam object
 */
LV_API void visual_param_notify_callbacks (VisParam *param);

/**
 * Emits an event in the eventqueue of the parent VisParamList
 *
 * @param param VisParam object that has changed.
 */
LV_API void visual_param_changed (VisParam *param);

/**
 * Determines if the VisParam is of a given name.
 *
 * @param param VisParam object
 * @param name  Name to check against
 *
 * @return TRUE if name matches, FALSE otherwise
 */
LV_API int visual_param_has_name (VisParam *param, const char *name);

LV_API const char * visual_param_get_name        (VisParam *param);
LV_API VisParamType visual_param_get_type        (VisParam *param);
LV_API const char * visual_param_get_description (VisParam *param);

LV_API void visual_param_set_value         (VisParam *param, VisParamValue *value);
LV_API void visual_param_set_value_bool    (VisParam *param, int boolean);
LV_API void visual_param_set_value_integer (VisParam *param, int integer);
LV_API void visual_param_set_value_float   (VisParam *param, float flt);
LV_API void visual_param_set_value_double  (VisParam *param, double dbl);
LV_API void visual_param_set_value_string  (VisParam *param, const char *string);
LV_API void visual_param_set_value_color   (VisParam *param, VisColor *color);
LV_API void visual_param_set_value_palette (VisParam *param, VisPalette *pal);

LV_API int         visual_param_get_value_bool    (VisParam *param);
LV_API int         visual_param_get_value_integer (VisParam *param);
LV_API float       visual_param_get_value_float   (VisParam *param);
LV_API double      visual_param_get_value_double  (VisParam *param);
LV_API const char *visual_param_get_value_string  (VisParam *param);
LV_API VisColor *  visual_param_get_value_color   (VisParam *param);
LV_API VisPalette *visual_param_get_value_palette (VisParam *param);

/* Type-safe variants of visual_param_new() */

#define _LV_DEFINE_PARAM_NEW(func,ctype,type,marshal) \
  static inline VisParam *visual_param_new_##func (const char *name,          \
                                                   const char *description,   \
                                                   ctype       default_value, \
                                                   VisClosure *validator)     \
  { return visual_param_new (name, description, VISUAL_PARAM_TYPE_##type, _LV_PARAM_MARSHAL_##marshal (default_value), validator); }

_LV_DEFINE_PARAM_NEW (bool   , int               , BOOL   , INTEGER)
_LV_DEFINE_PARAM_NEW (integer, int               , INTEGER, INTEGER)
_LV_DEFINE_PARAM_NEW (float  , float             , FLOAT  , FLOAT)
_LV_DEFINE_PARAM_NEW (double , double            , DOUBLE , DOUBLE)
_LV_DEFINE_PARAM_NEW (string , const char *      , STRING , POINTER)
_LV_DEFINE_PARAM_NEW (color  , const VisColor *  , COLOR  , POINTER)
_LV_DEFINE_PARAM_NEW (palette, const VisPalette *, PALETTE, POINTER)

static inline VisParam *visual_param_new_color_rgb (const char *name,
                                                    const char *description,
                                                    uint8_t     red,
                                                    uint8_t     green,
                                                    uint8_t     blue,
                                                    VisClosure *validator)
{
    VisColor color = { red, green, blue, 255 };
    return visual_param_new_color (name, description, &color, validator);
}

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_PARAM_H */
