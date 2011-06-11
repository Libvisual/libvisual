#ifndef EG_COMMON_H
#define EG_COMMON_H

#include <libmfl.h>

	#ifndef true
	#define true -1
	#define false 0
	#endif

	struct Rect {
		short left, top, right, bottom;
	};
	#define Rect_Defined

	typedef long KeyMap[4];
	
	struct Point {
		short v, h;
	};
		
	struct RGBColor {
		unsigned short red, green, blue;
	};

/* FIXME */
	typedef void *		PixMapHandle;
	typedef void *			GrafPtr;
	typedef void *			CGrafPtr;
	typedef void *		WindowPtr;
	typedef mfl_context		GWorldPtr;
	typedef void *			BitMap;

	struct ColorSpec {
		short		value;
		RGBColor	rgb;
	};


struct LongRect {
	long left, top, right, bottom;
};

#endif
