#include "Printable.h"

#ifdef UNIX_X
/* FIXME I guess printing goes here.  Optional. */
#else

#include "EgOSUtils.h"

#ifdef EG_ZINC
#include <Z_print.hpp>
#endif




ZafPrinter*		Printable::sZafPrinter 	= NULL;
bool			Printable::sJobIsSetup	= false;
char			Printable::sJobRecord[500];



#ifdef EG_ZINC

void* Printable::GetPrintDevice() {
	
	AssertInit();
	
	return sZafPrinter -> DisplayContext();
}



void Printable::InitJobStruct( void* inJobPtr, long inNumPages ) {
	ZafPrintJobStruct*	job = (ZafPrintJobStruct*) inJobPtr;

	job -> collate		= false;
	job -> copies		= 1;
	job -> maxPage		= inNumPages;
	job -> startPage	= 1;
	job -> endPage		= job -> maxPage;
	job -> minPage		= 1;
}


bool Printable::Print( char* inJobName, int inOption ) {
	ZafPrintJobStruct	job;
	ZafPrintJobStruct*	defJob = (ZafPrintJobStruct*) sJobRecord;
	int					curCopy, curPage;
	bool				ok = false;
	
	AssertInit();
		
	curPage = GetNumPages();

	if ( inOption == 1 && ! sJobIsSetup )
		inOption = 0;
		
	switch ( inOption ) {
	
		case 0:							// Std print	(print setup dialog)
			InitJobStruct( defJob, curPage );	
			ok = JobSetup();
			if ( ok ) 
				job	= *defJob;
			break;
										// Def Print	(uses most recent record from JobSetup() )
		case 1:
			ok = sJobIsSetup;
			if ( ok ) 
				job	= *defJob;
			break;

		case 2:							// Print One 	(no print setup and just print one)
			InitJobStruct( &job, curPage );
			ok = true;
			break;
	}
	
	if ( ok ) {
		if ( sZafPrinter -> BeginJob( inJobName ) == ZAF_ERROR_NONE ) {
	
			SetPrintDevice( (void*) sZafPrinter -> DisplayContext() );
			
			if ( job.collate ) {
				for ( curCopy = 0; curCopy < job.copies; curCopy++ ) {
					for ( curPage = job.startPage; curPage <= job.endPage; curPage++ ) {
						sZafPrinter -> BeginPage();
						DrawPage( curPage, 0, 0 );
						sZafPrinter -> EndPage();
					}
				} }
			else {
				for ( curPage = job.startPage; curPage <= job.endPage; curPage++ ) {
					for ( curCopy = 0; curCopy < job.copies; curCopy++ ) {
						sZafPrinter -> BeginPage();
						DrawPage( curPage, 0, 0 );
						sZafPrinter -> EndPage();
					}
				}
			}
			sZafPrinter -> EndJob(); }
		else
			ok = false;
	}
	
	return ok;
}






bool Printable::PageSetup() {

	AssertInit();
	sZafPrinter -> PrintSetup();
}

#endif




#ifdef EG_MAC

#include "CEgPrintout.h"
#include "UPrintingMgr.h"


bool Printable::Print( char* inJobName, int inOption ) {
	THPrint		printRec;
	
	if (inOption !=2)
		printRec = UPrintingMgr::GetDefaultPrintRecord();
	
	else printRec = UPrintingMgr::CreatePrintRecord();
	
	if ( printRec ) {
		CEgPrintout theJob( printRec );							// Set the current print record from this doc
		theJob.DoJob( this, inJobName );						// Print the exam image part
		
	}
	return true;
}


#endif







#ifdef EG_MAC
#include <UPrintingMgr.h>
#endif



void Printable::GetPagePointSize( long& outWidth, long& outHeight ) {
	outWidth	= 0;
	outHeight	= 0;

	#ifdef EG_MAC
	THPrint		printRec	= UPrintingMgr::GetDefaultPrintRecord();
	Rect 		paperRect;
	
	if ( printRec ) {
		paperRect		= (**printRec).rPaper;
		
		outWidth		= paperRect.right - paperRect.left;
		outHeight		= paperRect.bottom - paperRect.top; 
	}
	#endif
	
	
	#ifdef EG_ZINC
	outWidth 	= ( EgOSUtils::sXdpi * sZafPrinter -> PaperWidth() ) / sZafPrinter -> pixelsPerInchX;
	outHeight 	= ( EgOSUtils::sYdpi * sZafPrinter -> PaperHeight() ) / sZafPrinter -> pixelsPerInchY;
	#endif
}





void Printable::CloseOutputPort() {

}



void Printable::AssertInit() {

	#ifdef EG_ZINC
	if ( ! sZafPrinter ) {
		sZafPrinter = new ZafPrinter;
		sZafPrinter -> SetMargins( 0, 0, 0, 0 );
		InitJobStruct( (ZafPrintJobStruct*) sJobRecord, 1 );
	}
	#endif

}



void Printable::Shutdown() {

	#ifdef EG_ZINC
	if ( sZafPrinter ) {
		delete sZafPrinter;
		sZafPrinter = NULL;
	}
	#endif
}




bool Printable::JobSetup() {

	#ifdef EG_ZINC
	AssertInit();
	ZafPrintJobStruct* job = (ZafPrintJobStruct*) sJobRecord;
	sJobIsSetup = sZafPrinter -> JobSetup( *job );
	#endif

	
	// *** Implement mac code
	
	return sJobIsSetup;
}



void Printable::SetPrintDevice( void* inDevice ) {
	mCurPrintDevice = inDevice;
}

#endif

