#include "config.h"
#include "lv_buffer.h"
#include "lv_common.h"

VisBuffer *visual_buffer_new_wrap_data (void *data, visual_size_t size, int own)
{
    auto self = LV::Buffer::wrap (data, size, own);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

VisBuffer *visual_buffer_new_allocate (visual_size_t size)
{
    auto self = LV::Buffer::create (size);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

VisBuffer *visual_buffer_clone (VisBuffer *source)
{
    visual_return_val_if_fail (source != nullptr, nullptr);

    auto self = LV::Buffer::create ();
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
        self->copy (LV::BufferPtr (source));
    }

    return self.get ();
}

void *visual_buffer_get_data (VisBuffer *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return self->get_data ();
}

void *visual_buffer_get_data_offset (VisBuffer *self, visual_size_t offset)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return self->get_data (offset);
}

visual_size_t visual_buffer_get_size (VisBuffer *self)
{
    visual_return_val_if_fail (self != nullptr, 0);

    return self->get_size ();
}

int visual_buffer_is_allocated (VisBuffer *self)
{
    visual_return_val_if_fail (self != nullptr, FALSE);

    return self->is_allocated ();
}

void visual_buffer_copy_to (VisBuffer *self, VisBuffer *dest)
{
   visual_return_if_fail (self != nullptr);
   visual_return_if_fail (dest != nullptr);

   self->copy_to (dest);
}

void visual_buffer_copy_to_data (VisBuffer *self, void *dest, visual_size_t size)
{
   visual_return_if_fail (self != nullptr);
   visual_return_if_fail (dest != nullptr);

   self->copy_to (dest, size);
}

void visual_buffer_put (VisBuffer *self, VisBuffer *src, visual_size_t offset)
{
   visual_return_if_fail (self != nullptr);

   self->put (LV::BufferPtr (src), offset);
}

void visual_buffer_put_data (VisBuffer *self, const void *data, visual_size_t size, visual_size_t offset)
{
   visual_return_if_fail (self != nullptr);

   self->put (data, size, offset);
}

void visual_buffer_fill (VisBuffer *self, uint8_t value)
{
   visual_return_if_fail (self != nullptr);

   self->fill (value);
}

void visual_buffer_fill_with_pattern (VisBuffer *self, const void *data, visual_size_t size)
{
   visual_return_if_fail (self != nullptr);

   self->fill_with_pattern (data, size);
}

void visual_buffer_ref (VisBuffer *self)
{
    visual_return_if_fail (self != nullptr);

    LV::intrusive_ptr_add_ref (self);
}

void visual_buffer_unref (VisBuffer *self)
{
    visual_return_if_fail (self != nullptr);

    LV::intrusive_ptr_release (self);
}
