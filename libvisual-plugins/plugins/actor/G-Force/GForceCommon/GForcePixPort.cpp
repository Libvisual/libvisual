#include "GForcePixPort.h"


#if EG_MAC
#define __consoleFont		"Monaco"

#elif EG_WIN
#define __consoleFont		"Courier New"

#else
#define __consoleFont           ""
/* FIXME */
#endif



GForcePixPort::GForcePixPort() {

	mConsoleFontID = CreateFont();
	AssignFont( mConsoleFontID, __consoleFont, 9 );
	
	mTrackTextFontID = CreateFont();
}


void GForcePixPort::SetTrackTextFont( UtilStr& inName, long inSize ) {

	AssignFont( mTrackTextFontID, inName.getCStr(), inSize );
}

