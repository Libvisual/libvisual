#ifndef SCREENDEVICE_H
#define SCREENDEVICE_H

#ifdef UNIX_X
#include "EgCommon.h"
#include "PixPort.h"
#endif

// ### If Mac, in your global header, define USE_DRAW_SPROCKETS 1 or USE_DISP_MGR 1 if you want full screeen ability!!!
// ### If Win, in your global header, define USE_DIRECTX 1 if you want full screeen ability!!!

#if EG_WIN

typedef RGBQUAD PixPalEntry;

#if USE_DIRECTX
#include <ddraw.h>
#else
typedef long LPDIRECTDRAW;
typedef long LPDIRECTDRAWSURFACE;
#endif
#endif



#if EG_MAC
#include <QuickDraw.h>
#include <QDOffscreen.h>
#include <LowMem.h>

typedef ColorSpec PixPalEntry;


#if USE_DISP_MGR
	#undef USE_DRAW_SPROCKETS
	#include "RequestVideo.h"
#elif USE_DRAW_SPROCKETS
	#include <DrawSprocket.h>
#else
	typedef long DSpContextReference;
	struct DSpContextAttributes	{ };
#endif
#else
#endif




/* An instance of a ScreenDevice represents a structure that can tell 
a screen device to go into full screen mode.  */

class ScreenDevice {


	public:

		// Returns true if EnterFullscreen() can work for any device
		//bool					FullscreenAvail()							{ return mCanFullscreen;	}


								ScreenDevice();
								~ScreenDevice();
		
		// Two different ways to obtain a display ID...
		// Use the long returned here for EnterFullscreen()...
		// Returns the (inDeviceNum)th display ID
		static long				GetDisplayID( long inDeviceNum );
		// Returns a display ID that contains the given global coordinate
		static long				GetDisplayID( long inX, long inY );
		
		// Returns true if this ScreenDevice is currently fullscreen (ie, if EnterFullscreen has been called)
		inline bool				IsFullscreen()									{ return mContextRef != 0; }

		// Use these fcns to enter & exit fullscreen mode.  ioSize contains the desired screen size 
		// on entry and contains the final fullscreen res dimentions upon return.	
		// inWin expands to fit fullscreen size if in Windows *or* we're no using mac drawsprockets	
		bool					EnterFullscreen( long inDispID, Point& ioSize, int inBitDepth, WindowPtr inWin );
		void					ExitFullscreen();
		
		// Encase these before using the port returned by GetPort();
		//	Be sure this is called before and after *any* of the following calls
		GrafPtr					BeginFrame();
		void					EndFrame();

		void					SetPalette( PixPalEntry inPal[ 256 ] );

		static long				sMinDepth;
		static long				sOSDepth;

		
	protected:
		
		long					mDispID;
		long					mBitDepth;			 
		GrafPtr					mFS_DC;
		
		#if USE_DRAW_SPROCKETS
		DSpContextReference		mContextRef;
		DSpContextAttributes	mContext;
		#elif USE_DISP_MGR
		GrafPtr					mContextRef;
		RgnHandle				mMenuBarRgn;
		long					mMenuBarHeight;
		#elif EG_WIN
		LPDIRECTDRAWSURFACE		mContextRef;
		LPDIRECTDRAW			mDDObj;
		LPDIRECTDRAWPALETTE FAR	mFS_Palette;
		HWND					mFS_Win;
		#else			
		long					mContextRef;
		#endif


};

#endif
