#ifndef _LISTSOCKET_
#define _LISTSOCKET_


class UtilStr;
class XLongList;
class Clipboard;

class ListSocket {
	
	public:
		
		//	Post:	Mirrors a GUI drag of the selected items dropped before cell <inAboveCell> at depth <inDepth>
		//	Note:	If <inDepth> is invalid or <inAboveCell> < 0 or > N, the closest boundry is used.
		virtual void				MoveSelected( long inAboveCell, long inDepth );
		
		//	Post:	Used by a GUI to refine a given insertion point.
		//	Post:	<ioNodeNum> and <ioDepth> may be adjusted/corrected.
		//	Note:	Depths are such that 0 means root level.
		//	Note:	Fcn succesful iff <true> is returned.
		virtual bool				CheckInsertPt( long& ioNodeNum, long& ioDepth );
		
		//	Post:	Used to select cells.  DeleteSelected() and MoveSelected() use cell selections.
		virtual void				SetSelected( long inCellNum, bool isSelected ) = 0;
		virtual long				NumSelected();
		virtual bool				IsSelected( long inCellNum ) = 0;
		virtual void				SetSelectAll( bool inAreSelected );
		inline void					UnselectAll()								{ SetSelectAll( false );	}
		inline void					SelectAll()									{ SetSelectAll( true );		}
		
		//	Post:	Used to enable/disable cells.
		virtual void				SetEnabled( long inCellNum, bool isSelected );
		virtual bool				IsEnabled( long inCellNum );
		virtual void				EnableAll();
		
		//	Post:	Returns the first/last cell that's selected.
		//	Note:	0 is returned if no cell is selected
		long						GetFirstSelected();
		long						GetLastSelected();
		
		//	Post:	Permanently deletes the cells from the list.
		virtual void				DeleteSelected();
		
		//	Post:	Retruns info about the cell <inCellNum>.
		//	Note:	Returns true if <inCellNum> specifies a valid cell (and valid info is sent back).
		//	Note:	Depths are such that 0 means root level, etc.
		virtual bool				GetCell( long inCellNum, long& outIcnID, long& outDepth, UtilStr& outLabel ) = 0;
		
		//	Post:	Returns the number of cells in the list
		virtual long				NumCells() = 0;
		
		//	Post:	Stub fcn for ListSocket.  Overridde if your table needs this functionality.
		//	Note:	depth == 0 means root level.
		virtual void				SetInsertionPt( long inCellNum, long inDepth );

		//	Post:	Universally manages a click on the <inCell>th cell, using the current insertion
		//			point as the anchor if nessecary to select cells.
		//	Note:	It returns a list of cell numbers that need refreshing (ie, their select state changed).
		virtual XLongList*			ManageClick( long inCell, bool inShift, bool inCmd, bool inDeselectRest );
		
		//	Post:	Moves the selection location <inCellDelta> cells forward.  This fcn is useful for handling
		//			arrow keys in a list for example.
		//	Note:	If there is nothing selected, the first cell will be selected.
		//	Note:	It returns a list of cell numbers that need refreshing (ie, their select state changed).	
		XLongList*					ArrowSelection( int inCellDelta );
		
		//	Post:	Returns the cell number associated with the user typing in <inGUINum>.  In otherwords, this
		//			fcn maps the cell labeling to the cell number.
		virtual long				GUINumToCellNum( long inGUINum );

		//	Used to see if a given clipboard opertion can be performed (ie, if the menu item can be enabled).
		virtual bool				CanClear()												{ return false;		}
		virtual bool				CanCutToClip();
		virtual bool				CanCopyToClip();
		virtual bool				CanPasteFromClip( Clipboard& inClip );
			
		//	Performs the standard clipboard operations.
		virtual void				Clear();
		virtual void				CutToClip( Clipboard& outClip );
		virtual void				CopyToClip( Clipboard& outClip );
		virtual void				PasteFromClip( Clipboard& inClip );


	protected:
		long						mInsertionPt;
		long						mInsertionDepth;
		


};



#endif