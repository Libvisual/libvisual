#ifndef _Printable_
#define _Printable_


class ZafPrinter;


class Printable {


	public:
		//  inOption == 0: 	Std print	(print setup dialog)
		//	inOption == 1:	Def Print	(uses most recent record from JobSetup() )
		//	inOption == 2:	Print One 	(no print setup and just print one)
		virtual bool					Print( char* inJobName, int inOption );
		
		//	Post:	Returns the hDC (Win32) of the printer.
		static void*					GetPrintDevice();
		
		//	Post:	Returns the length in pages needed to print.
		virtual long					GetNumPages() = 0;
		
		//	Post:	This must be called at the end/exit of the program.
		static void						Shutdown();
		
		//	Util to bring up the page setup dialog box.
		static bool						PageSetup();
		
		//	Post:	Returns the dimentions, in points, of the current paper size
		static void						GetPagePointSize( long& outWidth, long& outHeight );
		
		//	Post:	Brings up the print job setup dialog and uses that iff inOption == 1 in Print()
		//	Note:	Returns <true> if user doesn't cancel the dialog box.
		static bool						JobSetup();

		//	Post:	This draws itself to the current device
		virtual void					DrawPage( long inPageNum, long inX, long inY ) = 0;

		//	Post:	Routes DrawPage output to <inDevice>
		//	Note:	If on win32 <inDevice> is a hDC, and on MacOS it's a GrafPortPtr. 
		virtual void					SetPrintDevice( void* inDevice );	
		
		//	Post:	Releases all ties to the current print device
		virtual void					CloseOutputPort();
		
		static void						InvalidePRec()  { sJobIsSetup = false; }
		
					
	protected:
		void*							mCurPrintDevice;
		static char						sJobRecord[ 500 ];
	

	private:
		static ZafPrinter*				sZafPrinter;
		static bool						sJobIsSetup;			// true id print rec is valid

		void							AssertInit();


		static void 					InitJobStruct( void* inJobPtr, long inNumPages );
};

#endif
