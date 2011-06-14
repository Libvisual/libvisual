
#ifndef _EGOSUTILS_
#define _EGOSUTILS_

#include "EgCommon.h"
#include "CEgFileSpec.h"
#include "XStrList.h"

#if EG_MAC
#include <Quickdraw.h>
#endif

#define INITAL_DIR_STRLEN	502

class UtilStr;
class CEgFileSpec;
class CEgErr;

class EgOSUtils {
			
						
	public:
		// 			Call this once at program startup/end!!
		//  If you're compiling under windows and making a DLL, pass the DLL instance otherwise 0 is fine.
		static void					Initialize( void* inModuleInstance = 0 );
		static void					Shutdown();
	
		//	Post:	Assigns spec info for the next file in the dir specified by <folderSpec>.  If the directory doesn't exist or there
		//			aren't any more files, <false> is returned.  If a next file is found, <true> is returned and <outSpec> contains a spec to that file.
		//			If inFolders is true, only folders are retuned
		static bool					GetNextFile( const CEgFileSpec& folderSpec, CEgFileSpec& outSpec, bool inStartOver, bool inFolders );
	
		//			After Initialize, these contain the screen pixels per inch
		static int					sXdpi;
		static int					sYdpi;
		
		// 	Post:	This static method will display the given C string in a window with an OK button.
		static void					ShowMsg( const char* inMsg );
		static void					ShowMsg( const UtilStr& inMsg );
		
		//	Post:	Prints <inMsg> in a dialog box with an "OK" button and a "cancel" button.  <true>
		//			is returned if the use selects the "OK" button
		static bool					AreYouSure( const UtilStr& inMsg );
		static bool					AreYouSure( const char* inMsg );
		
		//	Post:	Asks "Save changes to '<inName>' before closing?"
		//	Note:	Returns 	0 if Cancel button, 1 for OK, and -1 for Don't Save
		static int					AskSaveChanges( const UtilStr& inName );
		static int					AskSaveChanges( const char* inName );
	
		//	Post:	With the file dialog, this prompts the user for a file name, etc.  If the func returns <true>,
		//			<outSpec> contains the file spec the user just entered
		static bool					AskSaveAs( const char* inPrompt, const UtilStr& inDefaultName, CEgFileSpec& outSpec, long inFileType = 0 );
		static bool					AskSaveAs( const char* inPrompt, const char* inDefaultName, CEgFileSpec& outSpec, long inFileType = 0 );
	
		//	Post:	With the file dialog, this prompts the user to select a file..  If the func returns <true>,
		//			<outSpec> contains the file spec the user just entered.
		//	Note:	Only file of type <inFileType> are visible. Zero means to accept all files types.
		static bool					AskOpen( const char* inPrompt, CEgFileSpec& outSpec, long inFileType = 0 );

		//	Post:	Puts the error message in a message box on the screen, saying 
		//			"Error reading/writing "inName" -- inErrMsg
		//			<wasReading> determines chooses the word "reading" vs. "writing"
		static void					ShowFileErr( const UtilStr* inName, CEgErr& inErr, 			bool wasReading );
		static void					ShowFileErr( const UtilStr* inName, char* inErrMsg, 		bool wasReading );
		static void					ShowFileErr( const CEgFileSpec& inFileSpec, CEgErr& inErr,	bool wasReading );
		
		//	Post:	Makes the computer beep	
		static void					Beep();
		

		//	Pre:	Designed to be called continously during a long process.
		//	Post:	Displays a spinning curor after a certain period of time.
		static void					SpinCursor();
		
		//	Post:	Call this to restore the cursor if it's been altered (ie, after a SpinCursor).
		//	Note:	If This proc is installed in the main event loop, you can call SpinCursor() whenever things are intensive
		static void					ResetCursor()												{ if ( sLastCursor >= 0 ) EgOSUtils::ShowCursor();  }
		
		// 	Post:	Hides/Shows the mouse cursor.
		static void					ShowCursor();
		static void					HideCursor();
		
		// 	Returns the current time in milliseconds since the system start.
		static long					CurTimeMS();
		
		//	Returns the global cords of the mouse
		static void					GetMouse( Point& outPt );
		
		//	Post:	Returns a random number from <min> to <max>, inclusive.
		static long 				Rnd( long min, long max );
		
		//	Post:	Reverses the byte order of the given long
		static unsigned long		RevBytes( unsigned long inNum );
		
		// Is the directory/folder of this app/lib
		static CEgFileSpec			sAppSpec;
		
		// Converts a float HSV (each component is 0 to 1) into a 3x16 bit RGB value
		// H = .2 == 1.2 == 7.2 == -.8 == -3.8  (ie, H values are 'wrapped')
		// S,V = 1.0 == 1.1 == 6.9
		// S,V = 0 == -.1 == -4
		static void					HSV2RGB( float H, float S, float V, RGBColor& outRGB );
		
	protected:
		static XStrList				sFontList;
		static long					sLastCursor;
		static long					sLastCursorChange;
		static char					sInitialDir[ INITAL_DIR_STRLEN + 2 ];

};

#endif
