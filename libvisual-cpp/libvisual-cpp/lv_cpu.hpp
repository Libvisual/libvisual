// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_cpu.hpp,v 1.3 2006-09-12 01:50:35 descender Exp $
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#ifndef LVCPP_CPU_HPP
#define LVCPP_CPU_HPP

#include <libvisual/lv_cpu.h>
#include <libvisual-cpp/lv_object.hpp>

namespace Lv
{

  namespace CPU
  {
    // I personally like names like Lv::CPU::Type::MIPS, but I somehow
    // have this feeling that someone would be using macros like X86,
    // ALPHA, etc. and ruining the day with weird errors.

    enum Type
    {
        TYPE_MIPS    = VISUAL_CPU_TYPE_MIPS,
        TYPE_ALPHA   = VISUAL_CPU_TYPE_SPARC,
        TYPE_X86     = VISUAL_CPU_TYPE_X86,
        TYPE_POWERPC = VISUAL_CPU_TYPE_POWERPC,
        TYPE_OTHER   = VISUAL_CPU_TYPE_OTHER
    };

    class Caps
        : public Object
    {
    public:

        Caps ()
            : Object (vis_cpu_to_object (visual_cpu_get_caps ()))
        {
            // Increase reference count because visual_cpu_get_caps()
            // returns a globally shared object.
            ref ();
        }

        inline Type type () const
        {
            return Type (vis_cpu ().type);
        }

        inline int n_cpus () const
        {
            return vis_cpu ().nrcpu;
        }

        inline int x86_cpu_type () const
        {
            return vis_cpu ().x86cpuType;
        }

        inline int cacheline () const
        {
            return vis_cpu ().cacheline;
        }

        inline bool has_tsc () const
        {
            return vis_cpu ().hasTSC;
        }

        inline bool has_mmx () const
        {
            return vis_cpu ().hasMMX;
        }

        inline bool has_mmx2 () const
        {
            return vis_cpu ().hasMMX2;
        }

        inline bool has_sse () const
        {
            return vis_cpu ().hasSSE;
        }

        inline bool has_sse2 () const
        {
            return vis_cpu ().hasSSE2;
        }

        inline bool has_3dnow () const
        {
            return vis_cpu ().has3DNow;
        }

        inline bool has_3dnow_ext () const
        {
            return vis_cpu ().has3DNowExt;
        }

        inline bool has_altivec () const
        {
            return vis_cpu ().hasAltiVec;
        }

        inline bool enabled_tsc () const
        {
            return vis_cpu ().enabledTSC;
        }

        inline bool enabled_mmx () const
        {
            return vis_cpu ().enabledMMX;
        }

        inline bool enabled_mmx2 () const
        {
            return vis_cpu ().enabledMMX2;
        }

        inline bool enabled_sse () const
        {
            return vis_cpu ().enabledSSE;
        }

        inline bool enabled_sse2 () const
        {
            return vis_cpu ().enabledSSE2;
        }

        inline bool enabled_3dnow () const
        {
            return vis_cpu ().enabled3DNow;
        }

        inline bool enabled_3dnow_ext () const
        {
            return vis_cpu ().enabled3DNowExt;
        }

        inline bool enabled_altivec () const
        {
            return vis_cpu ().enabledAltiVec;
        }

        inline const VisCPU& vis_cpu () const
        {
            return reinterpret_cast<const VisCPU&> (vis_object ());
        }

        inline VisCPU& vis_cpu ()
        {
            return reinterpret_cast<VisCPU&> (vis_object ());
        }

    private:

        static inline VisObject *vis_cpu_to_object (VisCPU *cpu)
        {
            return reinterpret_cast<VisObject *> (cpu);
        }

        // No VisCPU copying function available, so we disable
        // assignment and copying for now until further clarification.

        Caps (const Caps& other);
        const Caps& operator = (const Caps& other);
    };

    inline void init ()
    {
        visual_cpu_initialize ();
    }

    // FIXME: Need copy constructors and/or assignment operators  to

    // inline Caps get_caps ()
    // {
    //   return visual_cpu_get_caps ();
    // }

    inline bool get_3dnow ()
    {
        return visual_cpu_get_3dnow ();
    }

    inline bool get_3dnow2 ()
    {
        return visual_cpu_get_3dnow2 ();
    }

    inline bool get_mmx ()
    {
        return visual_cpu_get_mmx ();
    }

    inline bool get_mmx2 ()
    {
        return visual_cpu_get_mmx2 ();
    }

    inline bool get_sse ()
    {
        return visual_cpu_get_sse ();
    }

    inline bool get_sse2 ()
    {
        return visual_cpu_get_sse2 ();
    }

    inline bool get_altivec ()
    {
        return visual_cpu_get_altivec ();
    }

    // misleading name =/
    inline bool get_tsc ()
    {
        return visual_cpu_get_tsc ();
    }

    inline int set_3dnow (bool enabled)
    {
        return visual_cpu_set_3dnow (enabled);
    }

    inline int set_3dnow2 (bool enabled)
    {
        return visual_cpu_set_3dnow2 (enabled);
    }

    inline int set_mmx (bool enabled)
    {
        return visual_cpu_set_mmx (enabled);
    }

    inline int set_mmx2 (bool enabled)
    {
        return visual_cpu_set_mmx2 (enabled);
    }

    inline int set_sse (bool enabled)
    {
        return visual_cpu_set_sse (enabled);
    }

    inline int set_sse2 (bool enabled)
    {
        return visual_cpu_set_sse2 (enabled);
    }

    inline int set_altivec (bool enabled)
    {
        return visual_cpu_set_altivec (enabled);
    }

    // another misleading name =/
    inline int set_tsc (bool enabled)
    {
        return visual_cpu_set_tsc (enabled);
    }

  } // namespace CPU

} // namespace Lv

#endif // #ifndef LVCPP_CPU_HPP
