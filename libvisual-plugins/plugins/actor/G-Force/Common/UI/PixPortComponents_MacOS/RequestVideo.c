/*------------------------------------------------------------------------------
#
#	MacOSª Sample Code
#	
#	Written by: Eric Anderson
#	 AppleLink: ERIC3
#	 AOL: ERICTHREE
#
#	Display Manager sample code
#
#	RequestVideo
#
#	RequestVideo.c	-	C Code
#
#	Copyright © 1995 Apple Computer, Inc.
#	All rights reserved.
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
#	RequestVideo demonstrates the usage of the Display Manager introduced
#	with the PowerMacs and integrated into the system under System 7.5. With
#	the RequestVideo sample code library, developers will be able to explore
#	the Display Manager API by changing bit depth and screen resolution on
#	multisync displays on built-in, NuBus, and PCI based video. Display Manager 1.0
#	is built into the Systems included with the first PowerMacs up through System 7.5.
#	Display Manager 2.0 is included with the release of the new PCI based PowerMacs,
#	and will be included in post 7.5 System Software releases. 
#	
#	It is a good idea to reset the screen(s) to the original setting before exit
#	since the call to RVSetVideoAsScreenPrefs() may not do the right thing under
#	Display Manager 1.0 with certain video drivers.
#
#	For information on the use of this sample code, please the documentation in the Read Me file
------------------------------------------------------------------------------*/

#include "RequestVideo.h"

// Internal includes
#include <Dialogs.h>
#include <ROMDefs.h>
#include <Devices.h>
#include <Errors.h>
#include <Gestalt.h>
#include <Memory.h>
#include <Palettes.h>
#include <Slots.h>
#include <StdIO.h>
#include <Displays.h>

#include <stdlib.h>

//--------------------------------------------------------------
//
// Internal defines, structs, typedefs, and routine declarations
//
//--------------------------------------------------------------
#define		KMonoDev			0						// false (handy definitions for gdDevType settings)
#define		kColorDev			1						// true
#define		char_Enter			0x03					// for our filter proc
#define		char_Return			0x0D					//
#define		iRevertItem			1						// User buttons
#define		iConfirmItem		2						//
#define		kSecondsToConfirm	8						// seconds before confirm dialog is taken down
#define		rConfirmSwtchAlrt	2735					// ID of alert dialog

struct DepthInfo {
	VDSwitchInfoRec			depthSwitchInfo;			// This is the switch mode to choose this timing/depth
	VPBlock					depthVPBlock;				// VPBlock (including size, depth and format)
};
typedef struct DepthInfo DepthInfo;

struct ListIteratorDataRec {
	VDTimingInfoRec			displayModeTimingInfo;		// Contains timing flags and such
	unsigned long			depthBlockCount;			// How many depths available for a particular timing
	DepthInfo				*depthBlocks;				// Array of DepthInfo
};
typedef struct ListIteratorDataRec ListIteratorDataRec;

void GetRequestTheDM1Way (		VideoRequestRecPtr requestRecPtr,
								GDHandle walkDevice);

void GetRequestTheDM2Way (		VideoRequestRecPtr requestRecPtr,
								GDHandle walkDevice,
								DMDisplayModeListIteratorUPP myModeIteratorProc,
								DMListIndexType theDisplayModeCount,
								DMListType *theDisplayModeList);

pascal void ModeListIterator (	void *userData,
								DMListIndexType itemIndex,
								DMDisplayModeListEntryPtr displaymodeInfo);

Boolean FindBestMatch (			VideoRequestRecPtr requestRecPtr,
								short bitDepth,
								unsigned long horizontal,
								unsigned long vertical);

void GravitateMonitors (void);

pascal Boolean ConfirmAlertFilter (DialogPtr dlg, EventRecord *evt, short *itemHit);

//--------------------------------------------------------------
//
// Implementation of sample code
//
//--------------------------------------------------------------
OSErr RVSetVideoRequest (VideoRequestRecPtr requestRecPtr)
{
	GDHandle		aMonitor;
	Boolean			displayMgrPresent;
	unsigned long	displayMgrVersion;
	OSErr			err;
	Boolean			isColor;
	long			value = 0;

	Gestalt(gestaltDisplayMgrVers, (long*)&displayMgrVersion);
	Gestalt(gestaltDisplayMgrAttr,&value);
	displayMgrPresent=value&(1<<gestaltDisplayMgrPresent);
	if (displayMgrPresent)
	{
		if (requestRecPtr->displayMode && requestRecPtr->depthMode)
		{
			if (requestRecPtr->availBitDepth == 1)	// Based on avail bit depth, 
				isColor = KMonoDev;					// set the device to a mono device, or
			else isColor = kColorDev;				// set the device to a color device
			SetDeviceAttribute(requestRecPtr->screenDevice,gdDevType,isColor);		
			
			// see how many monitors we have, aMonitor will be nil if we have only one.
			aMonitor = DMGetFirstScreenDevice (dmOnlyActiveDisplays);			// get the first guy
			aMonitor = DMGetNextScreenDevice ( aMonitor, dmOnlyActiveDisplays );	// get the next guy
			
			if (nil == aMonitor || displayMgrVersion >= 0x00020000)
			{
				// only call DMSetDisplayMode if we have one monitor or DM2.0 is installed
				// since DM1.0 does not automatically gravitate monitors and our gravitate code
				// is not implemented.
				err = DMSetDisplayMode(	requestRecPtr->screenDevice,	// GDevice
						requestRecPtr->displayMode,						// DM1.0 uses this
						&requestRecPtr->depthMode,						// DM1.0 uses this
						(unsigned long) &(requestRecPtr->switchInfo),	// DM2.0 uses this rather than displayMode/depthMode combo
						nil);
				if (noErr == err)
				{
					// Do the monitor gravitate here if we are using a version less than DM2.0
					if (displayMgrVersion < 0x00020000)
						GravitateMonitors ();
				}
				else if (kDMDriverNotDisplayMgrAwareErr == err)
				{
					// DM not supported by driver, so all we can do is set the bit depth
					err = SetDepth (requestRecPtr->screenDevice, requestRecPtr->depthMode, gdDevType, isColor);
				}
			}
			else
			{
				// we have more than one monitor and DM1.0 is installed, so all we can do is set the bit depth
				err = SetDepth (requestRecPtr->screenDevice, requestRecPtr->depthMode, gdDevType, isColor);
			}
			
			return (err);	// we did try to set the request
		}
	}
	return (-1);	// return a generic error
}

// This extern should be removed once this function is formally defined in Displays.h
extern pascal OSErr DMUseScreenPrefs(Boolean usePrefs, Handle displayState)
 THREEWORDINLINE(0x303C, 0x03EC, 0xABEB);

OSErr RVSetVideoAsScreenPrefs (void)
{
	Handle		displaystate;
	Boolean		displayMgrPresent;
	long		value = 0;

	Gestalt(gestaltDisplayMgrAttr,&value);
	displayMgrPresent=value&(1<<gestaltDisplayMgrPresent);
	if (displayMgrPresent)
	{
		DMBeginConfigureDisplays (&displaystate);	// Tell the world it is about to change
		DMUseScreenPrefs (true, displaystate);		// Make the change
		DMEndConfigureDisplays (displaystate);		// Tell the world the change is over
		
		return (noErr);	// we (maybe) set the world back to a known setting
	}
	return (-1);	// return a generic error
}

OSErr RVGetCurrentVideoSetting (VideoRequestRecPtr requestRecPtr)
{
	unsigned long		displayMgrVersion;
	OSErr				error = paramErr;
	CntrlParam			pBlock;
	VDSwitchInfoRec		switchInfo;
	AuxDCEHandle		theDCE;
	VDSwitchInfoRec		videoMode;		

	requestRecPtr->availBitDepth			= 0;	// init to default - you can do it if it is important to you
	requestRecPtr->availHorizontal			= 0;
	requestRecPtr->availVertical			= 0;
	requestRecPtr->availFlags				= 0;
	requestRecPtr->displayMode				= -1; 
	requestRecPtr->depthMode				= -1;
	requestRecPtr->switchInfo.csMode		= 0;
	requestRecPtr->switchInfo.csData		= 0;
	requestRecPtr->switchInfo.csPage		= 0;
	requestRecPtr->switchInfo.csBaseAddr	= 0;
	requestRecPtr->switchInfo.csReserved	= 0;
	
	Gestalt(gestaltDisplayMgrVers, (long*)&displayMgrVersion);
	if (requestRecPtr->screenDevice)
	{
		if (displayMgrVersion >= 0x00020000)
		{	// get the info the DM 2.0 way
			error = DMGetDisplayMode(requestRecPtr->screenDevice, &switchInfo);
			if (noErr == error)
			{
				requestRecPtr->depthMode			= switchInfo.csMode;
				requestRecPtr->displayMode			= switchInfo.csData; 
				requestRecPtr->switchInfo.csMode	= switchInfo.csMode;
				requestRecPtr->switchInfo.csData	= switchInfo.csData;
			}
			return (error);	// we (maybe) set the world back to a known setting
		}
		else
		{	// get the info the DM 1.0 way
			videoMode.csMode = -1;		// init to bogus value
			videoMode.csData = -1;		// init to bogus value			
			pBlock.ioNamePtr = nil;
			pBlock.ioCRefNum = (*(requestRecPtr->screenDevice))->gdRefNum;
			pBlock.csCode = cscGetCurMode;
			*(Ptr *)&pBlock.csParam[0] = (Ptr)&videoMode;
				
			error = PBStatusSync((ParmBlkPtr )&pBlock);	// ask the driver first....since we trust it the most
				
			if ( noErr == error && ((-1 == videoMode.csMode) || (-1 == videoMode.csData)) )
				error = statusErr;
			
			if (noErr != error)	// if the driver has no clue fill it videoMode by hand as a last resort
			{	
				theDCE = (AuxDCEHandle)GetDCtlEntry((*(requestRecPtr->screenDevice))->gdRefNum);
				
				if( theDCE )
				{
					videoMode.csData = (unsigned char)(*theDCE)->dCtlSlotId; 
					videoMode.csMode = (*(requestRecPtr->screenDevice))->gdMode;
					error = noErr;
				}
			}
			if (noErr == error)	// Set our data
			{
				requestRecPtr->displayMode			= videoMode.csData; 
				requestRecPtr->depthMode			= videoMode.csMode;
				requestRecPtr->switchInfo.csMode	= videoMode.csMode;
				requestRecPtr->switchInfo.csData	= videoMode.csData;
			}
			return (error);	// we (maybe) set the world back to a known setting
		}
	}
	return (-1);
}

pascal Boolean ConfirmAlertFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit)
{
	char charCode;
	Boolean enterORreturn;
	Boolean returnValue = false;

	if (0 == GetWRefCon(theDialog))
		SetWRefCon (theDialog,TickCount());
	else
	{
		if (GetWRefCon(theDialog) + kSecondsToConfirm * 60 < TickCount())
		{
			returnValue = true;
			theEvent->what = nullEvent;
			*itemHit = 1;
		}
		else
		{
			if (theEvent->what == keyDown)
			{
				charCode = (char)theEvent->message & charCodeMask;
				enterORreturn = (charCode == (char)char_Return) || (charCode == (char)char_Enter);
				if (enterORreturn)
				{
					theEvent->what = nullEvent;
					returnValue = true;
					*itemHit = iRevertItem;
					if (enterORreturn && (0 != (theEvent->modifiers & optionKey)))
					{
						*itemHit = iConfirmItem;
					}
				}
			}
		}
	}
	return (returnValue);
}

OSErr RVConfirmVideoRequest (VideoRequestRecPtr requestRecPtr)
{
	short			alertReturn;		// Alert() return value
	ModalFilterUPP	confirmFilterUPP;	// got to have us one of them new fangled UPP thingies
	
	if (requestRecPtr->availFlags & 1<<kModeValidNotSafeBit)
	{	// new mode is valid but not safe, so ask user to confirm
		SetCursor(&qd.arrow);										// have to show the arrow

		confirmFilterUPP = NewModalFilterProc (ConfirmAlertFilter);	// create a new modal filter proc UPP
		alertReturn = Alert(rConfirmSwtchAlrt, confirmFilterUPP);	// alert the user
		DisposeRoutineDescriptor (confirmFilterUPP);				// of course there is no DisposeModalFilterProc...
		
		if (alertReturn != iConfirmItem)
			return (-1);							// tell the caller to switch back to a known setting
		else return (noErr);						// all is well with the new setting, just leave it
	}
	return (noErr);									// the mode was safe, so do nothing
}


OSErr RVRequestVideoSetting (VideoRequestRecPtr requestRecPtr)
{
	Boolean							displayMgrPresent;
	short							iCount = 0;					// just a counter of GDevices we have seen
	DMDisplayModeListIteratorUPP	myModeIteratorProc = nil;	// for DM2.0 searches
	SpBlock							spBlock;
	Boolean							suppliedGDevice;	
	DisplayIDType					theDisplayID;				// for DM2.0 searches
	DMListIndexType					theDisplayModeCount;		// for DM2.0 searches
	DMListType						theDisplayModeList;			// for DM2.0 searches
	long							value = 0;
	GDHandle						walkDevice = nil;			// for everybody

	Gestalt(gestaltDisplayMgrAttr,&value);
	displayMgrPresent=value&(1<<gestaltDisplayMgrPresent);
	displayMgrPresent=displayMgrPresent && (SVersion(&spBlock)==noErr);	// need slot manager
	if (displayMgrPresent)
	{
		// init the needed data before we start
		if (requestRecPtr->screenDevice)							// user wants a specifc device?
		{
			walkDevice = requestRecPtr->screenDevice;
			suppliedGDevice = true;
		}
		else
		{
			walkDevice = DMGetFirstScreenDevice (dmOnlyActiveDisplays);			// for everybody
			suppliedGDevice = false;
		}
		
		myModeIteratorProc = NewDMDisplayModeListIteratorProc(ModeListIterator);	// for DM2.0 searches
	
		// Note that we are hosed if somebody changes the gdevice list behind our backs while we are iterating....
		// ...now do the loop if we can start
		if( walkDevice && myModeIteratorProc) do // start the search
		{
			iCount++;		// GDevice we are looking at (just a counter)
			if( noErr == DMGetDisplayIDByGDevice( walkDevice, &theDisplayID, false ) )	// DM1.0 does not need this, but it fits in the loop
			{
				theDisplayModeCount = 0;	// for DM2.0 searches
				if (noErr == DMNewDisplayModeList(theDisplayID, 0, 0, &theDisplayModeCount, &theDisplayModeList) )
				{
					// search NuBus & PCI the new kool way through Display Manager 2.0
					GetRequestTheDM2Way (requestRecPtr, walkDevice, myModeIteratorProc, theDisplayModeCount, &theDisplayModeList);
					DMDisposeList(theDisplayModeList);	// now toss the lists for this gdevice and go on to the next one
				}
				else
				{
					// search NuBus only the old disgusting way through the slot manager
					GetRequestTheDM1Way (requestRecPtr, walkDevice);
				}
			}
		} while ( !suppliedGDevice && nil != (walkDevice = DMGetNextScreenDevice ( walkDevice, dmOnlyActiveDisplays )) );	// go until no more gdevices
		if( myModeIteratorProc )
			DisposeRoutineDescriptor(myModeIteratorProc);
		return (noErr);	// we were able to get the look for a match
	}
	return (-1);		// return a generic error
}

void GetRequestTheDM1Way (VideoRequestRecPtr requestRecPtr, GDHandle walkDevice)
{
	AuxDCEHandle myAuxDCEHandle;
	unsigned long	depthMode;
	unsigned long	displayMode;
	OSErr			error;
	OSErr			errorEndOfTimings;
	short			height;
	short			jCount = 0;
	Boolean			modeOk;
	SpBlock			spAuxBlock;
	SpBlock			spBlock;
	unsigned long	switchFlags;
	VPBlock			*vpData;
	short			width;

	myAuxDCEHandle = (AuxDCEHandle) GetDCtlEntry((**walkDevice).gdRefNum);	
	spBlock.spSlot = (**myAuxDCEHandle).dCtlSlot;
	spBlock.spID = (**myAuxDCEHandle).dCtlSlotId;
	spBlock.spExtDev = (**myAuxDCEHandle).dCtlExtDev;
	spBlock.spHwDev = 0;								// we are going to get this pup
	spBlock.spParamData = 1<<foneslot;					// this slot, enabled, and it better be here.
	spBlock.spTBMask = 3;								// don't have constants for this yet
	errorEndOfTimings = SGetSRsrc(&spBlock);			// get the spDrvrHW so we know the ID of this puppy. This is important
														// since some video cards support more than one display, and the spDrvrHW
														// ID can, and will, be used to differentiate them.
	
	if ( noErr == errorEndOfTimings )
	{
		// reinit the param block for the SGetTypeSRsrc loop, keep the spDrvrHW we just got
		spBlock.spID = 0;								// start at zero, 
		spBlock.spTBMask = 2;							// 0b0010 - ignore DrvrSW - why ignore the SW side? Is it not important for video?
		spBlock.spParamData = (1<<fall) + (1<<foneslot) + (1<<fnext);	// 0b0111 - this slot, enabled or disabled, so we even get 640x399 on Blackbird
		spBlock.spCategory=catDisplay;
		spBlock.spCType=typeVideo;
		errorEndOfTimings = SGetTypeSRsrc(&spBlock);	// but only on 7.0 systems, not a problem since we require DM1.0
		
		// now, loop through all the timings for this GDevice
		if ( noErr == errorEndOfTimings ) do
		{
			// now, loop through all possible depth modes for this timing mode
			displayMode = (unsigned char)spBlock.spID;	// "timing mode, ie:resource ref number"
			for (jCount = firstVidMode; jCount<= sixthVidMode; jCount++)
			{
				depthMode = jCount;		// vid mode
				error = DMCheckDisplayMode(walkDevice,displayMode,depthMode,&switchFlags,0,&modeOk);
	
				// only if the mode is safe or we override it with the kAllValidModesBit request flag
				if (	noErr == error &&
						modeOk &&
						(	switchFlags & 1<<kNoSwitchConfirmBit ||
							requestRecPtr->requestFlags & 1<<kAllValidModesBit
						)
					)
				{
					// have a good displayMode/depthMode combo - now lets look inside
					spAuxBlock = spBlock;				// don't ruin the iteration spBlock!!
					spAuxBlock.spID = depthMode;		// vid mode
					error=SFindStruct(&spAuxBlock);		// get back a new spsPointer
					if (noErr == error)					// keep going if no errorÉ
					{
						spAuxBlock.spID = 0x01;			// mVidParams request
						error=SGetBlock (&spAuxBlock);	// use the new spPointer and get back...a NewPtr'ed spResult
						if (noErr == error)				// Ékeep going if no errorÉ
						{								// We have data! lets have a look
							vpData = (VPBlock*)spAuxBlock.spResult;
							height = vpData->vpBounds.bottom;	// left and top are usually zero
							width = vpData->vpBounds.right;
							
							if (FindBestMatch (requestRecPtr, vpData->vpPixelSize, vpData->vpBounds.right, vpData->vpBounds.bottom))
							{
								requestRecPtr->screenDevice = walkDevice;
								requestRecPtr->availBitDepth = vpData->vpPixelSize;
								requestRecPtr->availHorizontal = vpData->vpBounds.right;
								requestRecPtr->availVertical = vpData->vpBounds.bottom;
								requestRecPtr->displayMode = displayMode;
								requestRecPtr->depthMode = depthMode;
								requestRecPtr->switchInfo.csMode = depthMode;				// fill in for completeness
								requestRecPtr->switchInfo.csData = displayMode;
								requestRecPtr->switchInfo.csPage = 0;
								requestRecPtr->switchInfo.csBaseAddr = 0;
								requestRecPtr->switchInfo.csReserved = 0;
								if (switchFlags & 1<<kNoSwitchConfirmBit)
									requestRecPtr->availFlags = 0;							// mode safe
								else requestRecPtr->availFlags = 1<<kModeValidNotSafeBit;	// mode valid but not safe, requires user validation of mode switch
							}

							if (spAuxBlock.spResult) DisposePtr ((Ptr)spAuxBlock.spResult);	// toss this puppy when done
						}
					}
				}
			}
			// go around again, looking for timing modes for this GDevice
			spBlock.spTBMask = 2;		// ignore DrvrSW
			spBlock.spParamData =  (1<<fall) + (1<<foneslot) + (1<<fnext);	// next resource, this slot, whether enabled or disabled
			errorEndOfTimings = SGetTypeSRsrc(&spBlock);	// and get the next timing mode
		} while ( noErr == errorEndOfTimings );	// until the end of this GDevice
	}

}

pascal void ModeListIterator(void *userData, DMListIndexType, DMDisplayModeListEntryPtr displaymodeInfo)
{
	unsigned long			depthCount;
	short					iCount;
	ListIteratorDataRec		*myIterateData		= (ListIteratorDataRec*) userData;
	DepthInfo				*myDepthInfo;
	
	// set user data in a round about way
	myIterateData->displayModeTimingInfo		= *displaymodeInfo->displayModeTimingInfo;
	
	// now get the DMDepthInfo info into memory we own
	depthCount = displaymodeInfo->displayModeDepthBlockInfo->depthBlockCount;
	myDepthInfo = (DepthInfo*)NewPtrClear(depthCount * sizeof(DepthInfo));

	// set the info for the caller
	myIterateData->depthBlockCount = depthCount;
	myIterateData->depthBlocks = myDepthInfo;

	// and fill out all the entries
	if (depthCount) for (iCount=0; iCount < depthCount; iCount++)
	{
		myDepthInfo[iCount].depthSwitchInfo = 
			*displaymodeInfo->displayModeDepthBlockInfo->depthVPBlock[iCount].depthSwitchInfo;
		myDepthInfo[iCount].depthVPBlock = 
			*displaymodeInfo->displayModeDepthBlockInfo->depthVPBlock[iCount].depthVPBlock;
	}
}

void GetRequestTheDM2Way (	VideoRequestRecPtr requestRecPtr,
							GDHandle walkDevice,
							DMDisplayModeListIteratorUPP myModeIteratorProc,
							DMListIndexType theDisplayModeCount,
							DMListType *theDisplayModeList)
{
	short					jCount;
	short					kCount;
	ListIteratorDataRec		searchData;

	searchData.depthBlocks = nil;
	// get the mode lists for this GDevice
	for (jCount=0; jCount<theDisplayModeCount; jCount++)		// get info on all the resolution timings
	{
		DMGetIndexedDisplayModeFromList(*theDisplayModeList, jCount, 0, myModeIteratorProc, &searchData);
		
		// for all the depths for this resolution timing (mode)...
		if (searchData.depthBlockCount) for (kCount = 0; kCount < searchData.depthBlockCount; kCount++)
		{
			// only if the mode is valid and is safe or we override it with the kAllValidModesBit request flag
			if	(	searchData.displayModeTimingInfo.csTimingFlags & 1<<kModeValid && 
					(	searchData.displayModeTimingInfo.csTimingFlags & 1<<kModeSafe ||
						requestRecPtr->requestFlags & 1<<kAllValidModesBit
					)
				)
			{
				if (FindBestMatch (	requestRecPtr,
									searchData.depthBlocks[kCount].depthVPBlock.vpPixelSize,
									searchData.depthBlocks[kCount].depthVPBlock.vpBounds.right,
									searchData.depthBlocks[kCount].depthVPBlock.vpBounds.bottom))
				{
					requestRecPtr->screenDevice = walkDevice;
					requestRecPtr->availBitDepth = searchData.depthBlocks[kCount].depthVPBlock.vpPixelSize;
					requestRecPtr->availHorizontal = searchData.depthBlocks[kCount].depthVPBlock.vpBounds.right;
					requestRecPtr->availVertical = searchData.depthBlocks[kCount].depthVPBlock.vpBounds.bottom;
					
					// now set the important info for DM to set the display
					requestRecPtr->depthMode = searchData.depthBlocks[kCount].depthSwitchInfo.csMode;
					requestRecPtr->displayMode = searchData.depthBlocks[kCount].depthSwitchInfo.csData;
					requestRecPtr->switchInfo = searchData.depthBlocks[kCount].depthSwitchInfo;
					if (searchData.displayModeTimingInfo.csTimingFlags & 1<<kModeSafe)
						requestRecPtr->availFlags = 0;							// mode safe
					else requestRecPtr->availFlags = 1<<kModeValidNotSafeBit;	// mode valid but not safe, requires user validation of mode switch
	
				}
			}

		}
	
		if (searchData.depthBlocks)
		{
			DisposePtr ((Ptr)searchData.depthBlocks);	// toss for this timing mode of this gdevice
			searchData.depthBlocks = nil;				// init it just so we know
		}
	}
}

Boolean FindBestMatch (VideoRequestRecPtr requestRecPtr, short bitDepth, unsigned long horizontal, unsigned long vertical)
{
	// ¥¥ do the big comparison ¥¥
	// first time only if	(no mode yet) and
	//						(bounds are greater/equal or kMaximizeRes not set) and
	//						(depth is less/equal or kShallowDepth not set) and
	//						(request match or kAbsoluteRequest not set)
	if	(	nil == requestRecPtr->displayMode
			&&
			(	(horizontal >= requestRecPtr->reqHorizontal &&
				vertical >= requestRecPtr->reqVertical)
				||														
				!(requestRecPtr->requestFlags & 1<<kMaximizeResBit)	
			)
			&&
			(	bitDepth <= requestRecPtr->reqBitDepth ||	
				!(requestRecPtr->requestFlags & 1<<kShallowDepthBit)		
			)
			&&
			(	(horizontal == requestRecPtr->reqHorizontal &&	
				vertical == requestRecPtr->reqVertical &&
				bitDepth == requestRecPtr->reqBitDepth)
				||
				!(requestRecPtr->requestFlags & 1<<kAbsoluteRequestBit)	
			)
		)
		{
			// go ahead and set the new values
			return (true);
		}
	else	// can we do better than last time?
	{
		// if	(kBitDepthPriority set and avail not equal req) and
		//		((depth is greater avail and depth is less/equal req) or kShallowDepth not set) and
		//		(avail depth less reqested and new greater avail)
		//		(request match or kAbsoluteRequest not set)
		if	(	(	requestRecPtr->requestFlags & 1<<kBitDepthPriorityBit && 
					requestRecPtr->availBitDepth != requestRecPtr->reqBitDepth
				)
				&&
				(	(	bitDepth > requestRecPtr->availBitDepth &&
						bitDepth <= requestRecPtr->reqBitDepth
					)
					||
					!(requestRecPtr->requestFlags & 1<<kShallowDepthBit)	
				)
				&&
				(	requestRecPtr->availBitDepth < requestRecPtr->reqBitDepth &&
					bitDepth > requestRecPtr->availBitDepth	
				)
				&&
				(	(horizontal == requestRecPtr->reqHorizontal &&	
					vertical == requestRecPtr->reqVertical &&
					bitDepth == requestRecPtr->reqBitDepth)
					||
					!(requestRecPtr->requestFlags & 1<<kAbsoluteRequestBit)	
				)
			)
		{
			// go ahead and set the new values
			return (true);
		}
		else
		{
			// match resolution: minimize Æh & Æv
			if	(	abs( (long) (requestRecPtr->reqHorizontal - horizontal) ) <=
					abs( (long) (requestRecPtr->reqHorizontal - requestRecPtr->availHorizontal)) &&
					abs( (long) (requestRecPtr->reqVertical - vertical)) <=
					abs( (long) (requestRecPtr->reqVertical - requestRecPtr->availVertical))
				)
			{
				// now we have a smaller or equal delta
				//	if (h or v greater/equal to request or kMaximizeRes not set) 
				if (	(horizontal >= requestRecPtr->reqHorizontal &&
						vertical >= requestRecPtr->reqVertical)
						||
						!(requestRecPtr->requestFlags & 1<<kMaximizeResBit)
					)
				{
					// if	(depth is equal or kBitDepthPriority not set) and
					//		(depth is less/equal or kShallowDepth not set) and
					//		([h or v not equal] or [avail depth less reqested and new greater avail] or depth equal avail) and
					//		(request match or kAbsoluteRequest not set)
					if	(	(	requestRecPtr->availBitDepth == bitDepth ||			
								!(requestRecPtr->requestFlags & 1<<kBitDepthPriorityBit)
							)
							&&
							(	bitDepth <= requestRecPtr->reqBitDepth ||	
								!(requestRecPtr->requestFlags & 1<<kShallowDepthBit)		
							)
							&&
							(	(requestRecPtr->availHorizontal != horizontal ||
								requestRecPtr->availVertical != vertical)
								||
								(requestRecPtr->availBitDepth < requestRecPtr->reqBitDepth &&
								bitDepth > requestRecPtr->availBitDepth)
								||
								(bitDepth == requestRecPtr->reqBitDepth)
							)
							&&
							(	(horizontal == requestRecPtr->reqHorizontal &&	
								vertical == requestRecPtr->reqVertical &&
								bitDepth == requestRecPtr->reqBitDepth)
								||
								!(requestRecPtr->requestFlags & 1<<kAbsoluteRequestBit)	
							)
						)
					{
						// go ahead and set the new values
						return (true);
					}
				}
			}
		}
	}
	return (false);
}

void GravitateMonitors (void)
{
	// do the magic gravitation here
}