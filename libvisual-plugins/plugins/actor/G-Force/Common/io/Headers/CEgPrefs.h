#ifndef _CEGPREFS_
#define _CEGPREFS_

#include "ArgList.h"

class CEgFileSpec;
class Exam;

class CEgPrefs {
	
	private:
		
		static CEgFileSpec			sPrefsSpec;	
						
			
	public:
		
		static ArgList				sPLFactoryDefaults;
		static ArgList				sPLDefaults;
		static ArgList				sFactoryPrefs;
		static ArgList				sPrefs;

		// 	Call this at program startup!
		static void					LoadPrefs();
		
		//			This is true if valid prefs were initally found
		static bool					sPrefsFound;
		
		// 			Saves Exam::sExamDefaults, sPLDefaults and sEgPrefs to disk.
		static void					SavePrefs();
};

#endif