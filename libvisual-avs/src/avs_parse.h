#ifndef _LV_AVS_PARSE_H
#define _LV_AVS_PARSE_H

#include <libvisual/libvisual.h>

#include "avs_serialize.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AVS_TREE(obj)					(VISUAL_CHECK_CAST ((obj), 0, AVSTree))

#define AVS_ELEMENT(obj)				(VISUAL_CHECK_CAST ((obj), 0, AVSElement))
#define AVS_CONTAINER(obj)				(VISUAL_CHECK_CAST ((obj), 0, AVSContainer))

#define AVS_TREE_GET_CURRENT_POINTER(f)	((f)->cur)
	

typedef struct _AVSTree AVSTree;
typedef struct _AVSElement AVSElement;
typedef struct _AVSContainer AVSContainer;

typedef enum {
	AVS_VERSION_UNKNOWN,
	AVS_VERSION_1,
	AVS_VERSION_2
} AVSVersion;

typedef enum {
	AVS_ELEMENT_TYPE_MAIN,
	AVS_ELEMENT_TYPE_MISC_COMMENT,
	AVS_ELEMENT_TYPE_RENDER_BASSSPIN,
	AVS_ELEMENT_TYPE_RENDER_CLEARSCREEN,
	AVS_ELEMENT_TYPE_RENDER_RING,
	AVS_ELEMENT_TYPE_RENDER_SUPERSCOPE,
	AVS_ELEMENT_TYPE_TRANS_BLUR,
	AVS_ELEMENT_TYPE_TRANS_FASTBRIGHTNESS,
	AVS_ELEMENT_TYPE_TRANS_INVERT,
} AVSElementType;

typedef enum {
	AVS_DATA_CHANNEL_SOURCE_LEFT_WAVEFORM	= 0x00,
	AVS_DATA_CHANNEL_SOURCE_RIGHT_WAVEFORM	= 0x01,
	AVS_DATA_CHANNEL_SOURCE_CENTER_WAVEFORM	= 0x02,
	AVS_DATA_CHANNEL_SOURCE_LEFT_SPECTRUM	= 0x03,
	AVS_DATA_CHANNEL_SOURCE_RIGHT_SPECTRUM	= 0x04,
	AVS_DATA_CHANNEL_SOURCE_CENTER_SPECTRUM	= 0x05
} AVSDataChannelSourceType;

typedef enum {
	AVS_DATA_CHANNEL_ENABLED_TYPE_NONE	= 0x00,
	AVS_DATA_CHANNEL_ENABLED_TYPE_LEFT	= 0x01,
	AVS_DATA_CHANNEL_ENABLED_TYPE_RIGHT	= 0x02,
	AVS_DATA_CHANNEL_ENABLED_TYPE_BOTH	= 0x03
} AVSDataChannelEnabledType;

typedef enum {
	AVS_RENDER_SUPERSCOPE_TYPE_DOTS		= 0x00,
	AVS_RENDER_SUPERSCOPE_TYPE_LINES	= 0x01
} AVSRenderSuperScopeDrawType;

typedef enum {
	AVS_RENDER_CLEARSCREEN_TYPE_REPLACE	= 0x00,
	AVS_RENDER_CLEARSCREEN_TYPE_ADDITIVE	= 0x01,
	AVS_RENDER_CLEARSCREEN_TYPE_DEFAULT	= 0x02,
} AVSRenderClearScreenType;

typedef enum {
	AVS_RENDER_BASSSPIN_TYPE_LINES		= 0x00,
	AVS_RENDER_BASSSPIN_TYPE_TRAINGLE	= 0x01
} AVSRenderBassSpinType;

typedef enum {
	AVS_TRANS_FASTBRIGHTNESS_TYPE_DOUBLE	= 0x00,
	AVS_TRANS_FASTBRIGHTNESS_TYPE_HALF	= 0x01,
	AVS_TRANS_FASTBRIGHTNESS_TYPE_NOTHING	= 0x02
} AVSTransFastBrightnessType;

typedef enum {
	AVS_TRANS_BLUR_TYPE_NONE		= 0x00,
	AVS_TRANS_BLUR_TYPE_LIGHT		= 0x01,
	AVS_TRANS_BLUR_TYPE_MEDIUM		= 0x02,
	AVS_TRANS_BLUR_TYPE_HEAVY		= 0x03
} AVSTransBlurType;

/* The AVS data structure */
struct _AVSTree {
	VisObject object;

	char *origfile;

	int datasize;
	char *data;

	char *cur;

	AVSVersion version;

	AVSContainer *main;
};

struct _AVSElement {
	VisObject object;

	AVSElementType type;

	AVSSerializeContainer *serialize;
	VisParamContainer *pcont;
};

struct _AVSContainer {
	AVSElement element;
	
	VisList *members;
};


/* Prototypes */
AVSTree *avs_tree_new_from_preset (char *filename);
int avs_check_version (AVSTree *avstree);

int avs_parse_tree (AVSTree *avstree, AVSContainer *curcontainer);

int avs_element_connect_serialize_container (AVSElement *element, AVSSerializeContainer *scont);
int avs_element_deserialize (AVSElement *element, AVSTree *avstree);

AVSContainer *avs_parse_main (AVSTree *avstree);

AVSElement *avs_parse_misc_comment (AVSTree *avstree);

AVSElement *avs_parse_render_superscope (AVSTree *avstree);
AVSElement *avs_parse_render_clearscreen (AVSTree *avstree);
AVSElement *avs_parse_render_ring (AVSTree *avstree);
AVSElement *avs_parse_render_bassspin (AVSTree *avstree);

AVSElement *avs_parse_trans_fastbrightness (AVSTree *avstree);
AVSElement *avs_parse_trans_invert (AVSTree *avstree);
AVSElement *avs_parse_trans_blur (AVSTree *avstree);

int avs_parse_data (AVSTree *avstree, char *filename);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_AVS_PARSE_H */
