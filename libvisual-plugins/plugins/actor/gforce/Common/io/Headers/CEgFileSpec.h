#ifndef _CEGFILESPEC_
#define _CEGFILESPEC_


#include "UtilStr.h"
#include "CEgErr.h"

#define VAL(a,b,c,d) (((a)<<24)+((b)<<16)+((c)<<8)+(d))

class CEgFileSpec {

	protected:
		UtilStr					mSpecData;
		long					mFileType;


	public:
								CEgFileSpec();
								CEgFileSpec( const char* inPathName, long inType = VAL('T','E','X','T')  );
								CEgFileSpec( const CEgFileSpec& inSpec );



		//	Returns 1 is the file is found, 2 if the folder is found, 0 if not found
		int						Exists() const;

		// 	Appends "1", "2", "3", ... until the file name is unique/not taken
		void					MakeUnique();

		void					Assign( const CEgFileSpec& inSpec );
		void					AssignPathName( const char* inPathName );
		void					Assign( const void* inOSSpecPtr, long inType = VAL('T','E','X','T') );

		CEgErr					Duplicate( const CEgFileSpec& inDestSpec ) const;
		void					SaveAs( const CEgFileSpec& inDestSpec ) const;
		void					ChangeExt( const char* inExt );

		void					Delete() const;
		long					GetType() const;
		void					SetType( long inType );

		const void*				OSSpec() const;

		//	Post:	Returns the filename of this spec (without extension)
		//	Usgae:	"Blah.BL" ==>	Returns "Blah" in <outFileName>
		void					GetFileName( UtilStr& outFileName ) const;

		//	Post:	Sets this spec's filename, keeping the path information the same.
		void					Rename( const UtilStr& inNewName );

		void					AssignFolder( const char* inFolderName );
};

#undef VAL

#endif



