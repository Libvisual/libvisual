#ifndef LowUnionFind_H
#define LowUnionFind_H


#include "nodeClass.h"
#include "XLongList.h"

class SubTree : public nodeClass, public XLongList {

};


/* 
"Low" in contrast to HighUnionFind in that running time for Union() is O( N ), so 
performace will drop as the number of nodes starts to get very high.
*/

class LowUnionFind {

	public:
								LowUnionFind( long inNumVerticies );
								~LowUnionFind();
									
		// 	Note: O( N )
		void					AddEdge( long inA, long inB );
		
		//	Returns access to the largest union/subtree in this graph. 0 is returned if there's
		//  no verticies in this graph.
	 	const XLongList* 		LargestPile()					{ return mLargest;			}
	 	
	 	
	 	//	Removes the vertex from all given piles.
	 	void					RemoveVertex( long inA );
	 	
	 	
	 protected:
	 	XLongList*				mAdj;
	 	long					mNumVerticies;
	 	XLongList*				mLargest;						// Always pts to the largest node/group/pile
	 	
	 	XLongList*				FindLargestPile();
};

#endif
