#include "PixPort.h"

#include <math.h>
#include "RectUtils.h"

#include "ScreenDevice.h"

#ifdef UNIX_X
#include "libmfl.h"
#endif

#if EG_MAC
#include <QuickDraw.h>
#include <QDOffscreen.h>
#include <Displays.h>
#include <string.h>
#include <Palettes.h>



#define __setupPort		GDHandle		saveDev;								\
 						GWorldPtr		savePort;								\
						::GetGWorld( &savePort, &saveDev );						\
						::SetGWorld( mWorld, 0 );


#define __restorePort 	::SetGWorld( savePort, saveDev );



#endif

#if EG_WIN == 1 || defined(UNIX_X)


#define __setupPort
#define __restorePort
#endif


int32_t		PixPort::sTempSize                      = 0;
char*		PixPort::sTemp				= 0;






PixPort::PixPort() {
	mBM = 0;
	mWorld = 0;
	mX = 0;
	mY = 0;
	mLineWidth = 1;
	mBackColor	= 0;
	mBytesPerPix = 0;
	mCurFontID = 0;
	mDeviceLineHeight = 0;

	#if EG_WIN
	mWorld		= ::CreateCompatibleDC( 0 );
	mBM			= 0;
	#endif

	#ifdef UNIX_X
	mBits = 0;  // So we know if we need to delete it
	#endif
}



PixPort::~PixPort() {
	PixTextStyle* font;
	int i;

	Un_Init();

	#if EG_WIN
	::SelectObject( mWorld, ::GetStockObject( SYSTEM_FONT ) );

	// Dealloc HFONTs we made in windows
	for ( i = 0; i < mFonts.Count(); i++ ) {
		font = (PixTextStyle*) mFonts[ i ];
		::DeleteObject( (HFONT) font -> mOSFontID );
	}

	if ( mWorld )
		::DeleteDC( mWorld );

	// Dealloc the offscreen bitmap we made
	if ( mBM )
		::DeleteObject( mBM );
	#endif

	#if UNIX_X
	for ( i = 0; i < mFonts.Count(); i++ ) {
		font = (PixTextStyle*) mFonts[ i ];
		mfl_DestroyFont((mfl_font)font->mOSFontID);
	}

	#endif

	// Delete any info structures we may have created
	for ( i = 0; i < mFonts.Count(); i++ ) {
		font = (PixTextStyle*) mFonts[ i ];
		delete font;
	}

	if ( sTemp ) {
		delete []sTemp;
		sTemp = 0;
		sTempSize = 0;
	}
}




void PixPort::Un_Init() {


	#if EG_MAC
	if ( mWorld ) {
		::UnlockPixels( mBM );
		::DisposeGWorld( mWorld );
		mWorld = 0;
	}
	#endif

	#ifdef UNIX_X
	// Destroy font context
	if (mWorld) {
	  mfl_DestroyContext(mWorld);
	  mWorld = 0;
	}

	// Free buffer
	if (mBits) {
	  delete[] mBits;
	  mBits = 0;
	}
	#endif

	// Invalidate the selected text style
	mCurFontID = -1;
}



void PixPort::SetClipRect( const Rect* inRect ) {

	mClipRect.top = 0;
	mClipRect.left = 0;
	mClipRect.right = mX;
	mClipRect.bottom = mY;

	if ( inRect )
		SectRect( inRect, &mClipRect, &mClipRect );

	if ( mWorld ) {
		__setupPort

		#if EG_MAC
		::ClipRect( &mClipRect );
		#endif

		#if EG_WIN
		HRGN rgn = ::CreateRectRgn( mClipRect.left, mClipRect.top, mClipRect.right - 1, mClipRect.bottom - 1 );
		::SelectObject( mWorld, rgn );
		::DeleteObject( rgn );
		#endif

		__restorePort
	}
}


void PixPort::SetClipRect( int32_t inSX, int32_t inSY, int32_t inEX, int32_t inEY ) {

	Rect r;

	SetRect( &r, inSX, inSY, inEX, inEY );
	SetClipRect( &r );
}














/*

void PixPort::Init( GrafPtr inPort ) {

	Un_Init();

	#if EG_MAC
	mBM = ( (CGrafPtr) inPort ) -> portPixMap;
	mBytesPerRow	= (**mBM).rowBytes & 0xFFF;
	mBytesPerPix	= (**mBM).pixelSize / 8;
	mX			= (**mBM).bounds.right - (**mBM).bounds.left;
	mY			= (**mBM).bounds.bottom - (**mBM).bounds.top;
	#endif

}
*/


void PixPort::Init( int inWidth, int inHeight, int inDepth ) {

	if ( inWidth < 0 ) inWidth = 0;
	if ( inHeight < 0 ) inHeight = 0;

	// Catch any invalid depth levels.
	if ( inDepth != 32 && inDepth != 16 && inDepth != 8 )
		inDepth = ScreenDevice::sOSDepth;

	if ( inDepth < ScreenDevice::sMinDepth )
		inDepth = ScreenDevice::sMinDepth;


	// If we don't need to do anything, then don't do anything!
	if ( mWorld && mBytesPerPix * 8 == inDepth && inWidth == mX && inHeight == mY )
		return;

	// FIXME BUG again.  Maybe we should use DWORD alignment
#if 0
	mX			= 4 * (( inWidth + 3 ) / 4 );
#endif
	mX			= inWidth;
	mY			= inHeight;

	Un_Init();

	#if EG_MAC

	// Save current draw envir
	__setupPort

	Rect r;
	::SetRect( &r, 0, 0, mX, mY+1 );
	::NewGWorld( &mWorld, inDepth, &r, 0, 0, useTempMem );
	mBM = ::GetGWorldPixMap( mWorld );
	mBytesPerRow	= (**mBM).rowBytes & 0xFFF;
	mBytesPerPix	= (**mBM).pixelSize / 8;
	::LockPixels( mBM );
	mBits = ::GetPixBaseAddr( mBM );

	__restorePort

	#elif EG_WIN

	// Initialize a bmap info struct
	mInfo.bmiHeader.biSize			= sizeof( BITMAPINFOHEADER );
	mInfo.bmiHeader.biWidth			= mX + 4;
	mInfo.bmiHeader.biHeight		= mY + 2;
	mInfo.bmiHeader.biPlanes		= 1;
	mInfo.bmiHeader.biBitCount		= inDepth;
	mInfo.bmiHeader.biCompression	= BI_RGB;
	mInfo.bmiHeader.biSizeImage		= 0;
	mInfo.bmiHeader.biXPelsPerMeter	= 0;
	mInfo.bmiHeader.biYPelsPerMeter = 0;
	mInfo.bmiHeader.biClrUsed		= 0;
	mInfo.bmiHeader.biClrImportant	= 0;

	// Tell windows to make a bitmap and give us acess to its pixel data
	mBM = ::CreateDIBSection( mWorld, &mInfo, DIB_RGB_COLORS, &mBits, 0, 0 );
	HGDIOBJ oldBM = ::SelectObject( mWorld, mBM );
	if ( oldBM )
		::DeleteObject( oldBM );

	BITMAP b;
	::GetObject( mBM, sizeof( BITMAP ), &b );
	mBytesPerRow	= b.bmWidthBytes;
	mBytesPerPix	= b.bmBitsPixel / 8;

	::SetTextAlign( mWorld, TA_BASELINE | TA_LEFT );
	::SetBkMode( mWorld, TRANSPARENT );

	#elif defined(UNIX_X)
	// Setup bitmap
	mBytesPerRow = mX;
	mBytesPerPix = 1;
	mBits = new char[mBytesPerRow * (mY + 2)];

	// Setup font data
	mWorld = mfl_CreateContext(mBits, mBytesPerPix * 8,
				   mBytesPerRow, mX, mY);
	#endif

	SetClipRect();
	EraseRect();
}






#define __clipPt( x, y )	\
	if ( x < mClipRect.left )			\
		x = mClipRect.left;				\
	else if ( x > mClipRect.right )		\
		x = mClipRect.right;			\
	if ( y < mClipRect.top )			\
		y = mClipRect.top;				\
	else if ( y > mClipRect.bottom )	\
		y = mClipRect.bottom;



#define __clipRect( inRect )			\
	Rect r = inRect;					\
	__clipPt( r.left, r.top )			\
	__clipPt( r.right, r.bottom )		\
	int32_t width   = r.right - r.left;             \
	int32_t height = r.bottom - r.top;










int32_t PixPort::GetPortColor( int32_t inR, int32_t inG, int32_t inB ) {
	int bitDepth  =mBytesPerPix << 3;

	int32_t c;

	if ( inR > 0xFFFF )	inR = 0xFFFF;
	if ( inG > 0xFFFF )	inG = 0xFFFF;
	if ( inB > 0xFFFF )	inB = 0xFFFF;
	if ( inR < 0 )		inR = 0;
	if ( inG < 0 )		inG = 0;
	if ( inB < 0 )		inB = 0;

	if ( bitDepth == 32 )
		c = __Clr32( inR, inG, inB );
	else if ( bitDepth == 16 )
		c = __Clr16( inR, inG, inB );
	else
		c = __Clr8( inR, inG, inB );

	return c;
}





int32_t PixPort::SetBackColor( const RGBColor& RGB ) {

	mBackColor = GetPortColor( RGB );

	return mBackColor;
}


int32_t PixPort::SetBackColor( int32_t inR, int32_t inG, int32_t inB ) {
	mBackColor = GetPortColor( inR, inG, inB );

	return mBackColor;
}


void PixPort::SetPalette( PixPalEntry inPal[ 256 ] ) {

	if ( mBytesPerPix != 1 )
		return;

	#if EG_WIN
	::SetDIBColorTable( mWorld, 0, 256, inPal );
	#endif


	#if EG_MAC
	CTabHandle myTable = (**mBM).pmTable;

	::BlockMove( inPal, (**myTable).ctTable, 256 * sizeof( ColorSpec ) );
	::CTabChanged( myTable );
	#endif

	#ifdef UNIX_X
	#endif
}


void PixPort::PreventActivate( GrafPtr inSysWindow ) {

	#if EG_MAC

		if ( mBytesPerPix != 1 )
			return;

	/*
		#if USE_DISP_MGR
		if ( ! inSysWindow )
			inSysWindow = (GrafPort*) mContextRef;
		#endif */

		// Below prevents MacOS from reindexing the colors in our GWorld during CopyBits, thinking the colors in our GWorld
		// index to the standard system colors.  It's a hack, but then again, so is MacOS 8.x
		CTabHandle myTable = (**mBM).pmTable;
		if ( inSysWindow ) {
			(**myTable).ctSeed = (**(*((CGrafPort* ) inSysWindow) -> portPixMap) -> pmTable).ctSeed;
		}

	#endif
}


void PixPort::GaussBlur( int inBoxWidth, const Rect& inRect, void* inDestBits ) {

	// Don't let us draw in random parts of memory -- clip inRect
	__clipRect( inRect )


	if ( inBoxWidth <= 1 )
		return;

	// In Win32, everything's upside down
	#if EG_WIN
	r.top = mY - r.bottom;
	#endif

	// 3 box convolutions, 3 colors per pixel, 4 bytes per color
	int32_t         boxTempSize     = 36 * inBoxWidth;
	char*	tempBits	= 0;
	uint32_t*       boxTemp;
	int32_t imgOffset       = mBytesPerPix * r.left + r.top * mBytesPerRow;
	int32_t bytesNeeded     = mBytesPerRow * (mY + 2) + boxTempSize;


	// Resort to app's heap for temp mem if failed temp mem attempt or in win32
	tempBits = mBlurTemp.Dim( bytesNeeded );

	// Have the box temp and the pixel temp rgns use the same handle
	boxTemp = (uint32_t*) tempBits;
	tempBits += boxTempSize;

	if ( ! inDestBits )
		inDestBits = mBits;

	// Do a box blur on the x axis, transposing the source rgn to the dest rgn
	// Then o a box blur on the transposed image, effectively blurring the y cords, transposing it to the dest
	if ( mBytesPerPix == 2 )  {
		BoxBlur16( ( mBits + imgOffset), tempBits, inBoxWidth, width, height, mBytesPerRow, mBytesPerPix*height, boxTemp, mBackColor );
		BoxBlur16( tempBits, ((char*) inDestBits + imgOffset), inBoxWidth, height, width, mBytesPerPix*height, mBytesPerRow, boxTemp, mBackColor );  }
	else if ( mBytesPerPix == 4 ) {
		BoxBlur32( ( mBits + imgOffset), tempBits, inBoxWidth, width, height, mBytesPerRow, mBytesPerPix*height, boxTemp, mBackColor );
		BoxBlur32( tempBits, ((char*) inDestBits + imgOffset), inBoxWidth, height, width, mBytesPerPix*height, mBytesPerRow, boxTemp, mBackColor );
	}
}



void PixPort::CrossBlur( const Rect& inRect ) {

	// Don't let us draw in random parts of memory -- clip inRect
	__clipRect( inRect )


	// In Win32, everything's upside down
	#if EG_WIN
	r.top = mY - r.bottom;
	#endif

	// 3 box convolutions, 3 colors per pixel, 4 bytes per color
	int32_t imgOffset       = mBytesPerPix * r.left + r.top * mBytesPerRow;

	unsigned char* tempBits = (unsigned char*) mBlurTemp.Dim( mX * 3 );


	if ( mBytesPerPix == 2 )
		CrossBlur16( ( mBits + imgOffset), width, height, mBytesPerRow, tempBits );
	else if ( mBytesPerPix == 4 )
		CrossBlur32( ( mBits + imgOffset), width, height, mBytesPerRow, tempBits );

}

void PixPort::CopyBits( unsigned char* inOutVideo, const Rect* inSrce, const Rect* inDest ) {

	if (	inSrce -> left <= inSrce -> right && inSrce -> top <= inSrce -> bottom &&
			inDest -> left <= inDest -> right && inDest -> top <= inDest -> bottom ) {

		// FIXME do this in a C++ visual_mem_copy way.
		int i;
		unsigned char *imgBits = (unsigned char *) mBits;

		for ( i = 0; i < mY * mBytesPerRow; i++) {
			inOutVideo[ i ] = imgBits[ i ];
		}
	}
}

void PixPort::CopyBits( GrafPtr inPort, const Rect* inSrce, const Rect* inDest ) {

	if (	inSrce -> left <= inSrce -> right && inSrce -> top <= inSrce -> bottom &&
			inDest -> left <= inDest -> right && inDest -> top <= inDest -> bottom ) {



		#if EG_MAC
		::CopyBits( (BitMap*) *mBM, &inPort->portBits, inSrce, inDest, srcCopy, 0 );


		#elif EG_WIN
		//HDC hdc = ::GetDC( inDestWin );
		::BitBlt( inPort, inDest -> left, inDest -> top, inDest -> right - inDest -> left, inDest -> bottom - inDest -> top, mWorld, inSrce -> left, inSrce -> top, SRCCOPY );
		//::ReleaseDC( inDestWin, hdc );
		#elif defined(UNIX_X)
		// We don't handle this because we need private video buffer access
		#endif
	}
}


void PixPort::CopyBits( PixPort& inDestPort, const Rect* inSrce, const Rect* inDest ) {

	if (	inSrce -> left <= inSrce -> right && inSrce -> top <= inSrce -> bottom &&
			inDest -> left <= inDest -> right && inDest -> top <= inDest -> bottom ) {

		#if EG_MAC
		::CopyBits( (BitMap*) *mBM, (BitMap*) *inDestPort.mBM, inSrce, inDest, srcCopy, 0 );
		#elif EG_WIN
		::BitBlt( inDestPort.mWorld, inDest -> left, inDest -> top, inDest -> right - inDest -> left, inDest -> bottom - inDest -> top, mWorld, inSrce -> left, inSrce -> top, SRCCOPY );
		#endif
	}
}

void PixPort::Line( int sx, int sy, int ex, int ey, int32_t inColor ) {

	if ( mBytesPerPix == 2 )
		Line16( sx, sy, ex, ey, inColor );
	else if ( mBytesPerPix == 1 )
		Line8 ( sx, sy, ex, ey, inColor );
	else if ( mBytesPerPix == 4 )
		Line32( sx, sy, ex, ey, inColor );
}


#define __ABS( n )  ( ( n > 0 ) ? (n) : (-n) )
#define CLR_LINE_THR	520

void PixPort::Line( int sx, int sy, int ex, int ey, const RGBColor& inS, const RGBColor& inE ) {
	int32_t R, G, B, dR, dG, dB;

	R = inS.red;
	G = inS.green;
	B = inS.blue;
	dR = inE.red - R;
	dG = inE.green - G;
	dB = inE.blue - B;

	// If the endpoints have the same color, run the faster line procs (that just use one color)
	if (	dR > - CLR_LINE_THR && dR < CLR_LINE_THR &&
			dG > - CLR_LINE_THR && dG < CLR_LINE_THR &&
			dB > - CLR_LINE_THR && dB < CLR_LINE_THR ) {
		int32_t color;

		if ( mBytesPerPix == 2 ) {
			color = __Clr16( R, G, B );
			Line16( sx, sy, ex, ey, color ); }
		else if ( mBytesPerPix == 4 ) {
			color = __Clr32( R, G, B );
			Line32( sx, sy, ex, ey, color ); }
		else if ( mBytesPerPix == 1 ) {
			color = __Clr8( R, G, B );
			Line8 ( sx, sy, ex, ey, color );
		} }
	else {
		if ( mBytesPerPix == 2 )
			Line16( sx, sy, ex, ey, inS, dR, dG, dB );
		else if ( mBytesPerPix == 4 )
			Line32( sx, sy, ex, ey, inS, dR, dG, dB );
		else if ( mBytesPerPix == 1 )
			Line8 ( sx, sy, ex, ey, inS.red, dR );
	}
}



long PixPort::CreateFont() {
	PixTextStyle* newFont = new PixTextStyle;

	mFonts.Add( newFont );

	newFont -> mOSFontID = 0;

	return (long) newFont;
}



void PixPort::AssignFont( long inPixFontID, const char* inFontName, long inSize, long inStyleFlags ) {
	PixTextStyle* font = (PixTextStyle*) inPixFontID;

	font -> mFontName.Assign( inFontName );
	font -> mPointSize			= inSize;
	font -> mStyle				= inStyleFlags;
	font -> mOSStyle			= 0;
	font -> mDeviceLineHeight	= inSize;

	#if EG_MAC
	short fontNum;
	::GetFNum( font -> mFontName.getPasStr(), &fontNum );
	font -> mOSFontID = fontNum;
	if ( font -> mStyle & PP_BOLD )
		font -> mOSStyle |= bold;
	if ( font -> mStyle & PP_ITALIC )
		font -> mOSStyle |= italic;
	if ( font -> mStyle & PP_UNDERLINE )
		font -> mOSStyle |= underline;
	#endif


	#if EG_WIN
	long height = - MulDiv( inSize, ::GetDeviceCaps( mWorld, LOGPIXELSY ), 72 );
	font -> mFontName.Keep( 31 );
	font -> mOSFontID = (long) ::CreateFont( height, 0, 0, 0,
				( inStyleFlags & PP_BOLD ) ? FW_BOLD : FW_NORMAL,
				( inStyleFlags & PP_ITALIC ) ? true : false,
				( inStyleFlags & PP_UNDERLINE ) ? true : false,
				 0, DEFAULT_CHARSET,
					OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE, font -> mFontName.getCStr() );
	#endif

	#ifdef UNIX_X
	font -> mOSFontID = (long)mfl_LoadRawFont(DATADIR "/deffont");
	#endif
}






void PixPort::SelectFont( long inPixFontID ) {

	// Exit if we're already in in this text face
	if ( inPixFontID == mCurFontID )
		return;

	mCurFontID = inPixFontID;
	PixTextStyle* font = (PixTextStyle*) inPixFontID;
	mDeviceLineHeight = font -> mDeviceLineHeight;

	__setupPort

	#if EG_MAC
	::TextFont( font -> mOSFontID );
	::TextSize( font -> mPointSize );
	::TextFace( font -> mOSStyle );
	#endif

	#if EG_WIN
	::SelectObject( mWorld, (HFONT) font -> mOSFontID );
	#endif

	#ifdef UNIX_X
	mfl_SetFont(mWorld, (mfl_font) font->mOSFontID);
	#endif

	__restorePort
}


#include <stdio.h>
void PixPort::SetTextMode( PixDrawMode inMode ) {

	__setupPort

	#if EG_MAC
	long mode = srcCopy;
	if ( inMode == SRC_OR )
		mode = srcOr;
	else if ( inMode == SRC_BIC )
		mode = srcBic;
	else if ( inMode == SRC_XOR )
		mode = srcXor;
	::TextMode( mode );
	#endif


	#if EG_WIN
	long mode = R2_COPYPEN;
	if ( inMode == SRC_BIC )
		mode = R2_WHITE;
	else if ( inMode == SRC_XOR )
		mode = R2_NOT;
	::SetROP2( mWorld, mode );
	#endif

	#ifdef UNIX_X
	int mode = MFL_SETALL;
	if ( inMode == SRC_OR )
	  mode = MFL_OR;
	else if ( inMode == SRC_BIC )
	  mode = MFL_SETALL;
	else if ( inMode == SRC_XOR )
	  mode = MFL_XOR;
	mfl_SetDrawMode(mWorld, mode);
	#endif

	__restorePort
}

void PixPort::SetTextColor( RGBColor& inColor ) {


	__setupPort

	#if EG_MAC
	::RGBForeColor( &inColor );
	#endif

	#if EG_WIN
	::SetTextColor( mWorld, RGB( inColor.red >> 8, inColor.green >> 8, inColor.blue >> 8 ) );
	#endif

	#ifdef UNIX_X
#if 0
	/* This shouldn't ever happen.  If it did, the only way a color
	 * could be found would be by searching through the palette.
	 */
	fprintf(stderr, "r=%i g=%i b=%i\n", inColor.red,
			     inColor.green, inColor.blue);
#endif
	mfl_SetTextColor(mWorld, 255);
	#endif

	__restorePort

}

void PixPort::SetTextColor( PixPalEntry& inColor ) {

	#if EG_MAC
	SetTextColor( inColor.rgb );
	#endif

	#if EG_WIN
	::SetTextColor( mWorld, RGB( inColor.rgbRed, inColor.rgbGreen, inColor.rgbBlue ) );
	#endif

	#ifdef UNIX_X
	/* Palette index got stored */
	mfl_SetTextColor(mWorld, *((int32_t *)&inColor) >> 24);
	#endif
}

void PixPort::TextRect( const char* inStr, int32_t& outWidth, int32_t& outHeight ) {
	int32_t width, pos;
	char c;

	outWidth  = 0;
	outHeight = 0;

	__setupPort

	while ( *inStr ) {
		c = inStr[ 0 ];
		pos = 0;

		while ( c != '\r' && c ) {
			pos++;
			c = inStr[ pos ];
		}

		#if EG_MAC
		width = ::TextWidth( inStr, 0, pos );
		#endif

		#if EG_WIN
		SIZE dim;
		::GetTextExtentPoint( mWorld, inStr, pos, &dim );
		width  = dim.cx;
		#endif

		#ifdef UNIX_X
		width = mfl_GetTextWidthL(mWorld, inStr, pos);
		#endif

		if ( width > outWidth )
			outWidth = width;

		outHeight += mDeviceLineHeight;

		if ( c == 0 )
			break;

		inStr += pos + 1;
	}

	__restorePort

}


void PixPort::DrawText( int32_t inX, int32_t inY, const char* inStr ) {
	int32_t pos;
	char c;

	__setupPort

	while ( *inStr ) {
		c = inStr[ 0 ];
		pos = 0;

		while ( c != '\r' && c ) {
			pos++;
			c = inStr[ pos ];
		}

		#if EG_MAC
		::MoveTo( inX, inY );
		::DrawText( inStr, 0, pos );
		#endif

		#if EG_WIN
		::TextOut( mWorld, inX, inY, inStr, pos );
		#endif

		#ifdef UNIX_X
		mfl_OutText8L(mWorld, inX, inY, inStr, pos);
		#endif

		if ( c == 0 )
			break;

		inY += mDeviceLineHeight;
		inStr += pos + 1;
	}

	__restorePort
}

void PixPort::SetLineWidth( int32_t inLineWidth ) {
	if ( inLineWidth <= 0 )
		mLineWidth = 1;
	else if ( inLineWidth > MAX_LINE_WIDTH )
		mLineWidth = MAX_LINE_WIDTH;
	else
		mLineWidth = inLineWidth;
}

void PixPort::EraseRect( const Rect* inRect ) {

	if ( mBytesPerPix == 2 )
		EraseRect16( inRect );
	else if ( mBytesPerPix == 1 )
		EraseRect8 ( inRect );
	else if ( mBytesPerPix == 4 )
		EraseRect32( inRect );
}




#define P_SZ	1
#include "DrawXX.cpp"


#undef P_SZ
#define P_SZ	2
#include "DrawXX.cpp"


#undef P_SZ
#define P_SZ	4
#include "DrawXX.cpp"


#define HALFCORD	0x007F  /* 16 bits per cord, 8 bits for fixed decimal, 8 bits for whole number */
#define FIXED_BITS	8



// Assembly note w/ branch prediction:  the first block is chosen to be more probable

#include <stdio.h>
void PixPort::Fade( const char* inSrce, char* inDest, int32_t inBytesPerRow, int32_t inX, int32_t inY, uint32_t* grad ) {
	uint32_t u, v, u1, v1, P1, P2, P3, P4, p;
	const char* srceMap;
	const char* srce;

	// FIXME FIXME MMX SYNAP: OWyeha this is THE hot spot for optimalization!!!!

	// Setup the source row base address and offset to allow for negative grad components
	srce = inSrce - HALFCORD * inBytesPerRow - HALFCORD;

	// Start writing to the image...
	for ( int y = 0; y < inY; y++ ) {

		for ( int x = 0; x < inX; x++ ) {

			// Format of each long:
			// High byte: x (whole part), High-low byte: x (frac part)
			// Low-high byte: y (whole part), Low byte: y (frac part)
			u1 = *grad;
			grad ++;

			p = 0;

			// 0xFFFFFFFF is a signal that this pixel is black.
			if ( u1 != 0xFFFFFFFF )	{

				// Note that we use casting 3 times as an unsigned char to (smartly) get the compiler to do masking for us
			  // FIXME: BUG: Assumption that bytes per row equals width.  Also see _G-Force\ Common/DeltaField.*
				srceMap = srce + ( u1 >> 14 );
				v = ( u1 >> 7 ) & 0x7F;		// frac part of x
				u = ( u1      ) & 0x7F;		// frac part of y

				// In the end, the pixel intensity will be 31/32 of its current (interpolated) value
				v *= 31;

				/* Bilinear interpolation to approximate the source pixel value... */
				/* P1 - P2  */
				/* |     |  */
				/* P3 - P4  */
				P1  = ( (unsigned char*) srceMap )[0];
				P2  = ( (unsigned char*) srceMap )[1];
				u1	= 0x80 - u;
				P1  *= u1;
				P2  *= u1;
				v1 	=  3968 - v;  //  3968 == 31 * 0x80
				P3  = ( (unsigned char*) srceMap )[ inBytesPerRow ];
				P4  = ( (unsigned char*) srceMap )[ inBytesPerRow + 1 ];
				P3 *= u;
				P4 *= u;

				/* We can now calc the intensity of the pixel (truncating the fraction part of the pix value)  */
				/* We divide by (7+7+5) decimal places because p is units squared (7 places per decimal) and 5 more dec places cuz of the mult by 31 */
				p  = ( v * ( P2 + P4 ) + v1 * ( P1 + P3 ) ) >> 19;
			}
			( (unsigned char*) inDest )[ x ] = p;
		}

		inDest	+= inBytesPerRow;
		srce	+= inBytesPerRow;
	}
}


