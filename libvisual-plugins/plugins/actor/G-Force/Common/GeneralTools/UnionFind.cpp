#include "UnionFind.h"

#include "XLongList.h"


UnionFind::UnionFind() {
	mDimSize = 0;
	mElements = NULL;
}

UnionFind::~UnionFind() {
	if ( mElements )
		delete mElements;
}


long UnionFind::Find( long inA ) {
	long i, newSize, next;
	unsigned short* newmem;

	// Expand our list of elements if we need to...		
	if ( inA >= mDimSize ) {

		newSize = inA + 20;
		newmem = new unsigned short[ newSize + 1 ];
		for ( i = 0; i < mDimSize; i++ )
			newmem[ i ] = mElements[ i ];
		delete mElements;
		for ( i = mDimSize; i < newSize; i++ )
			newmem[ i ] = i;
		mDimSize = newSize;
		mElements = newmem;
	}



	// Find w/ path compression
	while ( mElements[ inA ] != inA ) {
		next = mElements[ inA ];
		if ( mElements[ next ] == next )
			return next;
		else {
			inA = mElements[ inA ] = mElements[ next ];
		}
	}
	
	
	return inA;
}


long UnionFind::LargestSet( long* outSize ) {
	unsigned short* tally = new unsigned short[ mDimSize ];
	long max, set, i;
	
	
	for ( i = 0; i < mDimSize; i++ )
		tally[ i ] = 0;
		
	for ( i = 0; i < mDimSize; i++ )
		tally[ Find( i ) ]++;
		
	max = 0;
	set = 0;
	for ( i = 0; i < mDimSize; i++ ) {
		if ( tally[ i ] > max ) {
			max = tally[ i ];
			set = i;
		}
	}
	
	delete []tally;
	
	if ( outSize )
		*outSize = max;
		
	return set;
}


void UnionFind::EnumerateSet( long inSetID, XLongList& outSet ) {
	long i;
	
	outSet.RemoveAll();
	for ( i = 0; i < mDimSize; i++ ) {
		if ( Find( i ) == inSetID )
			outSet.Add( i );
	}
}


void UnionFind::Union( long inA, long inB ) {
	
	// Do the union
	mElements[ Find( inB ) ] = Find( inA );
}






long UnionFind::NumSets() {
	long i, numSets = 0;
	
	for ( i = 0; i < mDimSize; i++ ) {
		if ( mElements[ i ] == i )
			numSets++;
	}
	
	return numSets;
}



void UnionFind::GetSets( XLongList& outSets ) {
	long i;
	
	outSets.RemoveAll();
	for ( i = 0; i < mDimSize; i++ ) {
		if ( mElements[ i ] == i )
			outSets.Add( i );
	}
}

