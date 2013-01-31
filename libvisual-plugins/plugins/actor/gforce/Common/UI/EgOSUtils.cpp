#include <libvisual/libvisual.h>
#include "EgOSUtils.h"

#include "CEgFileSpec.h"
#include "CEgErr.h"
#include "UtilStr.h"

#include <limits>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef EG_MAC
#include <Resources.h>
#include <Sound.h>
#include <Fonts.h>
#endif

#ifdef EG_WIN
#include <windows.h>
#endif

#ifdef UNIX_X
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#endif


int				EgOSUtils::sXdpi				= 72;
int				EgOSUtils::sYdpi				= 72;
long			EgOSUtils::sLastCursor			= -1;
long			EgOSUtils::sLastCursorChange 	= -1;
CEgFileSpec 	EgOSUtils::sAppSpec;
char			EgOSUtils::sInitialDir[ INITAL_DIR_STRLEN + 2 ];
XStrList		EgOSUtils::sFontList( cDuplicatesAllowed );




void EgOSUtils::ShowFileErr( const UtilStr* inName, char* inErrMsg, bool wasReading ) {
	UtilStr	s;

	if ( wasReading )
		s.Assign( "Error reading : " );
	else
		s.Assign( "Error writing : " );
	s.Append( inErrMsg );

	#ifdef EG_MAC
	s.Insert( 14, "рс" );
	#else
	s.Insert( 14, "\"\"" );
	#endif

	s.Insert( 15, inName );
	ShowMsg( s );
}


void EgOSUtils::ShowFileErr( const UtilStr* inName, CEgErr& inErr, bool wasReading ) {
	UtilStr	msg;

	inErr.GetErrStr( msg );
	ShowFileErr( inName, msg.getCStr(), wasReading );
}



void EgOSUtils::ShowFileErr( const CEgFileSpec& inFileSpec, CEgErr& inErr, bool wasReading ) {
	UtilStr	msg, fileName;

	inFileSpec.GetFileName( fileName  );
	inErr.GetErrStr( msg );
	ShowFileErr( &fileName, msg.getCStr(), wasReading );
}





void EgOSUtils::Initialize( void* inModuleInstance ) {

	#if EG_MAC

	#pragma unused( inModuleInstance )
	OSStatus	status;
	FCBPBRec	fcbPBRec;
	Str255		appName;
	FSSpec		spec;

	fcbPBRec.ioCompletion	= nil;
	fcbPBRec.ioFCBIndx		= 0;
	fcbPBRec.ioVRefNum		= 0;
	fcbPBRec.ioRefNum		= ::CurResFile();
	fcbPBRec.ioNamePtr		= appName;

	status = ::PBGetFCBInfoSync(&fcbPBRec);

	::FSMakeFSSpec(fcbPBRec.ioFCBVRefNum, fcbPBRec.ioFCBParID, appName, &spec );
	sAppSpec.Assign( &spec, 0 );
	#endif

	#ifdef EG_WIN
	HDC	hDC = ::CreateDC( "DISPLAY", 0, 0, 0 );
	if ( hDC ) {
		sXdpi = ::GetDeviceCaps( hDC, LOGPIXELSX );
		sYdpi = ::GetDeviceCaps( hDC, LOGPIXELSY );
		::DeleteDC( hDC );
	}

	char path[ 700 ];
	long len = ::GetModuleFileName( (HINSTANCE) inModuleInstance, path, 699 );
	if ( len ) {
		UtilStr fdir( path, len );
		fdir.Keep( fdir.FindLastInstanceOf( '\\' ) );
		sAppSpec.Assign( fdir.getCStr(), 0 );
	}


	// Init timer accuracy...
	///::timeBeginPeriod( 1 );

	// Remember the initial dir
	::GetCurrentDirectory( INITAL_DIR_STRLEN, sInitialDir );
	#endif

	// For mac, sXspi and sYdpi are already correct by default
	#ifdef UNIX_X
	sXdpi = sYdpi = 75;
	#endif
}





void EgOSUtils::Shutdown() {
	#ifdef UNIX_X
//        xpce_CloseGraph();
 	#endif
	#if EG_WIN

	// Restore the initial dir
	::SetCurrentDirectory( sInitialDir );

	// Init timer accuracy...
	//::timeEndPeriod( 1 );

	#endif

}







bool EgOSUtils::AskSaveAs( const char* inPrompt, const char* inDefaultName, CEgFileSpec& outSpec, long inFileType ) {
	UtilStr inName( inDefaultName );

	return AskSaveAs( inPrompt, inName, outSpec, inFileType );
}


#ifdef EG_ZINC
#include <z_fildlg.hpp>
#endif




bool EgOSUtils::AskSaveAs( const char* inPrompt, const UtilStr& inDefaultName, CEgFileSpec& outSpec, long inFileType ) {
	int	doSave = false;
	UtilStr prompt( inPrompt );

	#ifdef EG_MAC
	StandardFileReply	reply;

	//::UDesktop::Deactivate();
	::StandardPutFile( prompt.getPasStr(), inDefaultName.getPasStr(), &reply );
	//::UDesktop::Activate();

	if (reply.sfGood) {
		outSpec.Assign( &reply.sfFile, inFileType );
		doSave = true;
	}
	#endif

	#if EG_WIN && EG_ZINC
	UtilStr defName( inDefaultName );

	ZafFileDialog	fileDialog( 20, 5, 60, 14 );

	if ( inFileType )
		CEgFileSpec::TypeToExt( defName, inFileType );

	fileDialog.SetFile( defName.getCStr()  );

	if ( fileDialog.GetFile( ZAF_FILE_DIALOG_SAVEAS ) == S_DLG_OK ) {
		doSave = true;
		outSpec.Assign( fileDialog.File(), inFileType );
	}
	/*
	unsigned char	c;
	UtilStr		winFilter;
	OPENFILENAME 	paramBlk;
	char			pathName[ 601 ];

	inDefaultName.copyTo( pathName, 600 );

	// See page 519 of Vol 5 in Win32 Ref for descrip of the lpstrFilter scheme.
	winFilter.Append( "/0*" );

	// Append the ext mask...
	for ( int d = 0; d <= 24; d += 8 ) {			// Go thru each byte in ID num
		c = ((inFileType << d) >> 24 );
		winFilter.Append( (char) c );
	}
	winFilter.Append( (char) 0 );					// Windows exptects an extra NUL to end the filter

	visual_mem_set( &paramBlk, 0, sizeof(OPENFILENAME) );
	paramBlk.lStructSize = sizeof(OPENFILENAME);
	paramBlk.lpstrFilter		= winFilter.getCStr();
	paramBlk.lpstrFile			= pathName;
	paramBlk.nMaxFile			= 600;
	paramBlk.lpstrTitle			= prompt.getCStr();
	if ( ::GetSaveFileName( &paramBlk ) ) {
		outSpec.Assign( pathName );
		doSave = true;
	}*/
	#endif

	return doSave;
}




bool EgOSUtils::AskOpen( const char* inPrompt, CEgFileSpec& outSpec, long inTypeMask ) {
	bool didOpen = false;
	UtilStr prompt( inPrompt );

	#ifdef EG_MAC
	StandardFileReply	macFileReply;
	SFTypeList			typeList;

	// clear the rec
	for ( int i = 0; i < sizeof( macFileReply ); i++ )
		( (char*) &macFileReply )[ i ] = 0;

	//UDesktop::Deactivate();
	typeList[0] = inTypeMask;
	::StandardGetFile( 0, inTypeMask ? 1 : 0, typeList, &macFileReply );
	//UDesktop::Activate();

	if ( macFileReply.sfGood ) {
		outSpec.Assign( &macFileReply.sfFile );
		didOpen = true;
	}
	#endif

	#if EG_WIN && EG_ZINC
	ZafFileDialog	fileDialog( 5, 5, 60, 14 );
	UtilStr		str( "*" );

	if ( inTypeMask ) {
		CEgFileSpec::TypeToExt( str, inTypeMask );
		fileDialog.SetFilter( str.getCStr() );
	}

	if ( fileDialog.GetFile( ZAF_FILE_DIALOG_OPEN ) == S_DLG_OK ) {
		char path[ 401 ];
		didOpen = true;

		fileDialog.FullPath( path, 400 );
		outSpec.Assign( path );
	}
	/*
	unsigned char	c;
	UtilStr		winFilter;
	OPENFILENAME 	paramBlk;
	char			pathName[ 601 ];

	// See page 519 of Vol 5 in Win32 Ref for descrip of the lpstrFilter scheme.
	winFilter.Append( "/0*" );

	// Append the ext mask...
	for ( int d = 0; d <= 24; d += 8 ) {			// Go thru each byte in ID num
		c = ((inTypeMask << d) >> 24 );
		winFilter.Append( (char) c );
	}
	winFilter.Append( (char) 0 );					// Windows exptects an extra NUL to end the filter

	visual_mem_set( &paramBlk, 0, sizeof(OPENFILENAME) );
	paramBlk.lStructSize = sizeof(OPENFILENAME);
	paramBlk.lpstrFilter		= winFilter.getCStr();
	paramBlk.lpstrFile			= pathName;
	paramBlk.nMaxFile			= 600;
	paramBlk.lpstrTitle			= prompt.getCStr();
	if ( ::GetOpenFileName( &paramBlk ) ) {
		outSpec.Assign( pathName );
		didOpen = true;
	}*/
	#endif

	return didOpen;
}


bool EgOSUtils::AreYouSure( const UtilStr& inMsg ) {
	#ifdef EG_MAC
	::ParamText( inMsg.getPasStr(), "\p", "\p", "\p");
	//UDesktop::Deactivate();
	ans = ::CautionAlert( 2000, 0 );
	//UDesktop::Activate();

	return ans == 1; //answer_Save;
	#endif

	#ifdef EG_WIN
	int ans = ::MessageBox( 0, inMsg.getCStr(), "Examgen Message", MB_ICONEXCLAMATION | MB_YESNO | MB_SETFOREGROUND | MB_TASKMODAL );
	return ans == IDYES;
	#endif

	return true;
}


bool EgOSUtils::AreYouSure( const char* inMsg ) {
	UtilStr	msg( inMsg );

	return AreYouSure( msg );
}



int EgOSUtils::AskSaveChanges( const char* inName ) {
	UtilStr name( inName );
	return AskSaveChanges( name );
}



int EgOSUtils::AskSaveChanges( const UtilStr& inName ) {
	#ifdef EG_MAC
	::ParamText( inName.getPasStr(), "\p", "\p", "\p" );
	//UDesktop::Deactivate();
	int ans = ::CautionAlert( 2001, 0 );
	//UDesktop::Activate();

	return 2 - ans;
	#endif

	#ifdef EG_WIN
	UtilStr	msg( "Save changes to \"" );
	msg.Append( inName );
	msg.Append( "\" before closing?" );
	ans = ::MessageBox( 0, msg.getCStr(), "Examgen Message", MB_ICONEXCLAMATION | MB_YESNOCANCEL | MB_SETFOREGROUND | MB_TASKMODAL );
	if ( ans == IDYES )
		return 1;
	else if ( ans == IDNO )
		return -1;
	else
		return 0;
	#endif

	return 0;
}


void EgOSUtils::SpinCursor() {
	long time = clock();

	if ( sLastCursorChange == -1 )
		sLastCursorChange = time;
	else if ( time - CLOCKS_PER_SEC / 3 > sLastCursorChange ) {					// Every 1/3 second...
		#ifdef EG_MAC
		Handle cursHndl;
		sLastCursor			= ( sLastCursor + 1 ) % 8;							// 8 Cursors
		cursHndl			= ::GetResource( 'CURS', 6500 + sLastCursor );		// 6500 = Base ID
		sLastCursorChange	= time;
		if ( cursHndl )
			::SetCursor( (Cursor*) *cursHndl );
		#endif

		#ifdef EG_WIN
		SetCursor( ::LoadCursor( 0, IDC_WAIT ) );
		sLastCursor = 1;
		#endif
	}
}








void EgOSUtils::ShowCursor() {

	#ifdef EG_MAC
	::ShowCursor();
	::InitCursor();
	#endif

	#ifdef EG_WIN
	::SetCursor( ::LoadCursor( 0, IDC_ARROW ) );
	while ( ::ShowCursor( true ) < 0 ) { }
	#endif

	sLastCursor			= -1;
	sLastCursorChange	= -1;
}


void EgOSUtils::HideCursor() {

	#ifdef EG_MAC
	::HideCursor();
	#endif

	#ifdef EG_WIN
	while ( ::ShowCursor( false ) >= 0 ) { }
	#endif

	sLastCursor			= -1;
	sLastCursorChange	= -1;
}



bool EgOSUtils::GetNextFile( const CEgFileSpec& folderSpec, CEgFileSpec& outSpec, bool inStartOver, bool inFolders ) {
	bool ok;

	#ifdef EG_MAC
	static int			sLastIndex;
	OSErr				err;
	HFileInfo			pb;
	FSSpec				spec;
	long				parID;
	Str255				str;

	parID			= ( (FSSpec*) folderSpec.OSSpec() ) -> parID;
	pb.ioVRefNum	= ( (FSSpec*) folderSpec.OSSpec() ) -> vRefNum;
	pb.ioNamePtr 	= str;

	if ( inStartOver )
		pb.ioFDirIndex = 1;
	else
		pb.ioFDirIndex = sLastIndex;

	ok = false;
	do {
		pb.ioDirID 	= parID;
		err = PBGetCatInfoSync( (CInfoPBRec*) &pb );

		if ( err == noErr && ( (pb.ioFlFndrInfo.fdFlags & fInvisible) == 0)  /*&& pb.ioFlFndrInfo.fdCreator == cEGCreator*/ ) {
			ok = ( (pb.ioFlAttrib & ioDirMask ) == 0 );
			if ( inFolders )
				ok = ! ok;
			if ( ok ) {
				if ( inFolders )
					pb.ioFlParID = pb.ioDirID;
				::FSMakeFSSpec( pb.ioVRefNum, pb.ioFlParID, str, &spec );
				outSpec.Assign( &spec, pb.ioFlFndrInfo.fdType );
			}
		}

		pb.ioFDirIndex++;
		sLastIndex = pb.ioFDirIndex;
	} while ( err == noErr && ! ok );

	#elif defined(EG_WIN)

	WIN32_FIND_DATA		fileData;
	static HANDLE		hSearch;
	UtilStr				name;
	bool				isDir, tryAgain;

	do {
		if ( inStartOver ) {
			inStartOver = false;
			if ( name.getChar( name.length() ) == '\\' )
				name.Trunc( 1 );
			ok = SetCurrentDirectory( name.getCStr() );
			if ( ok ) {
				hSearch = ::FindFirstFile( "*.*", &fileData );
				ok = hSearch != INVALID_HANDLE_VALUE;
			} }
		else
			ok = ::FindNextFile( hSearch, &fileData );
		if ( ok ) {
			name.Assign( fileData.cFileName );
			isDir = ::GetFileAttributes( fileData.cFileName ) & FILE_ATTRIBUTE_DIRECTORY;
			if ( isDir == inFolders ) {
				tryAgain = name.compareTo( "." ) == 0 || name.compareTo( ".." ) == 0;
				outSpec.Assign( folderSpec );
				if ( isDir )
					name.Append( "\\" );
				outSpec.Rename( name ); }
			else
				tryAgain = true;
		}
	} while ( ok && tryAgain );

	#elif defined(UNIX_X)

	UtilStr				name;
	bool				isDir, tryAgain;
	UtilStr                         fullname;

	static DIR *d = 0;
	struct dirent *de;

	ok = true;
	do {
	  if ( inStartOver ) {
	    if (d != 0) {
	      closedir(d);
	      d = 0;
	    }
	    inStartOver = false;
	    name.Assign( (char*) folderSpec.OSSpec() );
	    if ( name.getChar( name.length() ) == '/' )
	      name.Trunc( 1 );
	    d = opendir(name.getCStr());
	    if (d == 0) return 0;
	  }
	  de = readdir(d);
	  if (de == 0) return 0;
	  name.Assign(de->d_name);
	  struct stat statdata;
	  fullname.Assign( (char*) folderSpec.OSSpec() );
	  if ( fullname.getChar( name.length() ) != '/' )
	      fullname.Append( '/' );
	  fullname.Append(de->d_name);
	  if (stat(fullname.getCStr(), &statdata) != 0)
	    return 0;
	  if (S_ISDIR(statdata.st_mode))
	    isDir = 1;
	  else
	    isDir = 0;

	  if ( isDir == inFolders ) {

	    tryAgain = name.compareTo( "." ) == 0 || name.compareTo( ".." ) == 0;
	    outSpec.Assign( folderSpec );
	    if ( isDir )
	      name.Append( "\\" );
	    outSpec.Rename( name ); }
	  else
	    tryAgain = true;
	} while ( ok && tryAgain );

	#else

	ok = false;

	#endif

	return ok;
}










void EgOSUtils::Beep() {

	#ifdef EG_MAC
	::SysBeep( 200 );
	#endif

	#ifdef EG_WIN
	MessageBeep(0);
	#endif
}


long EgOSUtils::CurTimeMS() {
	#if EG_WIN
	return ::timeGetTime();
	#elif EG_MAC
	return ::TickCount() * 16;
        #else
	struct timeval tv;
	struct timezone tz;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
	gettimeofday(&tv, &tz);
	return ((tv.tv_sec * 1000) & 0x7fffffff) + tv.tv_usec / 1000;
        #endif
}


void EgOSUtils::GetMouse( Point& outPt ) {

	#if EG_MAC
	::GetMouse( &outPt );
	::LocalToGlobal( &outPt );
	#endif

	#if EG_WIN
	POINT p;
	::GetCursorPos( &p );
	outPt.h = p.x;
	outPt.v = p.y;
	#endif

	#ifdef UNIX_X
	int x, y;

//	xpce_QueryMouse(&x, &y); // FIXME replace this
	x = 0;
	y = 0;
	outPt.h = x;
	outPt.v = y;
	#endif
}


void EgOSUtils::ShowMsg( const UtilStr& inMsg ) {

	#ifdef EG_MAC
	//UDesktop::Deactivate();
	::ParamText( inMsg.getPasStr(), "\p", "\p", "\p");
	::StopAlert( 2002, 0 );
	//#pragma rem back in!
	//UDesktop::Activate();
	#endif

	#ifdef EG_WIN
	::MessageBox( 0, inMsg.getCStr(), "Examgen Message", MB_ICONEXCLAMATION | MB_OK | MB_SETFOREGROUND | MB_APPLMODAL );
	//ZafMessageWindow* w = new ZafMessageWindow( "Message", ZAF_EXCLAMATION_ICON, ZAF_DIALOG_OK, ZAF_DIALOG_OK, inMsg.getCStr() );
	//zafWindowManager -> Add( w );
	//w -> Control();
	#endif

}



void EgOSUtils::ShowMsg( const char* inMsg ) {
	UtilStr msg( inMsg );

	ShowMsg( msg );
}



long EgOSUtils::Rnd( long min, long max ) {
	long maxRnd 	= std::numeric_limits<uint32_t>::max();
	long retNum 	= LV::rand() * ( max - min + 1 ) / maxRnd + min;

	if ( retNum >= max )
		return max;
	else
		return retNum;
}




unsigned long EgOSUtils::RevBytes( unsigned long inNum ) {


	return ( inNum << 24 ) | ( ( inNum & 0xFF00 ) << 8 ) | ( ( inNum & 0xFF0000 ) >> 8 ) | ( inNum >> 24 );
}



#define __SET_RGB( R, G, B ) 	\
	if ( R < 0 )				\
		outRGB.red = 0;			\
	else if ( R <= 0xFFFF )		\
		outRGB.red = R;			\
	else						\
		outRGB.red = 0xFFFF;	\
	if ( G < 0 )				\
		outRGB.green = 0;		\
	else if ( G <= 0xFFFF )		\
		outRGB.green = G;		\
	else						\
		outRGB.green = 0xFFFF;	\
	if ( B < 0 )				\
		outRGB.blue = 0;		\
	else if ( B <= 0xFFFF )		\
		outRGB.blue = B;		\
	else						\
		outRGB.blue = 0xFFFF;	\
	break;





void EgOSUtils::HSV2RGB( float H, float S, float V, RGBColor& outRGB ) {


	// H is given on [0, 1] or WRAPPED. S and V are given on [0, 1].
	// RGB are each returned on [0, 1].
	long hexQuadrant, m, n, v;
	H = ( H - floor( H ) ) * 6;  // Wrap the Hue angle around 1.0, then find quadrant

	hexQuadrant = H;
	float f = H - hexQuadrant;

	// Check sat bounds
	if ( S < 0 )
		S = 0;
	if ( S > 1 )
		S = 1;

	// Check val bounds
	if ( V < 0 )
		V = 0;
	if ( V > 1 )
		V = 1;

	if ( ! ( hexQuadrant & 1 ) )
		f = 1 - f; // hexQuadrant i is even

	V *= 65535.0;
	v = V;
	m = V * (1 - S);
	n = V * (1 - S * f);

	switch ( hexQuadrant ) {
		case 1: __SET_RGB( n, v, m );
		case 2: __SET_RGB( m, v, n );
		case 3: __SET_RGB( m, n, v );
		case 4: __SET_RGB( n, m, v );
		case 5: __SET_RGB( v, m, n );
		default:
			__SET_RGB( v, n, m );
	}
}



/*
#define RETURN_HSV(h, w, b) {HSV.H = h; HSV.S = s; HSV.V = v; return HSV;}

#define RETURN_RGB(r, g, b) {RGB.R = r; RGB.G = g; RGB.B = b; return RGB;}

#define UNDEFINED -1

// Theoretically, hue 0 (pure red) is identical to hue 6 in these transforms. Pure

// red always maps to 6 in this implementation. Therefore UNDEFINED can be

// defined as 0 in situations where only unsigned numbers are desired.

typedef struct {float R, G, B;} RGBType;

typedef struct {float H, S, V;} HSVType;

HSVType

RGB_to_HSV( RGBType RGB ) {

            // RGB are each on [0, 1]. S and V are returned on [0, 1] and H is
            // returned on [0, 6]. Exception: H is returned UNDEFINED if S==0.
            float R = RGB.R, G = RGB.G, B = RGB.B, v, x, f;
            int i;
            HSVType HSV;
            x = min(R, G, B);
            v = max(R, G, B);
            if(v == x) RETURN_HSV(UNDEFINED, 0, v);
            f = (R == x) ? G - B : ((G == x) ? B - R : R - G);
            i = (R == x) ? 3 : ((G == x) ? 5 : 1);
            RETURN_HSV(i - f /(v - x), (v - x)/v, v);


}


RGBType

HSV_to_RGB( HSVType HSV ) {

           // H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
           // RGB are each returned on [0, 1].
           float h = HSV.H, s = HSV.S, v = HSV.V, m, n, f;
           int i;
           RGBType RGB;
           if(h == UNDEFINED) RETURN_RGB(v, v, v);
           i = floor(h);
           f = h - i;
           if(!(i & 1)) f = 1 - f; // if i is even
           m = v * (1 - s);
           n = v * (1 - s * f);
           switch (i) {
                     case 6:
                     case 0: RETURN_RGB(v, n, m);
                     case 1: RETURN_RGB(n, v, m);
                     case 2: RETURN_RGB(m, v, n)
                     case 3: RETURN_RGB(m, n, v);
                     case 4: RETURN_RGB(n, m, v);
                     case 5: RETURN_RGB(v, m, n);
           }


}
*/

