#include "ListSocket.h"

#include "XLongList.h"


void ListSocket::MoveSelected( long, long ) {
	
}
		



bool ListSocket::CheckInsertPt( long&, long& ) {
	return false;

}
		

void ListSocket::DeleteSelected() {

}


void ListSocket::SetInsertionPt( long inCellNum, long inDepth ) {

	mInsertionPt		= inCellNum;
	mInsertionDepth		= inDepth;
}



long ListSocket::GetFirstSelected() {
	long i, stop = NumCells();
	
	for ( i = 1; i <= stop; i++ ) {
		if ( IsSelected( i ) )
			return i;
	}
	
	return 0;
}



long ListSocket::GetLastSelected() {
	long i;
	
	for ( i = NumCells(); i > 0; i-- ) {
		if ( IsSelected( i ) )
			return i;
	}
	
	return 0;
}

 

long ListSocket::NumSelected() {
	long	i, numSelected = 0;
	
	for ( i = NumCells(); i > 0; i-- ) 
		if ( IsSelected( i ) )
			numSelected++;
			
			
	return numSelected;
}








void ListSocket::SetSelectAll( bool inAreSelected ) {
	int i, stop = NumCells();
	
	for ( i = 1; i <= stop; i++ ) {
		SetSelected( i, inAreSelected );
	}

}
		

void ListSocket::SetEnabled( long, bool ) {

	
}

bool ListSocket::IsEnabled( long ) {
	return true;
}




void ListSocket::EnableAll() {
	int i, stop = NumCells();
	
	for ( i = 1; i <= stop; i++ ) {
		SetEnabled( i, true );
	}

}












XLongList* ListSocket::ManageClick( long inCell, bool inShift, bool inCmd, bool inDeselectRest ) {
	static XLongList refreshList;
	int i, e;
	

	refreshList.RemoveAll();
	

	if ( inShift ) {
		if ( mInsertionPt < inCell ) {
			i = mInsertionPt;
			e = inCell;  }
		else {
			e = mInsertionPt;
			i = inCell;
		}
		for ( ; i <= e; i++ ) {
			if ( !  IsSelected( i ) ) {
				SetSelected( i, true );
				refreshList.Add( i );
			}
		} }
	else {
		refreshList.Add( inCell );
		
		if ( inCmd ) 
			SetSelected( inCell, ! IsSelected( inCell ) ); 
		else if (  ! IsSelected( inCell ) || inDeselectRest ) {
			for ( i = NumCells(); i > 0; i-- ) {
				if ( i == inCell ) 
					SetSelected( i, true );
				else if ( IsSelected( i ) ) {
					SetSelected( i, false );
					refreshList.Add( i );
				}
			}
		}
	}
	
	SetInsertionPt( inCell, 0 );
	
	return &refreshList;
}
	



XLongList* ListSocket::ArrowSelection( int inCellDelta ) {
	long 	n		= GetFirstSelected() + inCellDelta;
	long 	max		= NumCells();
	
	if ( n < 1 )
		n = 1;
		
	if ( n > max )
		n = max;
		
	return ManageClick( n, false, false, false );
}

	
	
	
long ListSocket::GUINumToCellNum( long inGUINum ) {
	return inGUINum;
}




	
bool ListSocket::CanCutToClip() {
	if ( CanClear() )
		return CanCopyToClip();
	else
		return false;
}


bool ListSocket::CanPasteFromClip( Clipboard& ) { 
	return false;		
}


bool ListSocket::CanCopyToClip() {
	return GetFirstSelected() > 0;
}


	
void ListSocket::Clear() {
	DeleteSelected();
}


	
void ListSocket::CutToClip( Clipboard& outClip ) {
	CopyToClip( outClip );
	Clear();
}


void ListSocket::CopyToClip( Clipboard& ) {

}



void ListSocket::PasteFromClip( Clipboard& ) {

}


