#include "LowUnionFind.h"


LowUnionFind::LowUnionFind( long inNumVerticies ) {
	int i;
	
	mNumVerticies = inNumVerticies;
	mAdj = new XLongList[ mNumVerticies ];
	mLargest = &mAdj[ 0 ];
	for ( i = 0; i < mNumVerticies; i++ )
		mAdj[ i ].Add( i );
}



LowUnionFind::~LowUnionFind() {

	delete []mAdj;
}



void LowUnionFind::AddEdge( long inA, long inB ) {
	
	mAdj[ inA ].Add( inB );
	mAdj[ inB ].Add( inA );
	
	if ( mLargest -> Count() < mAdj[ inA ].Count() )
		mLargest = &mAdj[ inA ];
	else if ( mLargest -> Count() < mAdj[ inB ].Count() )
		mLargest = &mAdj[ inB ];
}


void LowUnionFind::RemoveVertex( long inA ) {
	long i, n, max = 0;
	XLongList* largest = &mAdj[ 0 ];
		
	for ( i = 0; i < mNumVerticies; i++ ) {
		mAdj[ i ].Remove( inA );
		n = mAdj[ i ].Count();
		if ( n > max ) {
			largest = &mAdj[ i ];
			max = n;
		}
	}
	
	mLargest = largest;
}




/*

XLongList* LowUnionFind::FindLargestPile() {
	SubTree* tree = (SubTree*) mSubTrees.GetHead();
	long max = tree -> Count(), n;
	SubTree* largest = tree;
	
	tree = (SubTree*) tree -> GetNext();
	
	// For each connected sub graph
	while ( tree ) {

		n = tree -> Count();
		if ( n > max ) {
			largest = tree;
			max = n;
		}

		tree = (SubTree*) tree -> GetNext();
	}
	

	return largest;
}



*/

