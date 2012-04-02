#include "config.h"
#include "lv_songinfo.h"
#include "lv_common.h"

extern "C" {

  VisSongInfo *visual_songinfo_new (VisSongInfoType type)
  {
      return new LV::SongInfo (LV::SongInfoType (type));
  }

  void visual_songinfo_free (VisSongInfo *self)
  {
      delete self;
  }

  void visual_songinfo_set_type (VisSongInfo *self, VisSongInfoType type)
  {
      visual_return_if_fail (self != NULL);

      self->set_type (LV::SongInfoType (type));
  }

  VisSongInfoType visual_songinfo_get_type (VisSongInfo *self)
  {
      visual_return_val_if_fail (self != NULL, VISUAL_SONGINFO_TYPE_NULL);

      return VisSongInfoType (self->get_type ());
  }

  void visual_songinfo_set_length (VisSongInfo *self, int length)
  {
      visual_return_if_fail (self != NULL);

      self->set_length (length);
  }

  int visual_songinfo_get_length (VisSongInfo *self)
  {
	  visual_return_val_if_fail (self != NULL, 0);

      return self->get_length ();
  }

  void visual_songinfo_set_elapsed (VisSongInfo *self, int elapsed)
  {
      visual_return_if_fail (self != NULL);

      self->set_elapsed (elapsed);
  }

  int visual_songinfo_get_elapsed (VisSongInfo *self)
  {
      return self->get_elapsed ();
  }

  void visual_songinfo_set_simple_name (VisSongInfo *self, const char *name)
  {
      visual_return_if_fail (self != NULL);

      self->set_simple_name (name);
  }

  const char *visual_songinfo_get_simple_name (VisSongInfo *self)
  {
      visual_return_val_if_fail (self != NULL, NULL);

      return self->get_simple_name ().c_str ();
  }

  void visual_songinfo_set_artist (VisSongInfo *self, const char *artist)
  {
      visual_return_if_fail (self != NULL);

      self->set_artist (artist);
  }

  const char *visual_songinfo_get_artist (VisSongInfo *self)
  {
      visual_return_val_if_fail (self != NULL, NULL);

      return self->get_artist ().c_str ();
  }

  void visual_songinfo_set_album (VisSongInfo *self, const char *album)
  {
      visual_return_if_fail (self != NULL);

      self->set_album (album);
  }

  const char *visual_songinfo_get_album (VisSongInfo *self)
  {
      visual_return_val_if_fail (self != NULL, NULL);

      return self->get_album ().c_str ();
  }

  void visual_songinfo_set_song (VisSongInfo *self, const char *song)
  {
      visual_return_if_fail (self != NULL);

      self->set_song (song);
  }

  const char *visual_songinfo_get_song (VisSongInfo *self)
  {
	  visual_return_val_if_fail (self != NULL, NULL);

      return self->get_song ().c_str ();
  }

  void visual_songinfo_set_cover (VisSongInfo *self, VisVideo *cover)
  {
      visual_return_if_fail (self  != NULL);
      visual_return_if_fail (cover != NULL);

      self->set_cover (cover);
  }

  void visual_songinfo_mark (VisSongInfo *self)
  {
      visual_return_if_fail (self != NULL);

      self->mark ();
  }

  long visual_songinfo_get_age (VisSongInfo *self)
  {
      visual_return_val_if_fail (self != NULL, 0);

      return self->get_age ();
  }

  void visual_songinfo_copy (VisSongInfo *lhs, const VisSongInfo *rhs)
  {
      visual_return_if_fail (lhs != NULL);
      visual_return_if_fail (rhs != NULL);

      *lhs = *rhs;
  }

  int visual_songinfo_compare (const VisSongInfo *lhs, const VisSongInfo *rhs)
  {
      visual_return_val_if_fail (lhs != NULL, FALSE);
      visual_return_val_if_fail (rhs != NULL, FALSE);

      return *lhs == *rhs;
  }

} // C extern
