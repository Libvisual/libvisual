#ifndef __RectUtils__
#define __RectUtils__

#include <EgCommon.h>

extern short 		PtInRect( const Point& inPt, const Rect* inRect );
extern void			SetRect( Rect* inR, long left, long top, long right, long bot );
extern void			InsetRect( Rect* inR, int inDelX, int inDelY );
extern void			UnionRect( const Rect* inR1, const Rect* inR2, Rect* outRect );
extern void			OffsetRect( Rect* inRect, int inDelX, int inDelY );
extern void			SectRect( const Rect* inR1, const Rect* inR2, Rect* outRect );

#endif
