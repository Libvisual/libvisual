#include "ListableClientBase.h"

//#include "Eg_Messages.h"
#include "ListSocket.h"
#include "XLongList.h"
#include "Clipboard.h"
#include <time.h>


long ListableClientBase::sLastKeyTime = 0;


ListableClientBase::ListableClientBase( long inCellHeight, ListSocket* inListToUse ) {
	
	mCellHeight		= inCellHeight;
	mCellWidth		= 1;
	mList			= inListToUse;
	mListIndent		= cDefaultListIndent;
	mDepthIndent	= cDefualtDepthIndent;
	mTextIconDist	= cDefaultTextIconDist;
	mRows 			= mList ? mList -> NumCells() : 0;
	mCols			= 1;
	mHasGridLines	= false;
	mRightJustified	= false;
}




void ListableClientBase::RedrawRect( long inLeft, long inTop, long inBottom, long inRight ) {
	int	i, n, row, endRow, endCol, col;

	endRow	= inBottom / mCellHeight;
	endCol	= inRight / mCellWidth;
	col		= inLeft / mCellWidth;
	row		= inTop / mCellHeight;
	n		= row * mCols + 1;
	
	if ( endCol >= mCols )
		endCol = mCols - 1;
		
	for ( ; row <= endRow; row++, n += mCols ) {
		for ( i = col; i <= endCol; i++ ) {
			DrawCell( n + i, false );
		}
	}
}



void ListableClientBase::RedrawCells( XLongList& inRefreshList ) {
	long cell, i, start, end;
	bool drew = false;

	GetVisibleRange( start, end );
	
	for ( i = 1; inRefreshList.Fetch( i, &cell ); i++ ) {
		if ( cell >= start && cell <= end ) {
			if ( ! drew ) {
				drew = true;
				LCBeginDraw();
			}
			DrawCell( cell, true );
		}
	}
	
	if ( drew ) 
		EndDraw();
}





void ListableClientBase::RedrawCell( long inCellNum ) {
	LCBeginDraw();
	
	DrawCell( inCellNum, true );
	
	EndDraw();
}



long ListableClientBase::YDeltaOfCell( long inCellNum ) {
	long	dummy, sy, ey;
	long 	cellBottom, cellTop;
		
	if ( inCellNum < 1 )
		return 0;
		
	GetVisibleRect( dummy, sy, dummy, ey );
	
	cellTop		= mCellHeight * ( (inCellNum - 1) / mCols );
	cellBottom	= cellTop + mCellHeight;

	if ( cellTop < sy ) 
		return cellTop - sy;
	else if ( cellBottom > ey )
		return cellBottom - ey;
	else
		return 0;
}





void ListableClientBase::ScrollToCell( long inCellNum ) {
	long yDelta = YDeltaOfCell( inCellNum );

	ScrollBy( 0, yDelta );
}





void ListableClientBase::ScrollToSelected() {
	long	yDelta;
	
	if ( mList ) {
			
		yDelta = YDeltaOfCell( mList -> GetFirstSelected() );
		
		if ( yDelta < 0 )
			ScrollBy( 0, yDelta );
		else {
			yDelta = YDeltaOfCell( mList -> GetLastSelected() );
			if ( yDelta > 0 )
				ScrollBy( 0, yDelta );
		}
	}
}










long ListableClientBase::GetFirstSelected() {
	if ( mList )
		return mList -> GetFirstSelected();
	else
		return 0;
}



long ListableClientBase::GetLastSelected() {
	if ( mList )
		return mList -> GetLastSelected();
	else
		return 0;
}


void ListableClientBase::GetVisibleRange( long& outStart, long& outEnd ) {
	long n, sy, ey;
		
	GetVisibleRect( n, sy, n, ey );
	
	outStart	= 1 + mCols * (sy / mCellHeight);
	outEnd 		= mCols * (1 + ey / mCellHeight);
	
	n = mList ? mList -> NumCells() : 0;
	if ( outEnd > n )
		outEnd = n;
		
	if ( outStart > n )
		outStart = n;
}






long ListableClientBase::GetCellHit( long inX, long inY ) {
	long col, n;

	col = inX / mCellWidth + 1;
	
	if ( col > mCols || inY <= 0 )
		n = 0;
	else
		n = ( inY / mCellHeight ) * mCols + col;
	
	return n;
}







bool ListableClientBase::FindItemStatus( long inItemID, bool& ) {
	bool outEnabled = false;
	
	switch ( inItemID ) {

		case cClear_ID:
			if ( mList )
				outEnabled = mList -> CanClear();
			break;
							
		case cCut_ID:
			if ( mList )
				outEnabled = mList -> CanCutToClip();
			break;
			
		case cCopy_ID:
			if ( mList )
				outEnabled = mList -> CanCopyToClip();
			break;

		case cPaste_ID:
			if ( mList )
				outEnabled = mList -> CanPasteFromClip( Clipboard::sAppClipboard );
			break;
						
		case cSelectAll_ID:
			outEnabled = true;		
			break;
	}
	
	return outEnabled;
}









void ListableClientBase::HandleKeyPress( unsigned char c ) {
	static UtilStr		sRecentStr;
	long				n, t = clock();
	XLongList*			refreshList = 0;

	if ( c >= '0' && c <= '9' ) {
		if ( t - 2*CLOCKS_PER_SEC >= sLastKeyTime )
			sRecentStr.Wipe();
		sLastKeyTime = t;
		sRecentStr.Append( (char) c );
		n = sRecentStr.GetValue();
		if ( mList )
			n = mList -> GUINumToCellNum( n );
		refreshList = mList -> ManageClick( n, false, false, true );  
		if ( refreshList )
			RedrawCells( *refreshList );
		ClickCellSelf( n, false );
		ScrollToSelected(); 
	}
	else 
		sLastKeyTime = t - 2*CLOCKS_PER_SEC;
}








bool ListableClientBase::ListenToMsg( long inCmdID, long inArgs[] ) {
	
	switch ( inCmdID ) {

		case cClear_ID:
			if ( mList ) {
				if ( mList -> CanClear() ) {
					mList -> Clear();
					NumCellsChanged();
				}
			}
			break;
			
		case cCopy_ID:
			if ( mList ) {
				if ( mList -> CanCopyToClip() ) 
					mList -> CopyToClip( Clipboard::sAppClipboard );
			}
			break;

		case cPaste_ID:
			if ( mList ) {
				if ( mList -> CanPasteFromClip( Clipboard::sAppClipboard ) ) {
					mList -> PasteFromClip( Clipboard::sAppClipboard );
					NumCellsChanged();
				}
			}
			break;
			
		case cCut_ID:
			if ( mList ) {
				if ( mList -> CanCutToClip() ) {
					mList -> CutToClip( Clipboard::sAppClipboard );
					NumCellsChanged();
				}
			}
			break;
			
		case cSelectAll_ID:
			if ( mList ) {
				mList -> SelectAll();
				NeedsRedraw();
			}
			break;
			
		case cEnter_ID:
			sLastKeyTime = clock() - 2*CLOCKS_PER_SEC;
			
		default:
			return Messager::ListenToMsg( inCmdID, inArgs );	
	}
		
	return true;
}	










void ListableClientBase::CalcSize( long& ioWidth, long& ioHeight ) {

	mCellWidth = ioWidth + 5;
	
	if ( mList )
		mRows = mList -> NumCells();
	else
		mRows = 0;
		
	ioHeight = mRows * mCellHeight + 1;
}






void ListableClientBase::SendDrag( Clipboard& outClip ) {

	if ( mList )
		mList -> CopyToClip( outClip );
	else
		outClip.Clear();
}	




void ListableClientBase::ClickSelf( long inX, long inY, bool inDblClick, bool inModKeys ) {
	XLongList* refreshList;
	int n = GetCellHit( inX, inY );
	
	if ( mList ) {
		if ( ! inModKeys ) {
			refreshList = mList -> ManageClick( n, false, false, true );
			if ( refreshList )
				RedrawCells( *refreshList );
		}
		
		if ( n > 0 )
			ClickCellSelf( n, inDblClick );
	}
}



void ListableClientBase::ClickCellSelf( long inCell, bool inDblClick ) {
	long args[2] = { inCell, inDblClick };
	
	BroadcastMessage( cListClientClick, args );
}

