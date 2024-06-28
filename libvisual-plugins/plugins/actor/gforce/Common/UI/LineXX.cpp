#include <math.h>
#include <PixPort.h>


#if CLR_INTERP
	#if P_SZ != 1
	void PixPort::_Line( int sx, int sy, int ex, int ey, const RGBColor& inS, int32_t dR, int32_t dG, int32_t dB ) {
	#else
	void PixPort::_Line( int sx, int sy, int ex, int ey, int32_t R, int32_t dR ) {
	#endif
#else
	void PixPort::_Line( int sx, int sy, int ex, int ey, int32_t color ) {
#endif
	int32_t xDirection, rowOffset, error_term;
	char* basePtr, *center;
	int32_t xmov, ymov, dx, dy, t, j, lw;
	int32_t penExtents;

	#if CLR_INTERP
	int32_t color;
	#if P_SZ != 1
	int32_t R = inS.red;
	int32_t G = inS.green;
	int32_t B = inS.blue;
	#endif
	#endif

	// Half the coordinte if it's large (we copy the sign bit in the 2^31 digit)
	// To do: use float clipping
	sx = ( ( (int32_t) (sx & 0x80000000) ) >> 1 ) | ( sx & 0x3FFFFFFF );
	ex = ( ( (int32_t) (ex & 0x80000000) ) >> 1 ) | ( ex & 0x3FFFFFFF );
	sy = ( ( (int32_t) (sy & 0x80000000) ) >> 1 ) | ( sy & 0x3FFFFFFF );
	ey = ( ( (int32_t) (ey & 0x80000000) ) >> 1 ) | ( ey & 0x3FFFFFFF );

	// Modify the line width so that the actual width matches mLineWidth
	lw = mLineWidth;
	if ( mLineWidth > 3 ) {
		dx = ex - sx;	dx = dx * dx;
		dy = ey - sy;	dy = dy * dy;
		if ( dx > 0 && dx >= dy )
			lw = 128 + 55 * dy / dx; 			// 1/cos( atan( x ) ) is about 1+.43*x^2 from 0 to 1 (55 == .43 * 128)
		else if ( dy > 0 && dy > dx )
			lw = 128 + 55 * dx / dy; 			// 1/cos( atan( x ) ) is about 1+.43*x^2 from 0 to 1 (55 == .43 * 128)

		if ( dx > 0 || dy > 0 )
			lw = ( mLineWidth * lw + 64 ) >> 7;		// Add in order to round up
	}
	penExtents = lw >> 1;



	// Clipping: Set the pen loc to a point that's in and stop drawing once/if the pen moves out
	if ( sx < mClipRect.left + penExtents || sx >= mClipRect.right - penExtents || sy < mClipRect.top + penExtents || sy >= mClipRect.bottom - penExtents ) {

		// Exit if both points are out of bounds (wimpy clipping, eh?)
		if ( ex < mClipRect.left + penExtents || ex >= mClipRect.right - penExtents || ey < mClipRect.top + penExtents || ey >= mClipRect.bottom - penExtents )
			return;

		t = ex; ex = sx; sx = t;
		t = ey; ey = sy; sy = t;

		#if CLR_INTERP
		R += dR; dR = -dR;
		#if P_SZ != 1
		G += dG; dG = -dG;
		B += dB; dB = -dB;
		#endif
		#endif

	}

	dx = ex - sx;
	dy = ey - sy;


	#if CLR_INTERP && P_SZ != 1
	int32_t len = sqrt( dx * dx + dy * dy ) + 1;
	dR /= len;
	dG /= len;
	dB /= len;
	color = __Clr( R, G, B );
	#elif CLR_INTERP && P_SZ == 1
	int32_t len = sqrt( dx * dx + dy * dy ) + 1;
	dR /= len;
	color = __Clr( R, G, B );
	#endif


	// moving left or right?
	dx = ex - sx;
	xmov = dx;
	if ( dx < 0 ) {
		xmov = -dx;
		if ( sx - xmov < mClipRect.left + penExtents )
			xmov = sx - ( mClipRect.left + penExtents );
		xDirection = - P_SZ;
		dx = -dx; }
	else if ( dx > 0 ) {
		if ( sx + xmov >= mClipRect.right - penExtents )
			xmov = mClipRect.right - penExtents - 1 - sx;
		xDirection = P_SZ;  }
	else
		xDirection = 0;


	// moving up or down?
	ymov = dy;
	if ( dy < 0 ) {
		ymov = -dy;
		if ( sy - ymov < mClipRect.top + penExtents )
			ymov = sy - ( mClipRect.top + penExtents );
		rowOffset = - mBytesPerRow;
		dy = -dy; }
	else {
		if ( sy + ymov >= mClipRect.bottom - penExtents )
			ymov = mClipRect.bottom - penExtents - sy - 1;
		rowOffset = mBytesPerRow;
	}

	// In Win32, everything's upside down
	#if EG_WIN
	sy = mY - sy - 1;
	ey = mY - ey - 1;
	rowOffset = - rowOffset;
	#endif


	basePtr = mBits + sy * mBytesPerRow + sx * P_SZ;
	error_term = 0;

	int32_t halfW;

	if ( lw > 1 ) {

		// Make a circle for the pen
		int32_t c_x, tw = mLineWidth;
		halfW = ( tw ) >> 1;

		if ( tw < 12 ) {
			const char* c_shape = nullptr;
			__circ( tw, c_shape )
			for ( j = 0; j < tw; j++ ) {
				c_x = c_shape[ j ];
				center = basePtr + (j-halfW) * mBytesPerRow;
				for ( int k = c_x; k < tw - c_x; k++ ){
					((PIXTYPE*) center)[k-halfW] = color;
				}
			} }
		else {

			for ( j = 0; j < tw; j++ ) {
				int32_t tmp = j - halfW;
				c_x = halfW - ( ( int32_t ) sqrt( halfW * halfW - tmp * tmp ) );
				center = basePtr + (j-halfW) * mBytesPerRow;
				for ( int k = c_x; k < tw - c_x; k++ ){
					((PIXTYPE*) center)[k-halfW] = color;
				}
			}
		}


		halfW = lw >> 1;

		// Draw the line
		if ( dx > dy ) {

			// Start counting off in x
			for ( ; xmov >= 0 && ymov >= 0; xmov-- ) {

				#if CLR_INTERP
				#if P_SZ == 1
				color = R >> 8;
				R += dR;
				#else
				__calcClr
				#endif
				#endif

				// Draw the vertical leading edge of the pen
				center = basePtr - halfW * mBytesPerRow;
				for ( j = 0; j < lw; j++ ) {
					*((PIXTYPE*) center) = color;
					center += mBytesPerRow;
				}
				/*
				// Draw the horizontal leading edge of the pen
				center = basePtr + halfW * ( rowOffset - P_SZ );
				for ( j = 0; j < lw; j++ ) {
					*((PIXTYPE*) center) = color;
					center += P_SZ;
				}*/
				basePtr += xDirection;

				// Check to see if we need to move the pixelOffset in the y direction.
				__doXerr
			} }
		else {
			// Start counting off in y
			for ( ; ymov >= 0 && xmov >= 0; ymov-- ) {

				#if CLR_INTERP
				#if P_SZ == 1
				color = R >> 8;
				R += dR;
				#else
				__calcClr
				#endif
				#endif
				/*
				// Draw the vertical leading edge of the pen
				center = basePtr + xDirection * (halfW) - P_SZ - halfW * mBytesPerRow;
				for ( j = 0; j < lw; j++ ) {
					*((PIXTYPE*) center) = color;
					center += mBytesPerRow;
				}*/
				// Draw the horizontal leading edge of the pen
				center = basePtr - ( halfW ) * P_SZ;
				for ( j = 0; j < lw; j++ ) {
					*((PIXTYPE*) center) = color;
					center += P_SZ;
				}
				basePtr += rowOffset;

				// Check to see if we need to move the pixelOffset in the y direction.
				__doYerr
			}
		}

		// If line len is 0, we don't need to draw ending pen circle
		/*
		if ( lw > 3 ) {
			if ( dx != 0 || dy != 0 ) {
			}
		} */

		}
	else {

		// Draw the (single pixel) line
		if ( dx >= dy ) {

			// Start counting off in x
			for ( ; xmov >= 0 && ymov >= 0; xmov-- ) {

				#if CLR_INTERP
				#if P_SZ == 1
				color = R >> 8;
				R += dR;
				#else
				__calcClr
				#endif
				#endif

				*((PIXTYPE*) basePtr) = color;

				basePtr += xDirection;

				// Check to see if we need to move the pixelOffset in the y direction.
				__doXerr
			} }
		else {
			// Start counting off in y
			for ( ; ymov >= 0 && xmov >= 0; ymov-- ) {

				#if CLR_INTERP
				#if P_SZ == 1
				color = R >> 8;
				R += dR;
				#else
				__calcClr
				#endif
				#endif

				*((PIXTYPE*) basePtr) = color;
				basePtr += rowOffset;

				// Check to see if we need to move the pixelOffset in the y direction.
				__doYerr
			}
		}
	}

}
