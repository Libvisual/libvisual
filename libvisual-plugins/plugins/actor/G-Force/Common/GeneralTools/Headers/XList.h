#ifndef _XLIST_
#define _XLIST_

#include "nodeClass.h"
#include "ListSocket.h"





class XList : protected nodeClass, public ListSocket {

		
		
	public:
									XList( nodeClass* inParent );
									
		//	Post:	This inserts <inNodeToAdd> within this node, after the current insertion point (ie, at mInsertionPt and mInsertionDepth).
		//	Note:	0 for a depth means the root level.
		//	Note:	The insertion pt is modified/maintained to follow after <inNodeToAdd> was placed
		void						DoBestInsert( nodeClass* inNodeToAdd );

		//	Post:	Sets the current insertion point before the node <inInsertPt>.
		//	Note:	If <inInsertPt> is 0, the insert pt is at the end of the list
		void						SetInsertPtBefore( nodeClass* inInsertPt );
		
		//	*** nodeClass overrides--see nodeClass.h for fcn docs ***
		//	Post:	Same as nodeClass::findSubNode() except that caching brings running time to O(1) (vs. O(n)) if
		//			the previous call was an adjacent node
		virtual nodeClass*			findSubNode( long inNodeNum );
					
		//	Post:	Same as nodeClass::findSubNode() except that caching brings running time to O(1) (vs. O(n)) if
		//			the previous call was an adjacent node
		virtual long				findSubNode( nodeClass* inNodePtr );

		//	Post:	Returns the cell number that <inCellNum> is hierarcharaly inside (similar to nodeClass's GetParent()).
		//	Note:	If the specified cell is at the root level (ie it has no real visible parent) then 0 is returned.
		//	Note:	If the specified cell does not exist, -1 is returned.
		long						GetParent( long inCellNum );

		//	*** ListSocket overrides--see ListSocket.h for fcn docs ***
		virtual bool				CheckInsertPt( long& ioNodeNum, long& ioDepth )								{ return nodeClass::CheckInsertPt( ioNodeNum, ioDepth );		}
		virtual void				MoveSelected( long inAboveCell, long inDepth )								{ nodeClass::MoveSelected( inAboveCell, inDepth );	}
		virtual void				DeleteSelected()															{ nodeClass::DeleteSelected();					}
		virtual XLongList*			ManageClick( long inCell, bool inShift, bool inCmd, bool inDeselectRest );
		virtual void				SetSelected( long inElementNum, bool isSelected );
		virtual bool				IsSelected( long inElementNum );
		virtual long				NumCells();

		
	protected:
		virtual void				UpdateCounts( int inShallowChange );

		
		nodeClass*					mCachedNode;		//	0 if Dirty
		long						mCachedNodeNum;		//	Deep instance of mCachedNode

};

#endif
