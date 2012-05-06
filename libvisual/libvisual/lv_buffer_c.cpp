#include "config.h"
#include "lv_buffer.h"
#include "lv_common.h"

VisBuffer *visual_buffer_new (void)
{
    return new LV::Buffer;
}

VisBuffer *visual_buffer_new_with_data (void *data, visual_size_t size)
{
    return new LV::Buffer (data, size);
}

VisBuffer *visual_buffer_new_wrap_data (void *data, visual_size_t size)
{
    return new LV::Buffer (data, size, false);
}

VisBuffer *visual_buffer_new_allocate (visual_size_t size)
{
    return new LV::Buffer (size);
}

VisBuffer *visual_buffer_clone (VisBuffer *source)
{
    visual_return_val_if_fail (source != NULL, NULL);

    LV::Buffer* self = new LV::Buffer;
    self->copy (*source);

    return self;
}

void visual_buffer_free (VisBuffer *self)
{
    delete self;
}

void visual_buffer_set_data_pair (VisBuffer *self, void *data, visual_size_t size)
{
    visual_return_if_fail (self != NULL);

    self->set (data, size);
}

void visual_buffer_set_data (VisBuffer *self, void *data)
{
    visual_return_if_fail (self != NULL);

    self->set_data (data);
}

void *visual_buffer_get_data (VisBuffer *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return self->get_data ();
}

void *visual_buffer_get_data_offset (VisBuffer *self, visual_size_t offset)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return self->get_data (offset);
}

void visual_buffer_set_size (VisBuffer *self, visual_size_t size)
{
    visual_return_if_fail (self != NULL);

    self->set_size (size);
}

visual_size_t visual_buffer_get_size (VisBuffer *self)
{
    visual_return_val_if_fail (self != NULL, 0);

    return self->get_size ();
}

int visual_buffer_is_allocated (VisBuffer *self)
{
    visual_return_val_if_fail (self != NULL, FALSE);

    return self->is_allocated ();
}

void visual_buffer_allocate (VisBuffer *self, visual_size_t size)
{
    visual_return_if_fail (self != NULL);

    return self->allocate (size);
}

void visual_buffer_destroy_content (VisBuffer *self)
{
    visual_return_if_fail (self != NULL);

    self->destroy_content ();
}

void visual_buffer_copy_data_to (VisBuffer *self, void *dest)
{
   visual_return_if_fail (self != NULL);

   self->copy_data_to (dest);
}

void visual_buffer_put (VisBuffer *self, VisBuffer *src, visual_size_t offset)
{
   visual_return_if_fail (self != NULL);

   self->put (*src, offset);
}

void visual_buffer_put_data (VisBuffer *self, const void *data, visual_size_t size, visual_size_t offset)
{
   visual_return_if_fail (self != NULL);

   self->put (data, size, offset);
}

void visual_buffer_fill (VisBuffer *self, uint8_t value)
{
   visual_return_if_fail (self != NULL);

   self->fill (value);
}

void visual_buffer_fill_with_pattern (VisBuffer *self, const void *data, visual_size_t size)
{
   visual_return_if_fail (self != NULL);

   self->fill_with_pattern (data, size);
}

void visual_buffer_ref (VisBuffer *self)
{
    visual_return_if_fail (self != NULL);

    self->ref ();
}

void visual_buffer_unref (VisBuffer *self)
{
    visual_return_if_fail (self != NULL);

    self->unref ();
}
