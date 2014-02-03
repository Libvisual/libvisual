#if P_SZ == 1
	#define PIXTYPE unsigned char
	#define REDSHIFT 4
	#define GRNSHIFT 2
	#define COLMASK 0x3
	#define _Line			Line8
	#define _BoxBlur		BoxBlur8
	#define _CrossBlur		CrossBlur8
	#define _EraseRect		EraseRect8
	#define __Clr(r,g,b)	(r >> 8)
#elif P_SZ == 2
	#define PIXTYPE unsigned short
	#define REDSHIFT 10
	#define GRNSHIFT 5
	#define COLMASK 0x1F
	#define _Line			Line16
	#define _BoxBlur		BoxBlur16
	#define _CrossBlur		CrossBlur16
	#define _EraseRect		EraseRect16
	#define __Clr(r,g,b)	(((r & 0xF800) >> 1) | ((g & 0xF800) >> 6) | (b >> 11))
#elif P_SZ == 4
	#define PIXTYPE uint32_t
	#define REDSHIFT 16
	#define GRNSHIFT 8
	#define COLMASK 0xFF
	#define _Line			Line32
	#define _BoxBlur		BoxBlur32
	#define _CrossBlur		CrossBlur32
	#define _EraseRect		EraseRect32
	#if EG_MAC  || defined(UNIX_X)
	#define	__Clr(r,g,b)	(((r & 0xFF00) << 8) | (g & 0xFF00) | (b >> 8))
	#elif EG_WIN
	#define	__Clr(r,g,b)	__winRGB( b, g, r )
	#endif

#endif




#define __doXerr		error_term += dy;				\
						if ( error_term >= dx ) {		\
							error_term -= dx;			\
							basePtr += rowOffset;		\
							ymov--;						\
						}


#define __doYerr		error_term += dx;				\
						if ( error_term >= dy ) {		\
							error_term -= dy;			\
							basePtr += xDirection;		\
							xmov--;						\
						}

#define __calcClr		color = __Clr( R, G, B );		\
						R += dR;						\
						G += dG;						\
						B += dB;


#define __circ( dia, a )	switch ( (dia) )		{									\
								case 2:		a = "\0\0"; break;							\
								case 3:		a = "\1\0\1"; break;						\
								case 4:		a = "\1\0\0\1"; break;						\
								case 5:		a = "\1\0\0\0\1"; break;					\
								case 6:		a = "\1\0\0\0\0\1"; break;					\
								case 7:		a = "\2\1\0\0\0\1\2"; break;				\
								case 8:		a = "\2\1\0\0\0\0\1\2"; break;				\
								case 9:		a = "\3\1\1\0\0\0\1\1\3"; break;			\
								case 10:	a = "\3\1\1\0\0\0\0\1\1\3"; break;			\
								case 11:	a = "\4\2\1\1\0\0\0\1\1\2\4"; break;		\
								case 12:	a = "\4\2\1\1\0\0\0\0\1\1\2\4"; break;		\
							}


#define CLR_INTERP 1
#include "LineXX.cpp"
#undef CLR_INTERP
#include "LineXX.cpp"

/*
void PixPort::_Line( int sx, int sy, int ex, int ey, long inColor ) {
	int xDirection, rowOffset, error_term;
	char* basePtr;
	int xmov, ymov, dx, dy, t;

	// Clipping: Set the pen loc to a point that's in and stop drawing once/if the pen moves out
	if ( sx < 0 || sx > mX || sy < 0 || sy > mY ) {
		t = ex; ex = sx; sx = t;
		t = ey; ey = sy; sy = t;
	}

	// Exit if the start pt is out of bounds (wimpy clipping, eh?)
	if ( sx < 0 || sx > mX || sy < 0 || sy > mY )
		return;

	// In Win32, everything's upside down
	#if EG_WIN
	sy = mY - sy;
	ey = mY - ey;
	#endif

	dx = ex - sx;
	dy = ey - sy;

	// moving left or right?
	xmov = dx;
	if ( dx < 0 ) {
		xmov = -dx;
		if ( sx - xmov < 0 )
			xmov = sx;
		xDirection = - P_SZ;
		dx = -dx; }
	else if ( dx > 0 ) {
		if ( sx + xmov > mX )
			xmov = mX - sx;
		xDirection = P_SZ;  }
	else
		xDirection = 0;

	// moving up or down?
	ymov = dy;
	if ( dy < 0 ) {
		ymov = -dy;
		if ( sy - ymov < 0 )
			ymov = sy;
		rowOffset = - mBytesPerRow;
		dy = -dy; }
	else if ( dy > 0 ) {
		if ( sy + ymov > mY )
			ymov = mY - sy;
		rowOffset = mBytesPerRow;  }
	else
		rowOffset = 0;

	basePtr = mBits + sy * mBytesPerRow + sx * P_SZ;
	error_term = 0;

	// Draw the line
	if ( dx >= dy ) {

		// Start counting off in x
		for ( ; xmov >= 0 && ymov >= 0; xmov-- ) {

			*((PIXTYPE*) basePtr) = inColor;
			basePtr += xDirection;

			// Check to see if we need to move the pixelOffset in the y direction.
			error_term += dy;
			if ( error_term >= dx ) {
				error_term -= dx;
				basePtr += rowOffset;
				ymov--;
			}
		} }
	else {
		// Start counting off in y
		for ( ; ymov >= 0 && xmov >= 0; ymov-- ) {

			*((PIXTYPE*) basePtr) = inColor;
			basePtr += rowOffset;

			// Check to see if we need to move the pixelOffset in the y direction.
			error_term += dx;
			if ( error_term >= dy ) {
				error_term -= dy;
				basePtr += xDirection;
				xmov--;
			}
		}
	}
}

*/



/*
void PixPort::_LineW( int sx, int sy, int ex, int ey, int inWidth, long inColor ) {
	int xDirection, rowOffset, error_term;
	char* basePtr, *rowPtr;
	int xmov, ymov, dx, dy, t, i, xDir, yDir;

	// Clipping: Set the pen loc to a point that's in and stop drawing once/if the pen moves out
	if ( sx < 0 || sx > mX || sy < 0 || sy > mY ) {
		t = ex; ex = sx; sx = t;
		t = ey; ey = sy; sy = t;
	}

	// Exit if the start pt is out of bounds (wimpy clipping, eh?)
	if ( sx < 0 || sx > mX || sy < 0 || sy > mY )
		return;

	// In Win32, everything's upside down
	#if EG_WIN
	sy = mY - sy;
	ey = mY - ey;
	#endif

	dx = ex - sx;
	dy = ey - sy;

	// moving left or right?
	xmov = dx;
	if ( dx < 0 ) {
		xmov = -dx;
		if ( sx - xmov < 0 )
			xmov = sx;
		xDirection = - P_SZ;
		xDir = -1;
		dx = -dx; }
	else if ( dx > 0 ) {
		if ( sx + xmov > mX )
			xmov = mX - sx;
		xDir = 1;
		xDirection = P_SZ;  }
	else {
		xDirection = 0;
		xDir = 0;
	}

	// moving up or down?
	ymov = dy;
	if ( dy < 0 ) {
		ymov = -dy;
		if ( sy - ymov < 0 )
			ymov = sy;
		yDir = 1;
		rowOffset = - mBytesPerRow;
		dy = -dy; }
	else if ( dy > 0 ) {
		if ( sy + ymov > mY )
			ymov = mY - sy;
		yDir = -1;
		rowOffset = mBytesPerRow;  }
	else {
		yDir = 0;
		rowOffset = 0;
	}


	basePtr = mBits + sy * mBytesPerRow + sx * P_SZ;
	error_term = 0;

	// Draw the line
	if ( dx >= dy ) {
		bool didRow = false;
		int limx;
		if ( xDirection > 0 )
			limx = ex;
		else
			limx = sx;
		if ( limx > mX )
			limx = mX;


		// Start counting off in x
		for ( ; xmov >= 0 && ymov >= 0; xmov-- ) {

			if ( ! didRow ) {
				for ( i = 0; i < inWidth && sx + i <= limx; i++ ) {
					((PIXTYPE*) basePtr)[i] = inColor;
				}
				didRow = true;
			}
			sx += xDir;

			basePtr += xDirection;

			// Check to see if we need to move the pixelOffset in the y direction.
			error_term += dy;
			if ( error_term >= dx ) {
				error_term -= dx;

				// Finish the rest of the line width, making sure we don't go off the right
				didRow = false;
				basePtr += rowOffset;
				ymov--;
			}
		} }
	else {

		// Start counting off in y
		for ( ; ymov >= 0 && xmov >= 0; ymov-- ) {

			// Do the whole line width, making sure we don't go off the right
			for ( i = 0; i < inWidth && sx + i < mX; i++ )
				((PIXTYPE*) basePtr)[i] = inColor;
			basePtr += rowOffset;

			// Check to see if we need to move the pixelOffset in the y direction.
			error_term += dx;
			if ( error_term >= dy ) {
				error_term -= dy;
				basePtr += xDirection;
				xmov--;
				sx += xDir;
			}
		}
	}
}*/



void PixPort::_EraseRect( const Rect* inRect ) {
	int32_t width, height;
	int x, y;
	char*	base;
	Rect	r;

	// Don't let us draw in random parts of memory -- clip inRect
	if ( inRect ) {
		r = *inRect;
		__clipPt( r.left, r.top )
		__clipPt( r.right, r.bottom ) }
	else {
		r = mClipRect;
	}

	width 	= r.right - r.left;
	height	= r.bottom - r.top;


	// In Win32, everything's upside down
	#if EG_WIN
	r.top = mY - r.bottom;
	#endif

	base = mBits + mBytesPerPix * r.left + r.top * mBytesPerRow;
	for ( y = 0; y <= height; y++ ) {
		for ( x = 0; x <= width; x++ ) {
			*((PIXTYPE*) base) = mBackColor;
			base += P_SZ;
		}
		base += mBytesPerRow - P_SZ * (width + 1);
	}
}



void PixPort::_CrossBlur( char* inSrce, int inWidth, int inHeight, int inBytesPerRow, unsigned char* inRowBuf ) {
	int32_t leftR, leftG, leftB, cenR, cenG, cenB, rightR, rightG, rightB;
	int32_t topR, topG, topB, val, botR, botG, botB, x;
	unsigned char *rowPos;

	// Init inRowBuf[]
	rowPos = inRowBuf;
	for ( x = 0; x < inWidth; x++ ) {
		val = ((PIXTYPE*) inSrce)[ x ];
		rowPos[ 0 ]  = val >> REDSHIFT;
		rowPos[ 1 ] = (val >> GRNSHIFT) & COLMASK;
		rowPos[ 2 ] = val & COLMASK;
		rowPos += 3;
	}

	// Go thru row by row in the source img
	for ( ; inHeight > 0; inHeight-- ) {

		// Prime the x-loop and get left and cen pixels
		val = *((PIXTYPE*) inSrce );
		leftR = cenR = val >> REDSHIFT;
		leftG = cenG = (val >> GRNSHIFT) & COLMASK;
		leftB = cenB = val & COLMASK;

		rowPos = inRowBuf;

		for ( x = 0; x < inWidth; x++ ) {

			// Get top pixel
			topR = rowPos[ 0 ];
			topG = rowPos[ 1 ];
			topB = rowPos[ 2 ];

			// Get right-most pixel
			val = ((PIXTYPE*) inSrce)[ x + 1 ];
			rightR = val >> REDSHIFT;
			rightG = (val >> GRNSHIFT) & COLMASK;
			rightB = val & COLMASK;

			// Get bottom pixel
			val = ((PIXTYPE*) (inSrce + inBytesPerRow))[ x ];
			botR = val >> REDSHIFT;
			botG = (val >> GRNSHIFT) & COLMASK;
			botB = val & COLMASK;

			*rowPos = cenR;		rowPos++;
			*rowPos = cenG;		rowPos++;
			*rowPos = cenB;		rowPos++;

			botR = ( ( cenR << 2 ) + 3 * ( leftR + rightR + topR + botR ) ) >> 4;
			botG = ( ( cenG << 2 ) + 3 * ( leftG + rightG + topG + botG ) ) >> 4;
			botB = ( ( cenB << 2 ) + 3 * ( leftB + rightB + topB + botB ) ) >> 4;
			((PIXTYPE*) inSrce )[ x ] = ( botR << REDSHIFT ) | ( botG << GRNSHIFT ) | botB;

			// Re-use already-fetched memory
			leftR = cenR;	cenR = rightR;
			leftG = cenG;	cenG = rightG;
			leftB = cenB;	cenB = rightB;
		}

		inSrce += inBytesPerRow;
	}
}







#define UL uint32_t
#define DENOM_SHIFT 14

void PixPort::_BoxBlur( char* inSrce, char* inDest, int inBoxWidth, int inWidth, int inHeight, int inSrceRowWidth, int inDestRowWidth, uint32_t* b, uint32_t inBackColor ) {
	uint32_t* bEnd;
	char *dest;
	uint32_t b1R, b1G, b1B, b2R, b2G, b2B, b3R, b3G, b3B, val, box9W, i, numerator;
	int x, half, useWidth;

	i = inBoxWidth * inBoxWidth * inBoxWidth;
	numerator = ( 1 << DENOM_SHIFT ) / ( i );
	box9W = 9 * inBoxWidth;		// 3 colors, 3 boxes
	bEnd = b + box9W;

	b1R = 0; b1G = 0; b1B = 0;
	b2R = 0; b2G = 0; b2B = 0;
	b3R = i >> 1; b3G = b3R; b3B = b3R;		// round up when > .5
	for ( i = 0; i < box9W; i++ ) {
		b[ i ] = 0;
	}

	half = 3 * inBoxWidth / 2 - 1;
	inSrce += P_SZ * half;
	useWidth = inWidth - half - inBoxWidth % 2;

	// Go thru row by row in the source img
	for ( ; inHeight > 0; inHeight-- ) {

		// Go thru the row
		dest = inDest;

		for ( x = - half - 5; x < inWidth; x++ ) {

			// Maintain the circular buffer
			if ( b == bEnd )
				b -= box9W;

			// p = fetch next pix from b1
			if ( x >= 0 && x < useWidth ) {
				val = *( (PIXTYPE*) inSrce );
				inSrce += P_SZ; }
			else
				val = inBackColor;

			// p' += new pix - end pix and store new pix
			i = val >> REDSHIFT;  				b1R += i - b[0];		b[0] = i;
			i = (val >> GRNSHIFT) & COLMASK; 	b1G += i - b[1];		b[1] = i;
			i = val & COLMASK;  				b1B += i - b[2];		b[2] = i;

			// Store the b2's new pix and calc its new pixel
			b2R += b1R - b[3];		b[3] = b1R;
			b2G += b1G - b[4];		b[4] = b1G;
			b2B += b1B - b[5];		b[5] = b1B;

			// Store the b3's new pix and calc its new pixel
			b3R += b2R - b[6];		b[6] = b2R;
			b3G += b2G - b[7];		b[7] = b2G;
			b3B += b2B - b[8];		b[8] = b2B;

			// Transpose the final pixel calculations
			if ( x >= 0 ) {
				*((PIXTYPE*)dest) = ( (( numerator * b3R ) >> DENOM_SHIFT) << REDSHIFT ) | ( (( numerator * b3G ) >> DENOM_SHIFT) << GRNSHIFT ) | (( numerator * b3B ) >> DENOM_SHIFT);
				dest += inDestRowWidth;
			}

			// Maintain our circular buffer
			b += 9;

		}

		// Do next row
		inSrce += inSrceRowWidth - P_SZ * useWidth;
		inDest += P_SZ;
	}
}



/*
this modification to BoxBlur only blurs the x row
void PixPort::_BoxBlur( char* inSrce, char*, int inBoxWidth, int inWidth, int inHeight, int inSrceRowWidth, int, char* b, unsigned long inBackColor ) {

	char *bEnd;
	unsigned long b1R, b1G, b1B, b2R, b2G, b2B, b3R, b3G, b3B, val, box9W, i, denom;
	int x, half, useWidth;


	denom = inBoxWidth * inBoxWidth * inBoxWidth;
	box9W = 36 * inBoxWidth;		// 3 colors, 3 boxes, 4 bytes per long
	bEnd = b + box9W;

	b1R = 0; b1G = 0; b1B = 0;
	b2R = 0; b2G = 0; b2B = 0;
	b3R = 0; b3G = 0; b3B = 0;
	for ( i = 0; i < 9 * inBoxWidth; ) {
		*((UL*) b + i) = 0;	i++;
	}

	half = 3 * inBoxWidth / 2 - 1;
	inSrce += P_SZ * half;
	useWidth = inWidth - half - inBoxWidth % 2;

	// Go thru row by row in the source img
	for ( ; inHeight > 0; inHeight-- ) {

		for ( x = - half - 5; x < inWidth; x++ ) {

			// Maintain our circular buffer
			if ( b == bEnd )
				b -= box9W;

			// p = fetch next pix from b1
			if ( x >= 0 && x < useWidth ) {
				val = *( (PIXTYPE*) inSrce );}
			else
				val = inBackColor;

			// p' += new pix - end pix and store new pix
			i = val >> REDSHIFT;  				b1R += i - *((UL*)b);		*((UL*)b) = i;	b += 4;
			i = (val >> GRNSHIFT) & COLMASK; 	b1G += i - *((UL*)b);		*((UL*)b) = i;	b += 4;
			i = val & COLMASK;  				b1B += i - *((UL*)b);		*((UL*)b) = i;	b += 4;

			// Store the b2's new pix and calc its new pixel
			b2R += b1R - *((UL*)b);		*((UL*)b) = b1R;	b += 4;
			b2G += b1G - *((UL*)b);		*((UL*)b) = b1G;	b += 4;
			b2B += b1B - *((UL*)b);		*((UL*)b) = b1B;	b += 4;

			// Store the b3's new pix and calc its new pixel
			b3R += b2R - *((UL*)b);		*((UL*)b) = b2R;	b += 4;
			b3G += b2G - *((UL*)b);		*((UL*)b) = b2G;	b += 4;
			b3B += b2B - *((UL*)b);		*((UL*)b) = b2B;	b += 4;

			// Transpose the final pixel calculations
			if ( x >= 0 ) {
				*((PIXTYPE*)inSrce) = ( (b3R/denom) << REDSHIFT ) | ( ( b3G/denom ) << GRNSHIFT ) | ( b3B/denom );
				inSrce += P_SZ;
			}
		}

		// Do next row
		inSrce += inSrceRowWidth - P_SZ * inWidth;
	}
}
*/



#undef PIXTYPE
#undef REDSHIFT
#undef GRNSHIFT
#undef COLMASK
#undef _Line
#undef _LineW
#undef _BoxBlur
#undef _CrossBlur
#undef _EraseRect
#undef __Clr
