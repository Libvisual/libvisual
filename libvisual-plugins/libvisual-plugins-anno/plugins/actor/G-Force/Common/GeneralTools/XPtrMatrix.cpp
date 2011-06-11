#include "XPtrMatrix.h"


XPtrMatrix::XPtrMatrix( bool inRowOrderImpt ) :
	mColumns( cOrderImportant ) {
	
	mRowOrdering = inRowOrderImpt ? cOrderImportant : cOrderNotImportant;
}


XPtrMatrix::~XPtrMatrix() {
	XPtrList* list;
	
	while ( mColumns.FetchLast( (void **)(&list) ) ) {
		delete list;
		mColumns.RemoveLast();
	}
}


XPtrList* XPtrMatrix::operator[]( long inIndex ) {
	long i;
	
	if ( inIndex < 0 )
		inIndex = 0;
		
	if ( inIndex >= mColumns.Count() ) {
		for ( i = inIndex - mColumns.Count(); i >= 0; i-- )
			mColumns.Add( new XPtrList( mRowOrdering ) );
	}
	
	return (XPtrList*) mColumns.Fetch( inIndex + 1 );
}



void XPtrMatrix::Add( long inColumn, void* inPtr ) {
	XPtrList* col = (*this)[ inColumn ];

	col -> Add( inPtr );
}
		
		
void XPtrMatrix::RemoveAll() {
	long i;
	XPtrList* col;
	
	for ( i = 1; mColumns.Fetch( i, (void **)(&col) ); i++ ) {
		col -> RemoveAll();	
	}
}
