#include "GForcePixPort.h"


GForcePixPort::GForcePixPort() {

	mConsoleFontID = CreateFont();
	AssignFont( mConsoleFontID, "", 9 );
	
	mTrackTextFontID = CreateFont();
}


void GForcePixPort::SetTrackTextFont( UtilStr& inName, long inSize ) {

	AssignFont( mTrackTextFontID, inName.getCStr(), inSize );
}

