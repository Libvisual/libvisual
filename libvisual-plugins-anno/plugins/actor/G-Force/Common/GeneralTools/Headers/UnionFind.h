#ifndef _UnionFind_
#define _UnionFind_

/* 
An implementation of the famous union-find algorithm.
*/

class XLongList;


class UnionFind {

	
	public:
							UnionFind();
		virtual				~UnionFind();
			
		// Helps UnionFind make a good size ahead of time
		void				Dim( long inNum )				{ Find( inNum - 1 );	}
		
		void				Union( long inA, long inB );
	
		long				Find( long inA );
		
		long				LargestSet( long* outSize );
		
		void				EnumerateSet( long inSetID, XLongList& outSet );
						
		long				NumSets();
		
		void				GetSets( XLongList& outSets );
		
	protected:
		long				mDimSize;
		unsigned short*		mElements;
	
};



#endif