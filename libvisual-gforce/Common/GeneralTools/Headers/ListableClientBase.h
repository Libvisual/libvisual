#ifndef _ListableClientBase_
#define _ListableClientBase_

#include "Messager.h"

class ListSocket;
class XLongList;
class Clipboard;

class ListableClientBase : public Messager {



	public:
									ListableClientBase( long inCellHeight, ListSocket* inListToUse );
								
		//	Post:	Redraws the cells specified by <inRefreshList>							
		virtual void				RedrawCells( XLongList& inRefreshList );
		
		//	Post:	Scrolls vertically to bring the specified cell into view.
		virtual void				ScrollToCell( long inCellNum );

		//	Post:	Scrolls vertically to bring the selection into view.
		virtual void				ScrollToSelected();
		
		//	Post:	Returns the index of the first selected cell.
		//	Note:	If no cells are selected, 0 is returned.
		long						GetFirstSelected();

		//	Post:	Returns the index of the last selected cell.
		//	Note:	If no cells are selected, 0 is returned.
		long						GetLastSelected();
				
		//	Post:	Returns the cell range currently visible.
		void						GetVisibleRange( long& outStart, long& outEnd );

		//	Post:	This fcn should peform a generic export of its particular drag data to <outClip>.
		virtual void				SendDrag( Clipboard& outClip );

		virtual void				ClickSelf( long inX, long inY, bool inDblClick, bool inModKeys );

		virtual void				ClickCellSelf( long inCell, bool inDblClick );


		void						RedrawCell( long inCellNum );
		
		virtual bool				FindItemStatus( long inItemID, bool& ioChecked );
		virtual bool				ListenToMsg( long inCmdID, long inArgs[] );
				
		//	Callbacks or visual updating
		virtual void				NumCellsChanged() = 0;
		virtual void				NeedsRedraw() = 0;
				
	protected:
		ListSocket*					mList;
		long						mCellHeight;
		long						mCellWidth;
		long						mRows;
		long						mCols;
		bool						mHasGridLines;
		bool						mRightJustified;		// If cell text is right justified
		long						mDepthIndent;
		long						mListIndent;
		long						mTextIconDist;
		
		
		//	Post:	Returns the number of pixels the given cell is out of view.  If it's negative
		//			the cell lies above, if positive, it's below, and zero means it's already in view.
		long						YDeltaOfCell( long inCellNum );
		
		
		virtual void				CalcSize( long& ioWidth, long& ioHeight );
		
		virtual void				HandleKeyPress( unsigned char c );
		
		virtual long				GetCellHit( long inX, long inY );

		//	Scroller callback
		virtual void				ScrollBy( long inDeltaX, long inDeltaY ) = 0;

		//	Callbacks before and after drawing
		virtual void				LCBeginDraw() = 0;
		virtual void				EndDraw() = 0;
		
		//	Post:	Returns the cell range currently visible to the user (ie, the range worth drawing).
		virtual void				GetVisibleRect( long& outLeft, long& outTop, long& outRight, long& outBottom ) = 0;

		//	Low level callback to draw one cell
		virtual void				DrawCell( long inCellNum, bool inDrawBackground ) = 0;
		
		virtual void				RedrawRect( long inLeft, long inTop, long inBottom, long inRight );

		static long					sLastKeyTime;
	
	
		enum {
			cDefaultTextIconDist	= 5,
			cDefaultListIndent		= 5,
			cDefualtDepthIndent		= 15
		};
};


#endif