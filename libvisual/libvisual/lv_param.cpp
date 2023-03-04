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

#include "config.h"
#include "lv_param.h"
#include "lv_common.h"
#include "lv_util.hpp"
#include <map>
#include <memory>
#include <list>
#include <stdexcept>
#include <string>
#include <cstdarg>

namespace LV
{
  class Closure
  {
  public:

      void           (*func) ();
      void*          data;
      VisDestroyFunc destroy_func;

      Closure (void* func, void* data, VisDestroyFunc destroy_func);

      ~Closure ();

      // Non-copyable
      Closure (Closure const&) = delete;
      Closure& operator= (Closure const&) = delete;
  };

  class Param
  {
  public:

      typedef std::list<std::unique_ptr<Closure>> HandlerList;

      ParamList*     parent;
      std::string    name;
      std::string    description;
      VisParamValue* value;
      VisParamValue* default_value;
      HandlerList    changed_handlers;

      std::unique_ptr<Closure> validator;

      Param (std::string const& name,
             std::string const& description,
             VisParamType       type,
             void*              default_value,
             Closure*           validator);

      ~Param ();

      // Non-copyable
      Param (Param const&) = delete;
      Param& operator= (Param const&) = delete;

      Closure* add_callback (VisParamChangedFunc func,
                             void*               priv,
                             VisDestroyFunc      destroy_func);

      bool remove_callback (Closure* to_remove);

      void changed ();

      void notify_callbacks ();
  };

  class ParamList::Impl
  {
  public:

      using Entries = std::map<std::string, std::unique_ptr<Param>, std::less<>>;

      Entries     entries;
      EventQueue* event_queue;

      Impl ();

      ~Impl ();
  };

  namespace {

    int validate_param_value (VisParamValue *value, Closure *validator)
    {
        return (* (VisParamValidateFunc) validator->func) (value, validator->data);
    }

  } // anonymous namespace

  // LV::Closure implementation

  Closure::Closure (void *func_, void *data_, VisDestroyFunc destroy_func_)
      : func         {reinterpret_cast<void(*)()> (func_)}
      , data         {data_}
      , destroy_func {destroy_func_}
  {
      // nothing
  }

  Closure::~Closure ()
  {
      if (data && destroy_func) {
          destroy_func (data);
      }
  }

  // LV::ParamList implementation

  ParamList::Impl::Impl ()
      : event_queue {nullptr}
  {
      // nothing
  }

  ParamList::Impl::~Impl ()
  {
      // nothing
  }

  ParamList::ParamList ()
      : m_impl {new Impl}
  {
      // nothing
  }

  ParamList::~ParamList ()
  {
      // nothing
  }

  ParamList::ParamList (std::initializer_list<Param*> params)
      : m_impl {new Impl}
  {
      for (auto param : params) {
          add (param);
      }
  }

  ParamList::ParamList (ParamList&& list)
      : m_impl {std::move (list.m_impl)}
  {
      // nothing
  }

  void ParamList::set_event_queue (EventQueue& event_queue)
  {
      m_impl->event_queue = &event_queue;
  }

  EventQueue* ParamList::get_event_queue () const
  {
      return m_impl->event_queue;
  }

  void ParamList::add (Param&& param)
  {
      add (&param);
  }

  void ParamList::add (Param* param)
  {
      visual_return_if_fail (param != nullptr);

      param->parent = this;

      m_impl->entries.emplace (param->name, std::unique_ptr<Param> {param});
  }

  bool ParamList::remove (std::string_view name)
  {
      auto entry = m_impl->entries.find (name);
      if (entry != m_impl->entries.end ()) {
          m_impl->entries.erase (entry);
          return true;
      }

      return false;
  }

  Param* ParamList::get (std::string_view name) const
  {
      auto entry = m_impl->entries.find (name);
      if (entry != m_impl->entries.end ()) {
          return entry->second.get ();
      }

      return nullptr;
  }

  // LV::Param Implementation

  Param::Param (std::string const& name_,
                std::string const& description_,
                VisParamType       type_,
                void*              default_value_,
                Closure*           validator_)
      : parent      (nullptr)
      , name        (name_)
      , description (description_)
      , validator   (validator_)
  {
      if (validator) {
          VisParamValue test_value;
          visual_param_value_init (&test_value, type_, default_value_);

          if (!validate_param_value (&test_value, validator.get ())) {
              throw std::runtime_error {"Default value failed to validate"};
          }
      }

      value = visual_param_value_new (type_, default_value_);
      default_value = visual_param_value_new (type_, default_value_);
  }

  Param::~Param ()
  {
      visual_param_value_free (value);
      visual_param_value_free (default_value);
  }

  Closure* Param::add_callback (VisParamChangedFunc func,
                                void*               priv,
                                VisDestroyFunc      destroy_func)
  {
      visual_return_val_if_fail (func != nullptr, nullptr);

      auto closure = new Closure {reinterpret_cast<void*> (func), priv, destroy_func};
      changed_handlers.emplace_back (closure);

      return closure;
  }

  bool Param::remove_callback (Closure* to_remove)
  {
      visual_return_val_if_fail (to_remove != nullptr, false);

      auto entry = std::find_if (changed_handlers.begin (),
                                 changed_handlers.end (),
                                 [&] (std::unique_ptr<Closure> const& handler) {
                                     return handler.get () == to_remove;
                                 });

      if (entry != changed_handlers.end ()) {
          changed_handlers.erase (entry);
          return true;
      }

      return false;
  }

  void Param::changed ()
  {
      if (!parent)
          return;

      auto event_queue = parent->get_event_queue ();
      if (event_queue) {
          visual_event_queue_add (event_queue, visual_event_new_param (this));
      }

      notify_callbacks ();
  }

  void Param::notify_callbacks ()
  {
      for (auto const& handler : changed_handlers) {
          (*(VisParamChangedFunc) (handler->func)) (this, handler->data);
      }
  }

} // LV namespace


// C API

// VisClosure implementation

VisClosure *visual_closure_new  (void *func, void *data, VisDestroyFunc destroy_func)
{
    return new LV::Closure {func, data, destroy_func};
}

void visual_closure_free (VisClosure *self)
{
    delete self;
}

// VisParamList implementation

VisParamList *visual_param_list_new  (void)
{
    return new LV::ParamList;
}

void visual_param_list_free (VisParamList *self)
{
    delete self;
}

void visual_param_list_set_event_queue (VisParamList *self, VisEventQueue* event_queue)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (event_queue != nullptr);

    self->set_event_queue (*event_queue);
}

VisEventQueue* visual_param_list_get_event_queue (VisParamList *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return self->get_event_queue ();
}

void visual_param_list_add (VisParamList *self, VisParam *param)
{
    visual_return_if_fail (self  != nullptr);
    visual_return_if_fail (param != nullptr);

    self->add (param);
    delete param;
}

void visual_param_list_add_array (VisParamList *self, VisParam **params, unsigned int nparams)
{
    visual_return_if_fail (self != nullptr);

    for (unsigned int i = 0; i < nparams; i++) {
        self->add (params[i]);
    }
}

void visual_param_list_add_many (VisParamList *self, ...)
{
    visual_return_if_fail (self != nullptr);

    va_list args;

    va_start (args, self);

    auto param = va_arg (args, VisParam *);

    while (param) {
        self->add (param);
        param = va_arg (args, VisParam *);
    }

    va_end (args);
}

VisParam **visual_param_list_get_entries (VisParamList *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    // FIXME: Implement

    return nullptr;
}

int visual_param_list_remove (VisParamList *self, const char *name)
{
    visual_return_val_if_fail (self != nullptr, FALSE);
    visual_return_val_if_fail (name != nullptr, FALSE);

    return self->remove (name);
}

VisParam *visual_param_list_get (VisParamList *self, const char *name)
{
    visual_return_val_if_fail (self != nullptr, nullptr);
    visual_return_val_if_fail (name != nullptr, nullptr);

    return self->get (name);
}

// VisParam implementation

VisParam *visual_param_new (const char * name,
                            const char * description,
                            VisParamType type,
                            void *       default_value,
                            VisClosure * validator)
{
    try {
        return new LV::Param (name, description, type, default_value, validator);
    }
    catch (std::exception& error) {
        return nullptr;
    }
}

void visual_param_free (VisParam *self)
{
    delete self;
}

int visual_param_has_name (VisParam *self, const char *name)
{
    visual_return_val_if_fail (self != nullptr, FALSE);

    return self->name == name;
}

VisParamType visual_param_get_type (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, VISUAL_PARAM_TYPE_NONE);

    return self->value->type;
}

const char *visual_param_get_name (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return LV::string_to_c (self->name);
}

const char *visual_param_get_description (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return LV::string_to_c (self->description);
}

void visual_param_set_value_bool (VisParam *self, int value)
{
    visual_return_if_fail (self != nullptr);

    visual_param_value_set_bool (self->value, value);
}

void visual_param_set_value_integer (VisParam *self, int value)
{
    visual_return_if_fail (self != nullptr);

    visual_param_value_set_integer (self->value, value);
}

void visual_param_set_value_float (VisParam *self, float value)
{
    visual_return_if_fail (self != nullptr);

    visual_param_value_set_float (self->value, value);
}

void visual_param_set_value_double (VisParam *self, double value)
{
    visual_return_if_fail (self != nullptr);

    visual_param_value_set_double (self->value, value);
}

void visual_param_set_value_string (VisParam *self, const char *string)
{
    visual_return_if_fail (self != nullptr);

    visual_param_value_set_string (self->value, string);
}

void visual_param_set_value_color (VisParam *self, VisColor *color)
{
    visual_return_if_fail (self != nullptr);

    visual_param_value_set_color (self->value, color);
}

void visual_param_set_value_palette (VisParam *self, VisPalette *palette)
{
    visual_return_if_fail (self != nullptr);

    visual_param_value_set_palette (self->value, palette);
}

int visual_param_get_value_bool (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, 0);

    return visual_param_value_get_bool (self->value);
}

int visual_param_get_value_integer (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, 0);

    return visual_param_value_get_integer (self->value);
}

float visual_param_get_value_float (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, 0.0f);

    return visual_param_value_get_float (self->value);
}

double visual_param_get_value_double (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, 0.0);

    return visual_param_value_get_double (self->value);
}

const char *visual_param_get_value_string (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, 0);

    return visual_param_value_get_string (self->value);
}

VisColor *visual_param_get_value_color (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return visual_param_value_get_color (self->value);
}

VisPalette *visual_param_get_value_palette (VisParam *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return visual_param_value_get_palette (self->value);
}
