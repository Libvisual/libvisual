
#include "CEgPrefs.h"

#include "Exam.h"
#include "EgOSUtils.h"
#include "CEgFileSpec.h"

#include "CEgIOFile.h"




bool		CEgPrefs::sPrefsFound	= false;
CEgFileSpec	CEgPrefs::sPrefsSpec;
ArgList		CEgPrefs::sPLFactoryDefaults;
ArgList		CEgPrefs::sPLDefaults;
ArgList		CEgPrefs::sFactoryPrefs;
ArgList		CEgPrefs::sPrefs;

#ifdef EG_WIN
#include <windows.h>		// For ::GetCurrentDirectory()
#endif

#ifdef EG_MAC
#include <Files.h>
#include <Folders.h>
#endif

	
void CEgPrefs::LoadPrefs() {

	// Set the factory settings
	sPLFactoryDefaults.SetArgs( "PtSz=11,Font=\"Times\",Ttls=0,IDNm=1,PgBk=0,STit=0,ColL=1,BdMr=0,Cols=1,TopM=54,BotM=36,LftM=36,RgtM=36,Hdr=\"\",Ftr=\"\"" );
	sFactoryPrefs.SetArgs( "" );

	#ifdef EG_MAC
  	short int	theVRef;
	long		theDirID;
	short		theErr;
	FSSpec		prefSpec;

	theErr = ::FindFolder( kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &theVRef, &theDirID );
					
	if (theErr == noErr) {
		::FSMakeFSSpec( theVRef, theDirID, "\pExamgen Preferences", &prefSpec );
		sPrefsSpec.Assign( &prefSpec, cPrefType );
	}
	#endif
	
	#ifdef EG_WIN
	UtilStr prefPath;
	char path[ 500 ];
	if ( ::GetCurrentDirectory( 495, path ) ) {
		prefPath.Assign( path );
		prefPath.Append( "\\Examgen Preferences" );
		sPrefsSpec.Assign( prefPath.getCStr(), 0 );
	}
	
	sPLFactoryDefaults.SetArg( 'Font', "Times New Roman" );
	#endif
	
	CEgIFile 		prefsFile;		
	UtilStr		str;
	
	prefsFile.open( &sPrefsSpec );
	
	Exam::sFactoryExam.MakeFactory();
	
	if ( prefsFile.noErr() ) {
		Exam::sDefaultExam.ReadFrom( &prefsFile );
		
		// Load PL prefs, having the factor PL prefs be the defaults
		sPLDefaults.SetArgs( sPLFactoryDefaults );
		prefsFile.Readln( str );
		sPLDefaults.SetArgs( str );
		
		// Load program prefs, having the factory PL prefs be the defaults
		sPrefs.SetArgs( sFactoryPrefs );
		prefsFile.Readln( str );
		sPrefs.SetArgs( str );
	}
	
	if ( prefsFile.noErr() && Exam::sDefaultExam.GetAttrib( 0, 'V' ) == 420 ) 
		sPrefsFound = true;
	else {
		prefsFile.close();
		Exam::sDefaultExam.MakeFactory();
		SavePrefs(); 
	}
}





void CEgPrefs::SavePrefs() {
	CEgIOFile 		prefsFile;
	
	prefsFile.open( &sPrefsSpec );
	
	if ( prefsFile.noErr() ) {
		Exam::sDefaultExam.WriteTo( &prefsFile );
		sPLDefaults.ExportTo( &prefsFile );
		prefsFile.Writeln();
		sPrefs.ExportTo( &prefsFile );
		prefsFile.Writeln();
		prefsFile.Writeln();
		prefsFile.Writeln();
	}
	
	if ( ! prefsFile.noErr() )
		EgOSUtils::ShowMsg( "File error writing preferences file." );

}



