#ifndef _OINK_SCENE_H
#define _OINK_SCENE_H

/* FIXME, uint8_t *buf should be removed everywhere */
void _oink_scene_background_special (OinksiePrivate *priv, uint8_t *buf);
void _oink_scene_background_select (OinksiePrivate *priv, uint8_t *buf);
void _oink_scene_blur_select (OinksiePrivate *priv, uint8_t *buf);
void _oink_scene_scope_select (OinksiePrivate *priv, uint8_t *buf, int color, int height);
void _oink_scene_randomize (OinksiePrivate *priv);
void _oink_scene_render (OinksiePrivate *priv);

#endif /* _OINK_SCENE_H */
