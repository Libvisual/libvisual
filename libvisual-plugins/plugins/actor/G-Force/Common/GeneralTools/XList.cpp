#include "XList.h"



XList::XList( nodeClass* inParent ) :
	nodeClass( inParent ) {
	
	mCachedNode = 0;
}




void XList::UpdateCounts( int inShallowChange ) {
	
	nodeClass::UpdateCounts( inShallowChange );
	
	mCachedNode = 0;
}	



nodeClass* XList::findSubNode( long inNum ) {
	nodeClass*		retPtr = 0;
	long			i;
	
	if ( mCachedNode ) {								//	Check cached node...
		i = inNum - mCachedNodeNum;						//	See how far away our cached node is
		
		if ( i > -4 && i < 4 ) {
			retPtr = mCachedNode;
			if ( i < 0 ) {								// 	The cached node was after the desired
				for ( ; i != 0 && retPtr; i++ )			//	Move forward the num that the cache is off
					retPtr = retPtr -> PrevInChain( this );	}
			else {
				for ( ; i != 0 && retPtr; i-- )
					retPtr = retPtr -> NextInChain( this ); 
			}
		}
		
		#ifdef EG_DEBUG
		if ( retPtr )
			sCacheHitsA++;
		sCacheTrysA++;
		#endif
	}			
	
	if ( ! retPtr ) 									// If no cache his was found
		retPtr = nodeClass::findSubNode( inNum );		// Do it by brute force
	
	if ( retPtr ) {
		mCachedNodeNum	= inNum;
		mCachedNode		= retPtr;
	}
	
	return retPtr;
}



long XList::findSubNode( nodeClass* inNodePtr ) {
	nodeClass*		nodePtr;
	long			n = 0;
	
	if ( mCachedNode && inNodePtr ) {					// If we have a cache available
		if ( mCachedNode == inNodePtr )					//	Check trivial cache case
			n = mCachedNodeNum;
			
		if ( n == 0 ) {									// If no hit yet...
			nodePtr = mCachedNode -> NextInChain( this );
			if ( nodePtr == inNodePtr )					// Check node after cached node
				n = mCachedNodeNum + 1;
		}
		
		if ( n == 0 ) {									// If no hit yet...
			nodePtr = mCachedNode -> PrevInChain( this );
			if ( nodePtr == inNodePtr )					// Check node before cached node
				n = mCachedNodeNum - 1;
		}
		
		#ifdef EG_DEBUG
		if ( n > 0 )
			sCacheHitsB++;
		sCacheTrysB++;
		#endif
	}
	
	if ( n == 0 ) 										// If no cache hit...
		n = nodeClass::findSubNode( inNodePtr );		// Do it with brute force.
	
	if ( n > 0 ) {										// Update the cache if possible...
		mCachedNode		= inNodePtr;
		mCachedNodeNum	= n;
	}
		
	return n;
}



long XList::GetParent( long inCellNum ) {
	nodeClass* nodePtr = findSubNode( inCellNum );
	
	if ( nodePtr ) {
		if ( nodePtr -> GetParent() == this )
			return 0;
		else
			return findSubNode( nodePtr -> GetParent() ); }
	else
		return -1;
}


		
XLongList* XList::ManageClick( long inCell, bool inShift, bool inCmd, bool inDeselectRest ) {
	XLongList*	retPtr	= ListSocket::ManageClick( inCell, inShift, inCmd, inDeselectRest );
	nodeClass*	nodePtr	= GetDeepTail();
	long		i		= deepCount();
	
	while ( nodePtr ) {
		if ( nodePtr -> IsSelected() ) {
			SetInsertionPt( i, nodePtr -> CountDepth( this ) - 1 );
			return retPtr;
		}
		
		i--;
		nodePtr = nodePtr -> PrevInChain( this );
	}
	
	return retPtr;
}



void XList::DoBestInsert( nodeClass* inNodeToAdd ) {
	nodeClass*	insertPt = findSubNode( mInsertionPt );
	long		insDepth;
	
	if ( inNodeToAdd ) {
		if ( insertPt ) {
			insDepth = insertPt -> CountDepth( this ) - 1;
			while ( insDepth > mInsertionDepth && insertPt ) {
				insertPt = insertPt -> GetParent();
				insDepth--;
				
				if ( insertPt == this )
					insertPt = 0;
			}
		}
		
		if ( insertPt ) {	
			if ( insDepth == mInsertionDepth )
				inNodeToAdd -> insertAfter( insertPt );
			else //  depth < inDepth
				insertPt -> addToHead( inNodeToAdd ); }
		else {
			if ( mInsertionPt <= 0 ) 
				addToHead( inNodeToAdd );
			else 
				addToTail( inNodeToAdd );
		}
		
		mInsertionPt	= findSubNode( inNodeToAdd );
		mInsertionDepth	= inNodeToAdd -> CountDepth( this ) - 1;
	}
}


void XList::SetInsertPtBefore( nodeClass* inInsertPt ) {
	long n = findSubNode( inInsertPt );

	if ( n <= 0 )
		inInsertPt = GetDeepTail();
	else if ( n == 1 ) 
		inInsertPt = 0;
	else 
		inInsertPt = inInsertPt -> PrevInChain( this ); 
					
	if ( inInsertPt )
		SetInsertionPt( findSubNode( inInsertPt ), inInsertPt -> CountDepth( this ) - 1 );
	else
		SetInsertionPt( 0, 0 );

}



void XList::SetSelected( long inElementNum, bool isSelected ) {
	nodeClass*	nodePtr = findSubNode( inElementNum );

	if ( nodePtr )
		nodePtr -> SetSelected( isSelected );
}


bool XList::IsSelected( long inElementNum ) {
	nodeClass*	nodePtr = findSubNode( inElementNum );
		
	if ( nodePtr )
		return nodePtr -> IsSelected();
	else
		return false;
}



long XList::NumCells() {
	return deepCount();
}
