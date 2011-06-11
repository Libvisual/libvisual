


#ifndef __XPTRMATRIX__
#define __XPTRMATRIX__


#include "XPtrList.h"


class XPtrMatrix {

	public:
							XPtrMatrix( bool inRowOrderImpt = false );
		virtual				~XPtrMatrix();
		
		// Removes all the rows (ie, elements)				
		void				RemoveAll();
		
		// Appends the given ptr to the end of the given column.
		// Note: zero-based indexing is used for the column number
		void				Add( long inColumn, void* inPtr );
		inline void			Add( long inColumn, long  inNum )     { Add( inColumn, (void*) inNum );	}
		
		// Fetches the specified column (using zero-based indexing)
		XPtrList*			operator[]( long inIndex );	
	
	
	protected:
		XPtrList			mColumns;
		ListOrderingT		mRowOrdering;
};


#endif

