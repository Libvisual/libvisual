#include <config.h>

#include <libvisual/libvisual.h>
#include <stdio.h>
#include <gettext.h>
#include <limits>

#include "G-Force_Proj.h"
#include "G-Force.h"
#include "XFloatList.h"

#ifdef UNIX_X

#define __defaultTTFormat 	""
#define __defaultFont 		""

#define __drawText( x, y, str )		fprintf(stderr,str->getCStr());


#include "RectUtils.h"
#define __setupPort

#define __restorePort

#endif


#include "CEgFileSpec.h"
#include "EgOSUtils.h"
#include <math.h>
#include <stdlib.h>
#include "ConfigFile.h"
#include "Expression.h"
#include "Hashtable.h"
#include "ParticleGroup.h"

#define VAL(a,b,c,d) (((a)<<24)+((b)<<16)+((c)<<8)+(d))
#define VAL2(a,b)    (((a)<<8)+(b))

GForce::GForce( void* inRefCon ) :
#if defined(UNIX_X)
	mPrefs( ".G-Force", true ),
#endif
	mConsoleLines( cDuplicatesAllowed, cOrderImportant ),
	mLineExpireTimes( cOrderImportant ),
	mT( 0 ),
	mPal1( mT, mIntensityParam ),
	mPal2( mT, mIntensityParam ),
	mDeltaFields	( cNoDuplicates_CaseInsensitive, cSortLowToHigh ),
	mColorMaps		( cNoDuplicates_CaseInsensitive, cSortLowToHigh ),
	mWaveShapes		( cNoDuplicates_CaseInsensitive, cSortLowToHigh ),
	mParticles		( cNoDuplicates_CaseInsensitive, cSortLowToHigh ),
	mWave1( mT ),
	mWave2( mT )
{
	// Do initting...
	mWind				= 0;
	mOutPort			= 0;
	mRefCon				= inRefCon;
	mFrameCount			= 0;
	mT_MS_Base			= EgOSUtils::CurTimeMS();
	mConsoleExpireTime	=
	mLastCursorUpdate	=
	mLastGetKeys		=
	mT_MS				= 0;
	mFrameCountStart	=
	mNextPaletteUpdate		= 0;
	mNextShapeChange	= mT + 10;
	mNextFieldChange	= mT + 10;
	mNextColorChange	= mT + 10;
	mLastSongStart		= mT - 10000;
	mLastKeyPollTime	= mT;
	mLastActiveTime		= mT;
	mDoingSetPortWin	= false;
	mNeedsPaneErased	= true;
	mShapeSlideShow		=
	mColorSlideShow		=
	mFieldSlideShow		= true;
	mAtFullScreen		= false;
	mMouseWillAwaken	= false;
	mTrackTextDur		= 0;

	mPrefs.Load();
	if ( mPrefs.GetPref( VAL('V','e','r','s') ) != GFORCE_COMPAT_VERSION ) {
		mConsoleDelay		= 8;
		mConsoleLineDur		= 14;
		mMagScale 			= 1;
		mTransitionLo		= 4;
		mTransitionHi		= 18;
		mScrnSaverDelay		= -1 * 60;			// Factory: screen saver mode disabled
		mPrefs.SetPref( VAL('V','e','r','s'), GFORCE_COMPAT_VERSION );
		mBorderlessWind		= 0;
		mHandleKeys			= 1;
		mNum_S_Steps 		= 200;
		mFullscreenSize.h	= 640;
		mFullscreenSize.v	= 480;
		mFullscreenDepth	= 8;
		mFullscreenDevice	= 0;
		mMaxSize.h			= 30000;
		mMaxSize.v			= 360;
		mTrackTextPosMode	= 5;
		mTrackTextSize		= 18;
		mNormalizeInput		= false;
		mNewConfigNotify	= false;
		mParticlesOn		= true;
		mKeyMap					.Assign( "TLRY`SNGFZXCQWE,.M[]{}P******!@#$%^&*()1234567890" );
		mFieldIntervalStr		.Assign( "18 + rnd( 15 )" );
		mColorIntervalStr		.Assign( "10 + rnd( 15 )" );
		mShapeIntervalStr		.Assign( "10 + rnd( 15 )" );
		mTrackFont				.Assign( __defaultFont );
		mTrackTextStartStr		.Assign( "4 + LAST_SONG_START - t" );
		mTrackTextDurationStr	.Assign( "5" );
		mTrackMetaText			.Assign( __defaultTTFormat );
		mParticleDuration		.Assign( "8 + rnd( 15 )" );
		mParticleProbability	.Assign( ".09/((NUM_PARTICLES+1)^1.66)" );


		// Show the welcome msg for a pref rewrite...
		Println( GFORCE_VERS_STR );
		Println( "Press '?' for help" );
	}
	else {
		mConsoleDelay       = mPrefs.GetPref( VAL('C','D','u','r') );
		mConsoleLineDur     = mPrefs.GetPref( VAL('C','L','i','n') );
		mMagScale           = mPrefs.GetPref( VAL('M','S','c','l') ) / 1000.0;
		mTransitionLo       = mPrefs.GetPref( VAL('T','r','L','o') );
		mTransitionHi       = mPrefs.GetPref( VAL('T','r','H','i') );
		mScrnSaverDelay     = mPrefs.GetPref( VAL('S','S','v','r') ) * 60.0;
		mBorderlessWind     = mPrefs.GetPref( VAL('N','o','B','o') );
		mHandleKeys         = mPrefs.GetPref( VAL('K','y','b','d') );
		mNum_S_Steps        = mPrefs.GetPref( VAL('S','t','p','s') );
		mFullscreenSize.h   = mPrefs.GetPref( VAL('F','S','_','X') );
		mFullscreenSize.v   = mPrefs.GetPref( VAL('F','S','_','Y') );
		mFullscreenDepth    = mPrefs.GetPref( VAL('F','S','_','D') );
		mFullscreenDevice   = mPrefs.GetPref( VAL('F','S','_','#') );
		mParticlesOn        = mPrefs.GetPref( VAL('P','_','O','n') );
		mNormalizeInput     = mPrefs.GetPref( VAL('N','o','r','m') );
		mMaxSize.v          = mPrefs.GetPref( VAL('M','a','x','Y') );
		mMaxSize.h          = mPrefs.GetPref( VAL('M','a','x','X') );
		mNewConfigNotify    = mPrefs.GetPref( VAL('S','h','w','T') );
		mTrackTextPosMode   = mPrefs.GetPref( VAL('T','P','o','s') );
		mTrackTextSize      = mPrefs.GetPref( VAL('T','S','z','e') );
		mPrefs.GetPref( VAL('W','I','n','t'), mShapeIntervalStr );
		mPrefs.GetPref( VAL('D','I','n','t'), mFieldIntervalStr );
		mPrefs.GetPref( VAL('C','I','n','t'), mColorIntervalStr );
		mPrefs.GetPref( VAL2('T','?'), mTrackTextStartStr );
		mPrefs.GetPref( VAL('T','D','u','r'), mTrackTextDurationStr );
		mPrefs.GetPref( VAL('T','F','n','t'), mTrackFont );
		mPrefs.GetPref( VAL('T','S','t','r'), mTrackMetaText );
		mPrefs.GetPref( VAL('P','D','u','r'), mParticleDuration );
		mPrefs.GetPref( VAL('P','P','r','b'), mParticleProbability );
		mPrefs.GetPref( VAL('K','M','a','p'), mKeyMap );
	}

	mPortA.SetTrackTextFont( mTrackFont, mTrackTextSize );
	mPortB.SetTrackTextFont( mTrackFont, mTrackTextSize );

	// Catch any bad values for mNumSampleBins
	if ( mNum_S_Steps < 1 || mNum_S_Steps > 10000 )
		mNum_S_Steps = 320;


	mNum_FFT_Steps = 256;

	// Alloc/setup the data we'll have our virtual machines accessing...
	SetNumSampleBins( mNum_S_Steps );
	SetNumFFTBins( mNum_FFT_Steps );

	// Setup waveshape members
	mWave1.SetMagFcn( (ExprUserFcn**) &mSampleFcn );
	mWave2.SetMagFcn( (ExprUserFcn**) &mSampleFcn );

	// FIXME, use FFT data here
	mWave1.SetFFTFcn( (ExprUserFcn**) &mFFTFcn );
	mWave2.SetFFTFcn( (ExprUserFcn**) &mFFTFcn );

	// Init particle stuff
	mDict.AddVar( "T", &mT );
	mDict.AddVar( "LAST_PARTICLE_START", &mLastParticleStart );
	mDict.AddVar( "NUM_PARTICLES", &mNumRunningParticles );
	mNumRunningParticles = 0;
	mNextParticleCheck = mT + 1;
	mParticleProbabilityFcn.Compile( mParticleProbability, mDict );
	mParticleDurationFcn.Compile( mParticleDuration, mDict );
	mShapeInterval.Compile( mShapeIntervalStr, mDict );
	mColorInterval.Compile( mColorIntervalStr, mDict );
	mFieldInterval.Compile( mFieldIntervalStr, mDict );

	// Track Text stuff
	mDict.AddVar( "LAST_SONG_START", &mLastSongStart );
	mTrackTextStartFcn.Compile( mTrackTextStartStr, mDict );
	mTrackTextDurFcn.Compile( mTrackTextDurationStr, mDict );

	// Transition bookkeeping
	mColorTransTime		= -1;
	mShapeTransTime		= -1;
	mGF_Palette			= 0;
	mWave				= 0;


	// Look in G-Force's support folders and see what we have to select from...
	BuildConfigLists();

	mField		= &mField1;
	mNextField	= &mField2;

	for ( int i = 0; i < 4; i++ )
		mCurKeys[ i ] = 0;
}




GForce::~GForce() {

	// Rewrite the prefs to disk...
	mPrefs.SetPref( VAL('S','S','v','r'), mScrnSaverDelay / 60.0 );
	mPrefs.SetPref( VAL('T','r','H','i'), mTransitionHi );
	mPrefs.SetPref( VAL('T','r','L','o'), mTransitionLo );
	mPrefs.SetPref( VAL('M','S','c','l'), mMagScale * 1000 );
	mPrefs.SetPref( VAL('C','I','n','t'), mColorIntervalStr );
	mPrefs.SetPref( VAL('W','I','n','t'), mShapeIntervalStr );
	mPrefs.SetPref( VAL('D','I','n','t'), mFieldIntervalStr );
	mPrefs.SetPref( VAL('S','t','p','s'), mNum_S_Steps );
	mPrefs.SetPref( VAL('P','D','u','r'), mParticleDuration );
	mPrefs.SetPref( VAL('P','P','r','b'), mParticleProbability );
	mPrefs.SetPref( VAL('N','o','B','o'), mBorderlessWind );
	mPrefs.SetPref( VAL('K','y','b','d'), mHandleKeys	);
	mPrefs.SetPref( VAL('F','S','_','X'), mFullscreenSize.h );
	mPrefs.SetPref( VAL('F','S','_','Y'), mFullscreenSize.v );
	mPrefs.SetPref( VAL('F','S','_','D'), mFullscreenDepth );
	mPrefs.SetPref( VAL('F','S','_','#'), mFullscreenDevice );
	mPrefs.SetPref( VAL('N','o','r','m'), mNormalizeInput );
	mPrefs.SetPref( VAL('M','a','x','Y'), mMaxSize.v );
	mPrefs.SetPref( VAL('M','a','x','X'), mMaxSize.h );
	mPrefs.SetPref( VAL('T','P','o','s'), mTrackTextPosMode );
	mPrefs.SetPref( VAL('T','S','z','e'), mTrackTextSize );
	mPrefs.SetPref( VAL('T','F','n','t'), mTrackFont );
	mPrefs.SetPref( VAL('T','S','t','r'), mTrackMetaText );
	mPrefs.SetPref( VAL2('T','?'), mTrackTextStartStr );
	mPrefs.SetPref( VAL('T','D','u','r'), mTrackTextDurationStr );
	mPrefs.SetPref( VAL('K','M','a','p'), mKeyMap );
	mPrefs.SetPref( VAL('P','_','O','n'), mParticlesOn ? 1 : 0 );
	mPrefs.SetPref( VAL('S','h','w','T'), mNewConfigNotify ? 1 : 0 );
	mPrefs.SetPref( VAL('C','D','u','r'), mConsoleDelay );
	mPrefs.SetPref( VAL('C','L','i','n'), mConsoleLineDur );

	// Init the track text info
	NewSong();

	mPrefs.Store();

}





void GForce::SetNumSampleBins( long inNumBins ) {
	float k;

	if ( inNumBins > 0 && inNumBins < 10000 ) {
		mSampleFcn = (ExprUserFcn*) mSamplesBuf.Dim( sizeof( float ) * inNumBins + sizeof( ExprUserFcn ) + 32 );
		mNum_S_Steps = inNumBins;
		mSampleFcn -> mNumFcnBins = inNumBins;

		// A fast lookup table for a sine wave
		mSine = (float*) mSineBuf.Dim( sizeof( float ) * inNumBins );
		k =  6.2831853071795 / ( (float) inNumBins );

		for ( int i = 0; i < inNumBins; i++ ) {
			mSampleFcn -> mFcn[ i ] = 0;
			mSine[ i ] = sin(  k * ( (float) i ) );
		}
	}
}





void GForce::SetNumFFTBins( long inNumBins ) {

	if ( inNumBins > 0 && inNumBins < 1000 ) {
		mFFTFcn = (ExprUserFcn*) mFFTBuf.Dim( sizeof( float ) * inNumBins + sizeof( ExprUserFcn ) + 32);
		mNum_FFT_Steps = inNumBins;
		mFFTFcn -> mNumFcnBins = inNumBins;
	}
}



#define __setChar( n, ID )		s.setChar( n, mKeyMap.getChar( ID ) )


void GForce::ShowHelp() {
	UtilStr s;

	s.Assign( "X     - Display track title" );			__setChar( 1, cDispTrackTitle );	Println( &s );
	s.Assign( "X     - List configs" );					__setChar( 1, cGetConfigInfo );		Println( &s );
	s.Assign( "X     - Frame Rate" );					__setChar( 1, cFrameRate );			Println( &s );
	s.Assign( "X     - Particles on/off" );				__setChar( 1, cToggleParticles );	Println( &s );
	s.Assign( "X     - Spawn new particle" );			__setChar( 1, cSpawnNewParticle );	Println( &s );
	s.Assign( "X     - Fullscreen on/off" );			__setChar( 1, cToggleFullsceen );	Println( &s );
	s.Assign( "X     - Show config titles on/off" );	__setChar( 1, cToggleConfigName );	Println( &s );
	s.Assign( "X     - Normalize input on/off" );		__setChar( 1, cToggleNormalize );	Println( &s );

	s.Assign( "X X   - Freeze/Continue slideshow" );		__setChar( 1, cStopSlideshowAll );	__setChar( 3, cStartSlideshowAll );		Println( &s );
	s.Assign( "X X   - +/- sound amplitude (See Extras docs)" );		__setChar( 1, cDecMagScale );		__setChar( 3, cIncMagScale );		Println( &s );
	s.Assign( "X X   - +/- num of bins (Stps) (See Extras docs)" );		__setChar( 1, cDecNumSSteps );		__setChar( 3, cIncNumSSteps );		Println( &s );
	s.Assign( "X X X - Prev/Next/Hold DeltaField" );		__setChar( 1, cPrevDeltaField );	__setChar( 3, cNextDeltaField );		__setChar( 5, cToggleFieldShow );		Println( &s );
	s.Assign( "X X X - Prev/Next/Hold ColorMap" );			__setChar( 1, cPrevColorMap );		__setChar( 3, cNextColorMap );			__setChar( 5, cToggleColorShow );		Println( &s );
	s.Assign( "X X X - Prev/Next/Hold WaveShape" );			__setChar( 1, cPrevWaveShape );		__setChar( 3, cNextWaveShape );			__setChar( 5, cToggleShapeShow );		Println( &s );

	Println( "" );
	Println( "Press SHIFT and a number to store the current ColorMap, Waveshape, and" );
	Println( "   DeltaField, and press just the number to recall them." );

	// Give the user more time than usual since this is help info
	mConsoleExpireTime += 8000;
}

bool GForce::HandleKey( long inChar ) {
	bool handled = true;
	int n;

	// See if this keystroke is to be ignored
	if ( ! mHandleKeys )
		return false;


	if ( inChar >= 'a' && inChar <= 'z' )
		inChar = 'A' + ( inChar - 'a' );

	if ( inChar == '/' || inChar == '?' )
		ShowHelp();
	else if ( inChar >= ' ' && inChar < 129 ) {

		inChar = mKeyMap.FindNextInstanceOf( 0, inChar );

		switch ( inChar ) {

		case cDispTrackTitle:
			StartTrackText();
			break;

		case cGetConfigInfo:
		  {
			Print(_("WaveShape:  "));
			Println( &mWaveShapeName );
			Print(_("ColorMap:   "));
			Println( &mColorMapName );
			Print(_("DeltaField: "));
			Println( mField -> GetName() );
			ParticleGroup* particle = (ParticleGroup*) mRunningParticlePool.GetHead();
			if ( particle ) {
				Print(_("Particles:  "));

				while ( particle ) {
					Print( &particle -> mTitle );
					particle = (ParticleGroup*) particle -> GetNext();
					if ( particle )
						Print( ", " );
				}
				Println( "" );
			}
			break;
		  }
		case cFrameRate:
			mTemp.SetFloatValue( ( (float) mCurFrameRate ) / 10.0 );
			mTemp.Append(_(" frames/sec"));
			Println( &mTemp );
			break;

		case cDecMagScale:
			mMagScale /= 1.2;
			mTemp.SetFloatValue( mMagScale );
			Print(_("Amplitude scale: "));
			Println( &mTemp );
			break;

		case cIncMagScale:
			mMagScale *= 1.2;
			mTemp.SetFloatValue( mMagScale );
			Print(_("Amplitude scale: "));
			Println( &mTemp );
			break;

		case cToggleParticles:
			mParticlesOn = ! mParticlesOn;
			if ( mParticlesOn )
				Println(_("Particles ON"));
			else
				Println(_("Particles OFF"));
			break;

		case cSpawnNewParticle:
			SpawnNewParticle();
			break;

		case cDecNumSSteps:
		case cIncNumSSteps:
			if ( inChar == cDecNumSSteps )
				n = - 4;
			else
				n = + 4;
			SetNumSampleBins( mNum_S_Steps + n );
			mTemp.Assign(_("Number s steps: "));
			mTemp.Append( mNum_S_Steps );
			Println( &mTemp );
			break;

		case cToggleConfigName:
			mNewConfigNotify = ! mNewConfigNotify;
			if ( mNewConfigNotify )
				Println(_("Show names ON"));
			else
				Println(_("Show names OFF"));
			break;

		case cToggleNormalize:
			mNormalizeInput = ! mNormalizeInput;
			if ( mNormalizeInput )
				Println(_("Normalize ON"));
			else
				Println(_("Normalize OFF"));
			break;

		case cPrevDeltaField:
		case cNextDeltaField:
			n = mFieldPlayList.FindIndexOf( mCurFieldNum );
			if ( inChar == cPrevDeltaField )
				n = n + mFieldPlayList.Count() - 2;

			loadDeltaField( mFieldPlayList.Fetch( 1 + n % mFieldPlayList.Count() ) );

			// If the pref says so, display that we're loading a new config
			if ( mNewConfigNotify ) {
				Print(_("Loading DeltaField: "));
				Println( mField -> GetName() );
			}

			// Turn field slide show off when we change deltafields manually
			if ( ! mFieldSlideShow )
				break;
		case cToggleFieldShow:
			mFieldSlideShow = ! mFieldSlideShow;
			mNextFieldChange = mT;
			if ( mFieldSlideShow ) {
				Println(_("DeltaField slideshow ON"));
				mFieldPlayList.Randomize(); }
			else
				Println(_("DeltaField slideshow OFF"));
			break;


		case cStartSlideshowAll:
			mFieldSlideShow = true;		mNextFieldChange = mT;
			mColorSlideShow = true;		mNextColorChange = mT;
			mShapeSlideShow = true;		mNextShapeChange = mT;
			Println(_("All slideshows ON"));
			break;

		case cStopSlideshowAll:
			mFieldSlideShow = false;
			mColorSlideShow = false;
			mShapeSlideShow = false;
			Println(_("All slideshows OFF"));
			break;


		case cPrevColorMap:
		case cNextColorMap:
			n = mColorPlayList.FindIndexOf( mCurColorMapNum );
			if ( inChar == cPrevColorMap )
				n = n + mColorPlayList.Count() - 2;

			loadColorMap( mColorPlayList.Fetch( 1 + n % mColorPlayList.Count() ), false );

			// Turn slide show off when we change colormaps manually
			if ( ! mColorSlideShow )
				break;
		case cToggleColorShow:
			mColorSlideShow = ! mColorSlideShow;
			mNextColorChange = mT;
			if ( mColorSlideShow ) {
				Println(_("ColorMap slideshow ON"));
				mColorPlayList.Randomize(); }
			else
				Println(_("ColorMap slideshow OFF"));
			break;

		case cPrevWaveShape:
		case cNextWaveShape:
			n = mShapePlayList.FindIndexOf( mCurShapeNum );
			if ( inChar == cPrevWaveShape )
				n = n + mShapePlayList.Count() - 2;

			loadWaveShape( mShapePlayList.Fetch( 1 + n % mShapePlayList.Count() ), false );

			// Turn slide show off when we change shapes manually
			if ( ! mShapeSlideShow )
				break;
		case cToggleShapeShow:
			mShapeSlideShow = ! mShapeSlideShow;
			mNextShapeChange = mT;
			if ( mShapeSlideShow ) {
				Println(_("WaveShape slideshow ON"));
				mShapePlayList.Randomize(); }
			else
				Println(_("WaveShape slideshow OFF"));
			break;

		case cSetPreset0:	StoreConfigState( VAL('S','E','T','0') );	break;
		case cSetPreset1:	StoreConfigState( VAL('S','E','T','1') );	break;
		case cSetPreset2:	StoreConfigState( VAL('S','E','T','2') );	break;
		case cSetPreset3:	StoreConfigState( VAL('S','E','T','3') );	break;
		case cSetPreset4:	StoreConfigState( VAL('S','E','T','4') );	break;
		case cSetPreset5:	StoreConfigState( VAL('S','E','T','5') );	break;
		case cSetPreset6:	StoreConfigState( VAL('S','E','T','6') );	break;
		case cSetPreset7:	StoreConfigState( VAL('S','E','T','7') );	break;
		case cSetPreset8:	StoreConfigState( VAL('S','E','T','8') );	break;
		case cSetPreset9:	StoreConfigState( VAL('S','E','T','9') );	break;


		case cPreset0:	handled = RestoreConfigState( VAL('S','E','T','0') );	break;
		case cPreset1:	handled = RestoreConfigState( VAL('S','E','T','1') );	break;
		case cPreset2:	handled = RestoreConfigState( VAL('S','E','T','2') );	break;
		case cPreset3:	handled = RestoreConfigState( VAL('S','E','T','3') );	break;
		case cPreset4:	handled = RestoreConfigState( VAL('S','E','T','4') );	break;
		case cPreset5:	handled = RestoreConfigState( VAL('S','E','T','5') );	break;
		case cPreset6:	handled = RestoreConfigState( VAL('S','E','T','6') );	break;
		case cPreset7:	handled = RestoreConfigState( VAL('S','E','T','7') );	break;
		case cPreset8:	handled = RestoreConfigState( VAL('S','E','T','8') );	break;
		case cPreset9:	handled = RestoreConfigState( VAL('S','E','T','9') );	break;

		default:
			handled = false;
		}	}
	else
		handled = false;

	return handled;
}



void GForce::StoreConfigState( long inParamName ) {
	UtilStr str;

	str.Assign( mWaveShapeName );		str.Append( ',' );
	str.Append( mColorMapName );		str.Append( ',' );
	str.Append( mField -> GetName() );	str.Append( ',' );

	mPrefs.SetPref( inParamName, str );
	Println(_("State stored."));
}


bool GForce::RestoreConfigState( long inParamName ) {
	UtilStr str, configName;
	long pos, n, found = false;

	if ( mPrefs.GetPref( inParamName, str ) ) {

		// Parse the waveshape config name
		pos = str.FindNextInstanceOf( 0, ',' );
		configName.Assign( str.getCStr(), pos - 1 );
		n = mWaveShapes.FetchBestMatch( configName );
		loadWaveShape( n, false );
		mShapeSlideShow = false;

		// Parse the colormap config name
		str.Trunc( pos, false );
		pos = str.FindNextInstanceOf( 0, ',' );
		configName.Assign( str.getCStr(), pos - 1 );
		n = mColorMaps.FetchBestMatch( configName );
		loadColorMap( n, false );
		mColorSlideShow = false;

		// Parse the colormap config name
		str.Trunc( pos, false );
		n = mDeltaFields.FetchBestMatch( str );
		loadDeltaField( n );
		mFieldSlideShow = false;

		found = true;
	}

	return found;
}



void GForce::ManageColorChanges() {
	int i;

	// If in a ColorMap transition/morph
	if ( mColorTransTime > 0 ) {

		// If we've the ColorMap transition is over, end it
		if ( mT_MS > mColorTransEnd ) {
			GF_Palette* temp = mGF_Palette;
			mGF_Palette = mNextPal;
			mNextPal = temp;
			mColorTransTime = -1;
			mNextColorChange = mT + mColorInterval.Evaluate();
		}
	}

	// Time for a color map change?
	else if ( mT > mNextColorChange && mColorSlideShow ) {

		// Load the next config in the (randomized) config list...
		i = mColorPlayList.FindIndexOf( mCurColorMapNum );

		// Make a new play list if we've reached the end of the list...
		if ( i >= mColorPlayList.Count() ) {
			mColorPlayList.Randomize();
			i = 0;
		}
		loadColorMap( mColorPlayList.Fetch( i + 1 ), true );
	}

	// Update the screen palette if it's time
	if ( mT > mNextPaletteUpdate ) {

		// If in a ColorMap transition/morph then we must set mColorTrans, for it's linked into mGF_Palette
		if ( mColorTransTime > 0 ) {
			float t = (float) ( mColorTransEnd - mT_MS ) / ( (float) mColorTransTime );
			mColorTrans = pow( t, TRANSITION_ALPHA );
		}

		// Evaluate the palette at this time
		mGF_Palette -> Evaluate( mPalette );

		// Set our offscreen ports to the right palette...
		mPortA.SetPalette( mPalette );
		mPortB.SetPalette( mPalette );

		// If we're at fullsceen, the screen device may need the current palette too
		if ( mAtFullScreen && mFullscreenDepth == 8 ) {
			mScreen.SetPalette( mPalette );
			mPortA.PreventActivate( mOutPort );
			mPortB.PreventActivate( mOutPort );
		}

		// Reevaluate the palette a short time from now
		mNextPaletteUpdate = mT + .1;
	}
}



void GForce::ManageShapeChanges() {
	int i;

	// If in a WaveShape transition/morph
	if ( mShapeTransTime > 0 ) {

		// If we've the ColorMap transition is over, end it
		if ( mT_MS > mShapeTransEnd ) {
			WaveShape* temp = mWave;
			mWave = mNextWave;
			mNextWave = temp;
			mShapeTransTime = -1;
			mNextShapeChange = mT + mShapeInterval.Evaluate();
		} }

	// Time for a wave shape change?
	else if ( mT > mNextShapeChange && mShapeSlideShow ) {

		// Load the next config in the (randomized) config list...
		i = mShapePlayList.FindIndexOf( mCurShapeNum );

		// Make a new play list if we've reached the end of the list...
		if ( i >= mShapePlayList.Count() ) {
			mShapePlayList.Randomize();
			i = 0;
		}
		loadWaveShape( mShapePlayList.Fetch( i + 1 ), true );
	}

}


void GForce::ManageFieldChanges() {
	long i;

	// If we have have a delta field in mid-calculation, chip away at it...
	if ( ! mNextField -> IsCalculated() )
		mNextField -> CalcSome();

	if ( mT > mNextFieldChange && mNextField -> IsCalculated() && mFieldSlideShow ) {

		// Load the next field in the (randomized) field list...
		i = mFieldPlayList.FindIndexOf( mCurFieldNum );

		// Make a new play list if we've reached the end of the list...
		if ( i >= mFieldPlayList.Count() ) {
			mFieldPlayList.Randomize();
			i = 0;
		}

		// loadGradField() will initiate computation on mField with a new grad field...
		loadDeltaField( mFieldPlayList.Fetch( i + 1 ) );
		DeltaField* temp = mField;
		mField = mNextField;
		mNextField = temp;

		// If the pref says so, display that we're loading a new config
		if ( mNewConfigNotify ) {
			Print( "Loaded DeltaField: " );
			Println( mField -> GetName() );
		}
	}
}



void GForce::ManageParticleChanges() {
	float rndVar;

	if ( mT > mNextParticleCheck && mParticlesOn ) {

		// Generate a random probability value.
		rndVar = ( (float) LV::rand() ) / ( (float) std::numeric_limits<uint32_t>::max() );

		// Comparing that to the evalated probability of a new particle being spawned determines if a new one *should* be spawned
		if ( rndVar < mParticleProbabilityFcn.Evaluate() ) {

			SpawnNewParticle();
		}

		// Check to make a new particle one second from now
		mNextParticleCheck = mT + 1;
	}
}



void GForce::DrawParticles( PixPort& inPort ) {

	// Draw all the particles
	ParticleGroup* particle, *next;
	particle = (ParticleGroup*) mRunningParticlePool.GetHead();
	while ( particle ) {
		next = (ParticleGroup*) particle -> GetNext();

		// When particles stop, move them to a holding/stopped list
		if ( ! particle -> IsExpired() )
			particle -> DrawGroup( inPort );
		else {
			mStoppedParticlePool.addToHead( particle );

			// Update the var that holds how many particles are running (and is accessible in the PPrb expr)
			mNumRunningParticles = mRunningParticlePool.shallowCount();
		}

		particle = next;
	}

}




void GForce::RecordZeroSample( long inCurTime ) {
	int i;

	for ( i = 0; i < mNum_S_Steps; i++ )
		mSampleFcn -> mFcn[ i ] = 0;

	RecordSample( inCurTime );
}



/*
void GForce::RecordSample( long inCurTime, float* inFourier, long inNumBins ) {
	long w, s, n;
	float sample;
	ExprUserFcn* fcn;

	// Now write the sample to memory, adjusted for amplitude...
	fcn = (ExprUserFcn*) mSampleFcn;
	fcn -> mNumFcnBins = mNum_S_Steps;

	for ( s = 0; s < mNum_S_Steps; s++ ) {

		sample = 0;
		for ( w = 0; w < inNumBins; w++ ) {
			n = ( 2.42322211 * w + 1.9 ) * ((float) s) + 1.23231121211 * w;
			sample += inFourier[ w ] * mSine[ n % mNum_S_Steps ];
		}

		fcn -> mFcn[ s ] = sample;
	}

	RecordSample( inCurTime );
}
*/


void GForce::IdleMonitor() {
	bool kybdPress = false;
	float pollDelay;
	float secsUntilSleep = mScrnSaverDelay - ( mT - mLastActiveTime );
	Point pt;

	// Calc time till next kybd poll (Don't waste time checking the kybd unless we've been idle a while)
	if ( IsFullscreen() )
		pollDelay = .6;

	// Don't bother rapildly checking the kybd until we're really close to going into screen saver mode
	else if ( secsUntilSleep < 90 )
		pollDelay = secsUntilSleep / 120.0;
	else
		pollDelay = 10;

	// If it's time to poll for activity...
	if ( mT > mLastKeyPollTime + pollDelay ) {

		mLastKeyPollTime = mT;

		// Check the mouse pos and record it as active if its been moved.
		EgOSUtils::GetMouse( pt );
		if ( pt.h != mLastMousePt.h || pt.v != mLastMousePt.v || kybdPress ) {
			mLastMousePt		= pt;
			mLastActiveTime		= mT;
		}

		// If we're elligible to enter fullscreen then do it
		if ( ! mAtFullScreen && mT - mLastActiveTime > mScrnSaverDelay ) {
			mMouseWillAwaken = true;
		}
	}
}



void GForce::RecordSample( long inCurTime, float* inSound, float inScale, long inNumBins, float* inFFT, float inFFTScale, long inFFTNumBins) {
	float mag, sum;
	int i, n;
	ExprUserFcn* fcn;

	// Only use/process bins we'll actually use
	if ( inNumBins > mNum_S_Steps )
		inNumBins = mNum_S_Steps;



	// Calc a 1/RMS avg value...
	if ( mNormalizeInput ) {

		// Find an RMS amplitude for the sample
		for ( sum = 0.0001, i = 0; i < inNumBins; i++ ) {
			mag = inSound[ i ];
			sum += mag * mag;
		}
		inScale = mMagScale * .009 * ( (float) inNumBins ) / ( sqrt( sum ) ); }
	else
		inScale *= mMagScale;


	// Now write the sample to memory, adjusted for amplitude...
	fcn = (ExprUserFcn*) mSampleFcn;
	fcn -> mNumFcnBins = inNumBins;
	for ( i = 0; i < inNumBins; i++ ) {
		mag = inSound[ i ];
		fcn -> mFcn[ i ] = inSound[ i ] * inScale;
	}

	XFloatList::GaussSmooth( 1.3, inNumBins, fcn -> mFcn );

	// Flatten the ends of the sample...
	n = inNumBins / 20 + 1;
	if ( n <= inNumBins ) {
		for ( i = 0; i < n; i++ ) {
			mag = sin( .5 * 3.1 * i / n );
			fcn -> mFcn[ i ] *= mag;
			fcn -> mFcn[ inNumBins - i - 1 ] *= mag;
		}
	}

	// Now write the FFT data to memory, adjusted for amplitude...
	fcn = (ExprUserFcn*) mFFTFcn;
	fcn -> mNumFcnBins = inFFTNumBins;

	for ( i = 0; i < inFFTNumBins; i++ ) {
		fcn -> mFcn[ i ] = inFFT[ i ] * inFFTScale;
	}


	RecordSample( inCurTime );
}





void GForce::RecordSample( long inCurTime ) {
	long intensity;
	float t;

	if ( &mPortA == mCurPort )
		mCurPort = &mPortB;
	else
		mCurPort = &mPortA;

	// All the waveshape virtual machines are linked to our time index
	mT_MS = inCurTime - mT_MS_Base;
	mT = ( (float) inCurTime ) / 1000.0;

	// Don't bother doing mouse or kybd poll if sceeen saver mode is disabled
	if ( mScrnSaverDelay > 0 )
		IdleMonitor();

	ManageColorChanges();
	ManageShapeChanges();
	ManageFieldChanges();
	ManageParticleChanges();

	// Do the blur operation, a fcn of what's oqn the screen, and the current delta field

	if ( mCurPort == &mPortA )
		mPortB.Fade( mPortA, mField -> GetField() );
	else
		mPortA.Fade( mPortB, mField -> GetField() );

        /* This redraws the image */

	// Draw all the current particles
	DrawParticles( *mCurPort );

	// Draw the current wave shape for the current music sample playing
	// If there's a morph going, drawing is a mix of both waves
	if ( mShapeTransTime > 0 ) {
		float morphPct = (float) ( mShapeTransEnd - mT_MS ) / ( (float) mShapeTransTime );
		mWave -> Draw( mNum_S_Steps, *mCurPort, 1, mNextWave, morphPct ); }
	else
		mWave -> Draw( mNum_S_Steps, *mCurPort, 1, 0, 0 );


	// If we're not currently drawing track text, check to see if we start new text
	if ( mTrackTextDur == 0 && mTrackTextPosMode ) {
		if ( mTrackTextStartFcn.Evaluate() > 0 )
			StartTrackText();
	}

	// If we already have a t.t. draw in progress, draw the text in the (full) foreground color
	if ( mTrackTextDur > 0 ) {

		// From 0 to 1, how far are we into the text display interval?
		t = ( mT - mTrackTextStartTime ) / mTrackTextDur;

		// Decrease the text intensity thru time
		intensity = 255 * ( 1.2 - .3*t );
		if ( intensity > 255 )
			intensity = 255;

		mCurPort -> SetTextColor( mPalette[ intensity ] );
		mCurPort -> SetTrackTextFont();
		mCurPort -> DrawText( mTrackTextPos.h, mTrackTextPos.v, mTrackText );
	}

	// Draw the console text to the offscreen image.  Then copy the image to the OS out port
	if ( mT_MS < mConsoleExpireTime ) {

		// To ensure the console text is readable, we erase it when we're done
		mCurPort -> SetTextMode( SRC_XOR );
		mCurPort -> SetTextColor( mPalette[ 255 ] );
		mCurPort -> SetConsoleFont();
		DrawConsole();
		DrawFrame();
		mCurPort -> SetTextColor( mPalette[ 0 ] );
		DrawConsole();
		mCurPort -> SetTextMode( SRC_OR );  }
	else
		DrawFrame();

	// We need to avoid text all bluring together so we overwrite the foreground text we just drew
	//  with text of a lower intensity...
	if ( mTrackTextDur > 0 ) {

		// Is the text is about to expire? if not, continue drawing.
		if ( t <= 1 ) {

			intensity = 255.5 * pow( t, 1.5 );
			mCurPort -> SetTextColor( mPalette[ intensity ] );
			mCurPort -> SetTrackTextFont();
			mCurPort -> DrawText( mTrackTextPos.h, mTrackTextPos.v, mTrackText ); }
		else {

			// The text's duration is up so turn the draw flag off
			mTrackTextDur = 0;
		}
	}


	// Maintain the frame rate
	mFrameCount++;
	if ( mT_MS - mFrameCountStart >= 1500 ) {
		mCurFrameRate = 10000 * mFrameCount / ( mT_MS - mFrameCountStart );
		mFrameCountStart = mT_MS;
		mFrameCount = 0;
	}

	if ( mT_MS - mLastCursorUpdate > 3000 ) {
		mLastCursorUpdate = mT_MS;
		if ( IsFullscreen() )
			EgOSUtils::HideCursor();
	}
}


void GForce::StartTrackText() {

	if ( mTrackTextPosMode ) {
		CalcTrackTextPos();
		mTrackTextDur = mTrackTextDurFcn.Evaluate();
		mTrackTextStartTime = mT;
	}
}

void GForce::DrawFrame() {


	__setupPort

	// If we're fullscreen, follow the API (the screen may need to do something to finish)
	if ( mScreen.IsFullscreen() )
		mOutPort = mScreen.BeginFrame();

	// Someone may have asked to clear the GF window/pane
	if ( mNeedsPaneErased ) {
		ErasePane();
		mNeedsPaneErased = false;
	}

	// Blt our offscreen world to the output device
	Rect r;
	r.left = r.top = 0;
	r.right = mDispRect.right - mDispRect.left;
	r.bottom = mDispRect.bottom - mDispRect.top;
//	mCurPort -> CopyBits( mOutPort, &r, &mDispRect );
	mCurPort -> CopyBits( mVideoBuffer, &r, &mDispRect );

	// If we're fullscreen, follow the API (the screen may need to do something to finish)
	if ( mScreen.IsFullscreen() )
		mScreen.EndFrame();

	__restorePort
}

void GForce::SetOutVideoBuffer( unsigned char *inVideoBuffer ) {
	mVideoBuffer = inVideoBuffer;
}

#define __loadFolder( folderName, specList, playList )						\
	startOver = true;														\
	folder.AssignFolder( folderName );										\
	while ( EgOSUtils::GetNextFile( folder, spec, startOver, false ) ) {	\
		specList.AddCopy( spec );											\
		startOver = false;													\
	}																		\
	/* Build a 'play' list */												\
	playList.RemoveAll();													\
	for ( i = 1; i <= specList.Count(); i++ ) {								\
		playList.Add( i );													\
	}																		\
	playList.Randomize();


void GForce::BuildConfigLists() {
	CEgFileSpec folder, spec;
	int i;
	bool startOver;

	__loadFolder( DATADIR "/GForceDeltaFields", mDeltaFields, mFieldPlayList )

	__loadFolder( DATADIR "/GForceWaveShapes", mWaveShapes, mShapePlayList )

	__loadFolder( DATADIR "/GForceColorMaps", mColorMaps, mColorPlayList )

	__loadFolder( DATADIR "/GForceParticles", mParticles, mParticlePlayList )
}


// A linear spread is the default field
#define __FIELD_FACTORY		"\
Aspc=0,\
srcX=\"x * .9\",\
srcY=\"y * .9\",\
Vers=100\
"

// A centered circle is the default shape
#define __SHAPE_FACTORY		"\
Stps=-1,\
B0=\"t * 0.0003\",\
Aspc=1,\
C0=\"abs( mag( s ) ) * 0.15 + .3\",\
C1=\"s * 6.28318530 + b0\",\
X0=\"c0 * cos( c1 )\",\
Y0=\"c0 * sin( c1 )\",\
Vers=100\
"

// A single color is the defaut color
#define __COLOR_FACTORY		"\
H=\".9\",\
S=\".8\",\
V=\"i\",\
Vers=100\
"




void GForce::loadColorMap( long inColorMapNum, bool inAllowMorph ) {
	const CEgFileSpec* spec;
	int ok = false, vers;
	ArgList args;

	// Fetch the spec for our config file or folder
	spec = mColorMaps.FetchSpec( inColorMapNum );

	if ( spec ) {
 		mCurColorMapNum = inColorMapNum;

		ok = ConfigFile::Load( spec, args );
		if ( ok ) {
			vers = args.GetArg( VAL('V','e','r','s') );
			ok = vers >= 100 && vers < 110;
			spec -> GetFileName( mColorMapName );
		}
	}


	if ( ! ok ) {
		args.SetArgs( __COLOR_FACTORY );
		mColorMapName.Assign( "<Factory Default>" );
	}

	// If the pref says so, display that we're loading a new config
	if ( mNewConfigNotify ) {
		Print( "Loaded ColorMap: " );
		Println( &mColorMapName );
	}


	// If first time load, don't do any transition/morph, otherwise set up the morph
	if ( mGF_Palette == 0 || ! inAllowMorph ) {
		mGF_Palette = &mPal1;
		mNextPal	= &mPal2;
		mGF_Palette -> Assign( args );
		mColorTransTime = -1;
		mNextColorChange = mT + mColorInterval.Evaluate(); }
	else {
		mNextPal -> Assign( args );
		mGF_Palette -> SetupTransition( *mNextPal, mColorTrans );

		// Calculate how long this transition/morph will be
		mColorTransTime	= EgOSUtils::Rnd( mTransitionLo * 1000, mTransitionHi * 1000 );
		mColorTransEnd	= mT_MS + mColorTransTime;
	}
}





#define DEC_SIZE 6

void GForce::loadDeltaField( long inFieldNum ) {
	const CEgFileSpec* spec;
	int ok = false, vers;
	ArgList args;
	UtilStr	name;

	// Fetch the spec for our config file or folder
	spec = mDeltaFields.FetchSpec( inFieldNum );

	if ( spec ) {

		// Know what to put a check mark next to in the popup menu
		mCurFieldNum = inFieldNum;

		ok = ConfigFile::Load( spec, args );
		if ( ok ) {
			vers = args.GetArg( VAL('V','e','r','s') );
			ok = vers >= 100 && vers < 110;
			spec -> GetFileName( name );
		}
	}

	if ( ! ok ) {
		args.SetArgs( __FIELD_FACTORY );
		name.Assign( "<Factory Default>" );
	}

	// Initiate recomputation of mField
	mField -> Assign( args, name );
	mNextFieldChange = mT + mFieldInterval.Evaluate();
}





void GForce::loadWaveShape( long inShapeNum, bool inAllowMorph ) {
	const CEgFileSpec* spec;
	int ok = false, vers;
	ArgList	args;

	// Fetch the spec for our config file or folder
	spec = mWaveShapes.FetchSpec( inShapeNum );

	if ( spec ) {
		// Know what to put a check mark next to in the popup menu
		mCurShapeNum = inShapeNum;

		ok = ConfigFile::Load( spec, args );

		if ( ok ) {
			vers = args.GetArg( VAL('V','e','r','s') );
			ok = vers >= 100 && vers < 110;
			spec -> GetFileName( mWaveShapeName );
		}
	}


	if ( ! ok ) {
		args.SetArgs( __SHAPE_FACTORY );
		mWaveShapeName.Assign( "<Factory Default>" );
	}

	// If the pref says so, display that we're loading a new config
	if ( mNewConfigNotify ) {
		Print( "Loaded WaveShape: " );
		Println( &mWaveShapeName );
	}


	// If first time load, don't do any transition/morph, otherwise set up the morph
	if ( mWave == 0 || ! inAllowMorph ) {
		mWave		= &mWave1;
		mNextWave	= &mWave2;
		mWave -> Load( args, mNum_S_Steps );
		mNextShapeChange = mT + mShapeInterval.Evaluate();
		mShapeTransTime = -1; }
	else {
		mNextWave -> Load( args, mNum_S_Steps );
		mWave -> SetupTransition( mNextWave );

		// Calculate how long this transition/morph will take
		mShapeTransTime	= EgOSUtils::Rnd( mTransitionLo * 1000, mTransitionHi * 1000 );
		mShapeTransEnd	= mT_MS + mShapeTransTime;
	}

}






void GForce::loadParticle( long inParticleNum ) {
	const CEgFileSpec* spec;
	int ok = false, vers;
	ArgList args;
	ParticleGroup* newParticle;
	UtilStr name;

	// Fetch the spec for our config file or folder
	spec = mParticles.FetchSpec( inParticleNum );

	if ( spec ) {

		mCurParticleNum = inParticleNum;
		ok = ConfigFile::Load( spec, args );
		if ( ok ) {
			vers = args.GetArg( VAL('V','e','r','s') );
			ok = vers >= 100 && vers < 110;

			spec -> GetFileName( name );

			// If the pref says so, display that we're loading a new config
			if ( mNewConfigNotify ) {
				Print( "Loaded Particle: " );
				Println( &name );
			}
		}
	}


	if ( ok ) {

		// Avoid having to reallocate mem...
		newParticle = (ParticleGroup*) mStoppedParticlePool.GetHead();

		// If there weren'y any particles already expired, make a new instance
		if ( ! newParticle )
			newParticle = new ParticleGroup( &mT, (ExprUserFcn**) &mSampleFcn );

		// Add the new particle to the group that gets executed each frame
		newParticle -> mTitle.Assign( name );
		mRunningParticlePool.addToHead( newParticle );

		// The GF particle probability fcn has access to these variables
		mNumRunningParticles = mRunningParticlePool.shallowCount();
		mLastParticleStart = mT;

		// Determine how long this particle will be around
		newParticle -> SetDuration( mParticleDurationFcn.Evaluate() );

		// Tell the particle to compile it's config text
		newParticle -> Load( args );
	}
}


void GForce::NewSong() {

	mTrackText.Assign( mTrackMetaText );
	mTrackText.Replace( "\\r", "\r" );
	mTrackText.Replace( "#ARTIST#", mArtist.getCStr(), false );
	mTrackText.Replace( "#ALBUM#", mAlbum.getCStr(), false );
	mTrackText.Replace( "#TITLE#", mSongTitle.getCStr(), false );

	CalcTrackTextPos();

	mLastSongStart = mT;
}



void GForce::CalcTrackTextPos() {
	int32_t height, width;
	int32_t x = mCurPort -> GetX();
	int32_t y = mCurPort -> GetY();

	mCurPort -> TextRect( mTrackText.getCStr(), width, height );

	switch ( mTrackTextPosMode ) {

		case 1:		// Upper-left corner
			mTrackTextPos.h = 5;
			mTrackTextPos.v = mTrackTextSize + 5;
			break;

		case 2:		// Bottom-left corner
			mTrackTextPos.h = 5;
			mTrackTextPos.v = y - height - 3;
			break;

		case 3:		// Centered
			mTrackTextPos.h = ( x - width )  / 2;
			mTrackTextPos.v = ( y - height ) / 2;
			break;

		default:	// Random Position
			mTrackTextPos.h = EgOSUtils::Rnd( 5, x - width );
			mTrackTextPos.v = EgOSUtils::Rnd( mTrackTextSize + 5, y - height );
			break;
	}
}



void GForce::SpawnNewParticle() {
	int i;

	// Load the next particle in the (randomized) play list...
	i = mParticlePlayList.FindIndexOf( mCurParticleNum );

	// Make a new play list if we've reached the end of the list...
	if ( i >= mParticlePlayList.Count() ) {
		mParticlePlayList.Randomize();
		i = 0;
	}

	// loadGradField() will initiate computation on mField with a new grad field...
	loadParticle( mParticlePlayList.Fetch( i + 1 ) );
}





void GForce::Print( const char* inStr ) {
	long num = mConsoleLines.Count();
	UtilStr* lastLine = mConsoleLines.Fetch( num );

	// Append the text to the console text..
	if ( lastLine )
		lastLine -> Append( inStr );
	else {
		mConsoleLines.Add( inStr );
		num = 1;
	}

	// Setup when this line will be deleted
	mLineExpireTimes[ num - 1 ] = mT_MS + mConsoleLineDur * 1000;

	// Make the console visible for the next few seconds
	mConsoleExpireTime = mT_MS + mConsoleDelay * 1000;
}


void GForce::Println( const char* inStr ) {
	Print( inStr );

	mConsoleLines.Add( "" );
}

// This is stupid!
#define PIX_PER_LINE 10

void GForce::DrawConsole() {
	long i, start, num = mConsoleLines.Count();
	long x = mDispRect.left + 5;
	long top = PIX_PER_LINE + 3;
	UtilStr* theLine;

	if ( mConsoleLines.Count() == 0 )
		return;

	// Delete console lines that are too old...
	while ( mLineExpireTimes.Fetch( 1 ) < mT_MS && num > 0 ) {
		mConsoleLines.Remove( 1 );
		mLineExpireTimes.RemoveElement( 1 );
		num--;
	}

	// Check if console runs off the display rect...
	if ( num * PIX_PER_LINE > mDispRect.bottom - mDispRect.top - top)
		start = num - ( mDispRect.bottom - mDispRect.top - top) / PIX_PER_LINE;
	else
		start = 1;

	// Draw each line of the console...
	for ( i = start; i <= num; i++ ) {
		theLine = mConsoleLines.Fetch( i );
		mCurPort -> DrawText( x, top + (i-start) * PIX_PER_LINE, *theLine );
	}
}






void GForce::ErasePane() {

}



void GForce::SetWinPort( WindowPtr inWin, const Rect* inRect ) {
	Rect r;

	// mDoingSetPortWin == true is a signal that another thread is in SetWinPort()
	if ( mDoingSetPortWin )
		return;
	mDoingSetPortWin = true;
	mWind = inWin;

	if ( inRect )
		r = *inRect;

	SetPort( 0, r, false );

	// Signal that this thread is done with SetPortWin()
	mDoingSetPortWin = false;
}




void GForce::SetPort( GrafPtr inPort, const Rect& inRect, bool inFullScreen ) {
	int32_t x = inRect.right - inRect.left;
	int32_t y = inRect.bottom - inRect.top;

	mOutPort = inPort;
	mAtFullScreen = inFullScreen;


	// The pane rect is the rect within inPort th plugin frame occupies
	mPaneRect = inRect;

	// mDispRect is the rect within inPort G-Force is drawing in (ex, the letterbox)
	// Change the disp rect if the desired size exceeds the pixel ceiling
	mDispRect = inRect;

	// Setup the offscreen port
	mPortA.Init( x, y, 8 );
	mPortB.Init( x, y, 8 );
	mCurPort = &mPortA;

	// Erase/init our output window
	mNeedsPaneErased = true;

	// If setting port for the first time...
	if ( mWave == 0 ) {
		loadWaveShape( mShapePlayList.Fetch( 1 ), false );
		loadColorMap( mColorPlayList.Fetch( 1 ), false );

		// loadGradField() will initiate computation on mField with a new grad field...
		loadDeltaField( mFieldPlayList.Fetch( 1 ) );
		DeltaField* temp = mField;
		mField = mNextField;
		mNextField = temp;
		loadDeltaField( mFieldPlayList.Fetch( 2 ) );
	}

	// The grad fields have to know the pixel dimentions
	mField1.SetSize( x, y, mPortA.GetRowSize() );
	mField2.SetSize( x, y, mPortA.GetRowSize() );

	// The track text may depend on the port size
	CalcTrackTextPos();

	// Changing the port (and the resolution) may change the mouse cords
	EgOSUtils::GetMouse( mLastMousePt );

}

void GForce::GetWinRect( Rect& outRect ) {
	SetRect( &outRect, 0, 0, 0, 0 );
}

