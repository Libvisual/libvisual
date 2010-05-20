#ifndef XDynArray_H
#define XDynArray_H


#include "UtilStr.h"





class XDynArray : protected UtilStr {


	public:	
		// The argument specified how big each element is in this array 
								XDynArray( long inRecSize );
										
		// Returns the dimmed size of this Array
		inline long				Count() const							{ return mNumElements;			}
	
		// Effectively makes the length of this array zero.
		void					RemoveAll() 							{ Wipe();  mNumElements = 0;	}
		
		// 	Allows easy dynamic array usage.  Simple use any index and XPtrList will expand to meet that size.
		//	Impt:	Zero based indexing.
		//	Note:	Indexs below 0 lead to disaster;
		//	Note:	Since caller has access to changes values, any current sorting fcn is not used
		void*					operator[] ( const long inIndex );			



	protected:
		long					mNumElements;
		long					mRecSize;
		
		static void*			sDummy;
};


#endif