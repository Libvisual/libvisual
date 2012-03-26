#include <limits.h>
#include "XPtrList.h"
#include "nodeClass.h"


void* XPtrList::sDummy = 0;


XPtrList::XPtrList( ListOrderingT inOrdering ) {

	mOrdering	= inOrdering;
	mCompFcn	= 0;
}




void XPtrList::Assign( const XPtrList& inList ) {

	UtilStr::Assign( inList );
}



#define __ptr( idx )   *((void**) (base + idx * sizeof(void*)))

long XPtrList::FetchPredIndex( const void* inPtr ) const {
	long M, L = 0, R = Count()-1;
	char* base = getCStr();
	int order = ( mOrdering == cSortHighToLow ) ? INT_MIN : 0;

	if ( R < 0 )
		return 0;
	else {
		while (L <= R) {

			M = (L + R) / 2;

			if ( (mCompFcn( inPtr, __ptr( M ) ) ^ order) >= 0 ) // If inPtr <= __ptr( M )...
				R = M - 1;										// Throw away right half
			else
				L = M + 1;										// Throw away left half
		}

		if ( L > R )											// Catch the case where R+1==L
			L = M;												// In this case, M specifies the critical element

		// At this point, we know L is the critical element (case: L==R or L contains M from case above)
		if ( mCompFcn( inPtr, __ptr( L ) ) < 0 )				// If inPtr > __ptr( M )...
			L++;

		return L;
	}
}


void XPtrList::SetCompFcn( CompFunctionT inFcn, bool inSortLowToHigh ) {
	mCompFcn = inFcn;

	RemoveAll();

	if ( inSortLowToHigh )
		mOrdering = cSortLowToHigh;
	else
		mOrdering = cSortHighToLow;

}



long XPtrList::FindIndexOf( const void* inMatch ) const {
	long	i = 0;
	char*	curPtr, *endPtr;
	void*	ptr;

	if ( mCompFcn ) {
		i = FetchPredIndex( inMatch );
		curPtr = getCStr() + sizeof(void*) * i;
		endPtr = getCStr() + length();
		while ( curPtr < endPtr ) {
			i++;
			ptr = *((void**) curPtr);
			if ( ptr == inMatch )
				return i;

			// Stop checking when we hit items that aren't equal to inMatch
			else if ( mCompFcn( inMatch, ptr ) != 0 )
				break;
			curPtr += sizeof(void*);
		} }
	else {
		curPtr = getCStr();
		endPtr = curPtr + length();

		while ( curPtr < endPtr ) {
			i++;
			if ( *((void**) curPtr) == inMatch )
				return i;
			else
				curPtr += sizeof(void*);
		}
	}

	return 0;
}




long XPtrList::Add( const void* inPtrToAdd ) {
	long i;

	if ( mCompFcn ) {
		i = FetchPredIndex( inPtrToAdd );
		Insert( i*sizeof(void*), (char*) &inPtrToAdd, sizeof(void*) );
		return i+1; }
	else {
		UtilStr::Append( (char*) &inPtrToAdd, sizeof(void*) );
		return Count();
	}
}





void XPtrList::Add( const void* inPtrToAdd, long inN ) {

	if ( inN < 0 )
		inN = 0;

	if ( inN > Count() )
		inN = Count();

	Insert( inN * sizeof(void*), (char*) &inPtrToAdd, sizeof(void*) );
}




void XPtrList::Add( const XPtrList& inList ) {

	if ( mOrdering == cOrderNotImportant )
		UtilStr::Append( inList );
	else {
		int i, n = inList.Count();
		for ( i = 1; i <= n; i++ )
			Add( inList.Fetch( i ) );
	}
}




void*& XPtrList::operator[] ( long inIndex ) {
	if ( inIndex >= 0 ) {
		long len = mStrLen;
		if ( inIndex >= len / long (sizeof(void*)) ) {
			Insert( len, '\0', ( inIndex + 1 ) * sizeof(void*) - len );
		}

		return *( (void**) ( mBuf + inIndex * sizeof(void*) + 1 ) ); }
	else
		return sDummy;
}





bool XPtrList::Remove( const void* inMatchPtr ) {
	long	idx = FindIndexOf( inMatchPtr );

	return RemoveElement( idx );
}



bool XPtrList::RemoveElement( long inIndex ) {
	char* s;

	if ( inIndex > 0 && inIndex <= Count() ) {
		inIndex--;
		if ( mOrdering == cOrderNotImportant ) {
			s = getCStr();
			*( (void**) (s + inIndex * sizeof(void*)) ) = *( (void**) (s + length() - sizeof(void*) ) );
			Trunc( sizeof(void*) ); }
		else
			UtilStr::Remove( inIndex * sizeof(void*) + 1, sizeof(void*) );
		return true; }
	else
		return false;
}




bool XPtrList::RemoveLast() {

	if ( length() > 0 ) {
		Trunc( sizeof(void*) );
		return true; }
	else
		return false;
}


void XPtrList::RemoveAll() {
	Wipe();
}




void XPtrList::MoveToHead( long inIndex ) {
	void* p;
	char* s;

	if ( inIndex > 1 ) {
		if ( Fetch( inIndex, &p ) ) {
			inIndex--;
			s = getCStr();
			if ( mOrdering == cOrderNotImportant )
				*( (void**) (s + inIndex * sizeof(void*)) ) = *( (void**) s);
			else
				UtilStr::Move( s+sizeof(void*), s, inIndex * sizeof(void*) );
			*( (void**) s) = p;
		}
	}
}




void* XPtrList::Fetch( long inIndex ) const {
	if ( inIndex >= 1 && inIndex <= long (length() / sizeof(void*)) )
		return *( (void**) (getCStr() + ( inIndex - 1 ) * sizeof(void*)) );
	else
		return 0;
}


bool XPtrList::Fetch( long inIndex, void** ioPtrDest ) const {

	if ( ioPtrDest ) {
		if ( inIndex >= 1 && inIndex <= long (length() / sizeof(void*)) ) {
			*ioPtrDest = *( (void**) (getCStr() + ( inIndex - 1 ) * sizeof(void*)) );
			return true; }
		else
			*ioPtrDest = 0;
	}

	return false;
}






#include <stdio.h>
void XPtrList::Randomize() {
	void*	temp, **ptrArray = (void**) getCStr();
	long	i, randIdx, n = Count();

	for ( i = 0; i < n; i++ ) {
		randIdx = nodeClass::Rnd( 1, n );
		temp = ptrArray[ i ];
		ptrArray[ i ] = ptrArray[ randIdx-1 ];
		ptrArray[ randIdx-1 ] = temp;
	}
}

