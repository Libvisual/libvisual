/*------------------------------------------------------------------------------
#
#	MacOSª Sample Code
#	
#	Written by: Eric Anderson
#	 AppleLink: ERIC3
#
#	Display Manager sample code
#
#	RequestVideo
#
#	RequestVideo.h	-	C Header
#
#	Copyright © 1995 Apple Computer, Inc.
#	All rights reserved.
#
#	Revision History:
#
#	5/31/95		ewa		Added RVGetCurrentVideoSetting and RVConfirmVideoRequest routines
#						to make it easy to revert back to where you came from and to give
#						the user a chance to confirm the new setting if the new mode was
#						valid (ie: the card supports it) but not safe (the monitor may not).
#	5/24/95		ewa		Give the kAllValidModesBit requestFlags option for safe only or all
#						valid resolution timings.
#
#
#
#	Components:	PlayVideo.c			
#				RequestVideo.c		
#				RequestVideo.h		
#				RequestVideo.rsrc		
#
#	For information on the use of this sample code, please the documentation in the Read Me file
------------------------------------------------------------------------------*/
 
#include <QuickDraw.h>
#include <Video.h>

// requestFlags bit values in VideoRequestRec (example use: 1<<kAbsoluteRequestBit)
enum {
	kBitDepthPriorityBit		= 0,	// Bit depth setting has priority over resolution
	kAbsoluteRequestBit			= 1,	// Available setting must match request
	kShallowDepthBit			= 2,	// Match bit depth less than or equal to request
	kMaximizeResBit				= 3,	// Match screen resolution greater than or equal to request
	kAllValidModesBit			= 4		// Match display with valid timing modes (may include modes which are not marked as safe)
};

// availFlags bit values in VideoRequestRec (example use: 1<<kModeValidNotSafeBit)
enum {
	kModeValidNotSafeBit		= 0		//  Available timing mode is valid but not safe (requires user confirmation of switch)
};

// video request structure
struct VideoRequestRec	{
	GDHandle		screenDevice;		// <in/out>	nil will force search of best device, otherwise search this device only
	short			reqBitDepth;		// <in>		requested bit depth
	short			availBitDepth;		// <out>	available bit depth
	unsigned long	reqHorizontal;		// <in>		requested horizontal resolution
	unsigned long	reqVertical;		// <in>		requested vertical resolution
	unsigned long	availHorizontal;	// <out>	available horizontal resolution
	unsigned long	availVertical;		// <out>	available vertical resolution
	unsigned long	requestFlags;		// <in>		request flags
	unsigned long	availFlags;			// <out>	available mode flags
	unsigned long	displayMode;		// <out>	mode used to set the screen resolution
	unsigned long	depthMode;			// <out>	mode used to set the depth
	VDSwitchInfoRec	switchInfo;			// <out>	DM2.0 uses this rather than displayMode/depthMode combo
};
typedef struct VideoRequestRec VideoRequestRec;
typedef struct VideoRequestRec *VideoRequestRecPtr;

// Routine defines
OSErr RVRequestVideoSetting(VideoRequestRecPtr requestRecPtr);
OSErr RVGetCurrentVideoSetting(VideoRequestRecPtr requestRecPtr);
OSErr RVSetVideoRequest (VideoRequestRecPtr requestRecPtr);
OSErr RVConfirmVideoRequest (VideoRequestRecPtr requestRecPtr);
OSErr RVSetVideoAsScreenPrefs (void);


//  ### Here's some example code ###
#if 0


#define minHorizontalRequest		640
#define minVerticalRequest			480

#define maxHorizontalRequest		2000
#define maxVerticalRequest			2000

#define bitDepthRequest				32

#include <Types.h>
#include <Quickdraw.h>

#include "RequestVideo.h"					// The code that does the real work

struct INITGlobals {
	QDGlobals		initQDGlobals;			// FKEY's own private QDGlobals
	unsigned long	initQDBase;				// FKEY's global base
};
typedef struct INITGlobals INITGlobals;

main()
{
	VideoRequestRec	requestRec;
	INITGlobals		qdGlobs;
	long			oldA5;
	unsigned long	theHorizontalRequest;
	unsigned long	theVerticalRequest;		
	
	oldA5 = SetA5((long) &qdGlobs.initQDBase);
	InitGraf((Ptr) &qdGlobs.initQDGlobals.thePort);
	
	theHorizontalRequest = minHorizontalRequest;
	theVerticalRequest = minVerticalRequest;

	requestRec.screenDevice = GetMainDevice ();
	requestRec.reqHorizontal = (*(*(requestRec.screenDevice))->gdPMap)->bounds.right;	// main screen is always zero offset (bounds.left == 0)
	requestRec.reqVertical = (*(*(requestRec.screenDevice))->gdPMap)->bounds.bottom;	// main screen is always zero offset (bounds.top == 0)
	 if (requestRec.reqHorizontal == minHorizontalRequest || requestRec.reqVertical == minVerticalRequest)	// on a small screen now?
	 {
 		theHorizontalRequest	=	maxHorizontalRequest;
 		theVerticalRequest		=	maxVerticalRequest;
	 }

	requestRec.reqBitDepth		=	bitDepthRequest;		// bit depth request
	requestRec.reqHorizontal	=	theHorizontalRequest;	// H request
	requestRec.reqVertical		=	theVerticalRequest;		// V request
	requestRec.displayMode		=	nil;					// must init to nil
	requestRec.depthMode		=	nil;					// must init to nil
	requestRec.requestFlags		=	0;						
	if (noErr == RVRequestVideoSetting(&requestRec))
	{
		RVSetVideoRequest (&requestRec);
	}
	SetA5(oldA5);
	return (noErr);
}

#endif
