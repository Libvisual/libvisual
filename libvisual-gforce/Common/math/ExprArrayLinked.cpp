#include "ExprArrayLinked.h"

#include "Hashtable.h"

void ExprArrayLinked::SetupStrengthGroups( Hashtable& ioVarList ) {
	UtilStr str;
	long strongest, strength, i, pos;
	
	mStrengthGroups.RemoveAll();
	
	// Go thru the expr arry
	for ( i = 0; i < mNumExprs; i++ ) {
		
		// Find the token with the greatest strength/priority
		strongest = 0;
		for ( pos = 0; mExprs[ i ].GetNextToken( str, pos ); ) {
			if ( ioVarList.Get( &str, (void**) &strength ) ) {
				if ( strength > strongest )
					strongest = strength;
			}		
		}
		
		// Add this expr to the var list of appropriate strength
		mStrengthGroups[ strongest ] -> Add( (void*) i );
		
		// Higher expr elements may use this element, so add a dict entry that returns the priority of this expr
		str.Assign( mIDStr );
		str.Append( i );
		ioVarList.Put( new UtilStr( str ), (void*) strongest );
	}
}


void ExprArrayLinked::EvaluateStrength( long inStrength ) {
	XPtrList* col = mStrengthGroups[ inStrength ];
	long i, j, n = col -> Count();
	
	for ( i = 1; i < n; i++ ) {
		j = (long) col -> Fetch( i );
		mVals[ j ] = mExprs[ j ].Evaluate();
	}
}
