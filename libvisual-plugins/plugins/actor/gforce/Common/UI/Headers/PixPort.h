#ifndef _PIXPORT_
#define _PIXPORT_

#include <stdint.h>

#include "EgCommon.h"

#include "UtilStr.h"
#include "TempMem.h"
#include "XPtrList.h"

#if EG_MAC
#include <QuickDraw.h>
#include <QDOffscreen.h>
typedef ColorSpec PixPalEntry;
#elif defined(UNIX_X)
struct PixPalEntry {
  unsigned char red, green, blue, x;
};
#endif

enum {
	REL_8_ROW = 5,
	FIXED_16_ROW = 6
};

class DeltaFieldData {
	public:
	
	//long			mNegYExtents;
	const char*		mField;
	//char*			mYExtentsBuf;
};



class PixTextStyle {
	public:
	int32_t                                 mPointSize;
	UtilStr					mFontName;
	int32_t                                 mStyle;
	int32_t                                 mDeviceLineHeight;
	long					mOSFontID;
	int32_t                                 mOSStyle;
};

enum PixDrawMode {
	SRC_OR,
	SRC_BIC,
	SRC_XOR
};

#define __Clr8(r,g,b)	( r >> 8 )
#define __Clr16(r,g,b) ((( ((uint32_t) r) & 0xF800) >> 1) | ((((uint32_t) g) & 0xF800) >> 6) | (((uint32_t) b) >> 11))
#if EG_MAC != 0 || defined(UNIX_X)
#define	__Clr32(r,g,b)	(((r & 0xFF00) << 8) | (g & 0xFF00) | (b >> 8))
#elif EG_WIN
typedef RGBQUAD PixPalEntry;
#define	__Clr32(r,g,b)	__winRGB( b, g, r )
#endif

#define __ClrREF( rgb ) __Clr32( rgb.red, rgb.green, rgb.blue )



/*  PixPort is a platform indep drawing world/environment.  You set the size of one using
Init() and then execute whatever drawing commands. When the drawing is complete, CopyBits() 
copies pixel data from this world to the given destination OS graphics world.  */
	
#define		PP_PLAIN		0
#define 	PP_BOLD			0x1
#define		PP_ITALIC		0x2
#define		PP_UNDERLINE	0x4




class PixPort {


	public:
	
		//friend class PixPort;
								 PixPort();
		virtual					~PixPort();


	
		// One or the other must be called before a PixPort is used.  If inDepth is 0 or invalid,
		// the current OS depth is used.  Call Deactivate() or Init() to exit fullscreen mode.
		// inWin is passed because PixPort may need the window that's going fullscreen
		void					Init( GrafPtr inPort );
		void					Init( int inWidth, int inHeight, int inDepth = 0 );
		

			

		// Returns the current bit color depth from Init(). (8, 16, or 32)
		int32_t                                 GetDepth(){ return mBytesPerPix * 8;      }
	
		// See how many bytes there are per row
		int32_t					GetRowSize() 										{	return mBytesPerRow;	}
		
		
		//	Sets the background colors (for erase rect and Blur() )
		//  Returns the new pixel entry for the given color and this port
		//  Note:  For 8 bit ports, the red 0-2^16 component maps directly to 0-255 pixel value
		int32_t					SetBackColor( const RGBColor& inColor );
		int32_t					SetBackColor( int32_t inR, int32_t inG, int32_t inB );

		//	Blurs the rect given in this image, with a given box filter of size (1=no blur)
		//	If the dest is 0, the blur is applied to itself
		void					GaussBlur( int inBoxWidth, const Rect& inRect, void* inDestBits = 0 );	
		
		//	A different, more primitive blur that doesn't look as good as GaussBlur,
		void					CrossBlur( const Rect& inRect );

		// 	Sets the width of the pen
		void					SetLineWidth( int32_t inWidth );
		
		//	Draw a line.  Use GetPortColor() to get a device color for a RGB
		void					Line( int sx, int sy, int ex, int ey, int32_t inColor );
		void					Line( int sx, int sy, int ex, int ey, const RGBColor& inS, const RGBColor& inE );

		//	Sets the clip rgn for all drawing operations.  0 for the cliprect means to remove all clipping.
		void					SetClipRect( const Rect* inRect = 0 );
		void					SetClipRect( int32_t inSX, int32_t inSY, int32_t inEX, int32_t inEY );
		
		//  Note:  For 8 bit ports, the red 0-2^16 component maps directly to 0-255 pixel value
		inline int32_t                          GetPortColor_inline( int32_t inR, int32_t inG, int32_t inB ) {
			if ( mBytesPerPix == 2 )
				return __Clr16( inR, inG, inB );
			else if ( mBytesPerPix == 4 ) 
				return __Clr32( inB, inG, inR );
			else
				return __Clr8( inR, inG, inB );
		}
		
		void					SetTextMode( PixDrawMode inMode );
		void					SetTextColor( PixPalEntry& inColor );
		void					SetTextColor( RGBColor& inColor );
		void                                    DrawText( int32_t inX, int32_t inY, UtilStr& inStr )                                                                    { DrawText( inX, inY, inStr.getCStr() );        }
		void                                    DrawText( int32_t inX, int32_t inY, const char* inStr );

		// See how big some text is going to be...
		void                                    TextRect( const char* inStr, int32_t& outWidth, int32_t& outHeight );
		
		//	Set the given rect to the current background color.  If no rect is specified, the entire port rect is cleared.
		void					EraseRect( const Rect* inRect = 0 );
		
		//	Copies the pixels in a rectangle from this pixel rect to the destination.  If the port is in fullscreen, inDestWin and inDest are ignored.
		void					CopyBits( unsigned char* inOutVideo, const Rect* inSrce, const Rect* inDest );
		void					CopyBits( GrafPtr inDestPort, const Rect* inSrce, const Rect* inDest );
		void					CopyBits( PixPort& inDestPort, const Rect* inSrce, const Rect* inDest );
				
		//	Gets a port/device color for a given RGB		
		//  Note:  For 8 bit ports, the red 0-2^16 component maps directly to 0-255 pixel value
		int32_t                                 GetPortColor( int32_t inR, int32_t inG, int32_t inB );
		inline int32_t                          GetPortColor( const RGBColor& inColor )                 { return GetPortColor( inColor.red, inColor.green, inColor.blue );  }
	
		//	The guts for G-Force...  This PixPort must be in 8-bit mode to do anything.
#if 0
		void					Fade( DeltaFieldData* inGrad )									{ Fade( mBits, mBytesPerRow, mX, mY, inGrad ); } 

#endif
		void                                    Fade( PixPort& inDest, DeltaFieldData* inGrad )                                 { Fade( mBits, inDest.mBits, mBytesPerRow, mX, mY, (uint32_t*) inGrad -> mField ); }
		
	
		//	When this sprocket is set to 256 colors, you may change the palette it's using any time
		//	Pre: inColors[].rgb is the RGB of palette entry i.
		//	Post:  The current palette is set to inColors[]
		//	If inSysWindow holds an OS window, SetPalette will set that particular window to the palette give in inColors and force the OS palette to this palette.
		void					SetPalette( PixPalEntry inPal[ 256 ] );
		
		//	Set the given window to this PixPort's palette, assuming that the given window already has a matching palette to this PixPort
		//  This prevents the OS from recalculating palette info if we already know the two match.
		//  Note: if the given window or this port isn't in 8 bit mode, this fcn does nothing.
		void					PreventActivate( GrafPtr inSysWindow );

		//	To draw in a new font, you:
		//		1) Call CreateFont() and PixPort will return you a fontID
		//		2) Call AssignFont() to set various font characteristic about it
		//		3) Call SelectFont() to set it as the current font for subsequent text drawing
		long					CreateFont();
		void					AssignFont( long inPixFontID, const char* inFontName, long inSize, long inStyleFlags = PP_PLAIN );
		void					SelectFont( long inPixFontID );


		int32_t					GetX()			{ return mX; }
		int32_t					GetY()			{ return mY; }
				
		
		static char*			sTemp;
		static int32_t				sTempSize;


		#if EG_MAC
		PixMapHandle			GetPixMap()											{ return mWorld -> portPixMap; }
		#endif
		
		#define MAX_LINE_WIDTH		32
	
		
	protected:

	
		Rect					mClipRect;
		int32_t					mBytesPerPix;
		int32_t					mBytesPerRow;
		int32_t					mX, mY;
		int32_t					mBackColor;
		int32_t					mLineWidth;
		
		char*					mBits;
		
		PixMapHandle			mBM;
		GWorldPtr				mWorld;
		
		TempMem					mBlurTemp;
		
		XPtrList				mFonts;
		long					mCurFontID;
		int32_t					mDeviceLineHeight;
					
		#if EG_WIN
		BITMAPINFO				mInfo;
		#endif
		
		void					Un_Init();
		
		void					EraseRect8 ( const Rect* inRect );
		void					EraseRect16( const Rect* inRect );
		void					EraseRect32( const Rect* inRect );

		static void                             BoxBlur8 ( char* inSrce, char* inDest, int inBoxWidth, int inWidth, int inHeight, int inSrceRowSize, int inDestRowSize, uint32_t* temp, uint32_t inBackColor );
		static void                             BoxBlur16( char* inSrce, char* inDest, int inBoxWidth, int inWidth, int inHeight, int inSrceRowSize, int inDestRowSize, uint32_t* temp, uint32_t inBackColor );
		static void                             BoxBlur32( char* inSrce, char* inDest, int inBoxWidth, int inWidth, int inHeight, int inSrceRowSize, int inDestRowSize, uint32_t* temp, uint32_t inBackColor ); 

		static void				CrossBlur8 ( char* inSrce, int inWidth, int inHeight, int inBytesPerRow, unsigned char* inRowBuf );
		static void				CrossBlur16( char* inSrce, int inWidth, int inHeight, int inBytesPerRow, unsigned char* inRowBuf );
		static void				CrossBlur32( char* inSrce, int inWidth, int inHeight, int inBytesPerRow, unsigned char* inRowBuf );

		void                                    Line8 ( int sx, int sy, int ex, int ey, int32_t inColor );
		void                                    Line16( int sx, int sy, int ex, int ey, int32_t inColor );
		void                                    Line32( int sx, int sy, int ex, int ey, int32_t inColor );


		void                                    Line8 ( int sx, int sy, int ex, int ey, int32_t inR, int32_t dR );
		void                                    Line16( int sx, int sy, int ex, int ey, const RGBColor& inS, int32_t dR, int32_t dG, int32_t dB );
		void                                    Line32( int sx, int sy, int ex, int ey, const RGBColor& inS, int32_t dR, int32_t dG, int32_t dB );

		static void                             Fade( const char* inSrce, char* inDest, int32_t inBytesPerRow, int32_t inX, int32_t inY, uint32_t* inGrad );
		static void                             Fade( char* ioPix, int32_t inBytesPerRow, int32_t inX, int32_t inY, DeltaFieldData* inGrad );
};

#endif
