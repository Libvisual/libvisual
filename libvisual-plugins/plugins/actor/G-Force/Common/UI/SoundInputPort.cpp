



#include "SoundInputPort.h"

#include "EgOSUtils.h"


#define __err( msg )			EgOSUtils::ShowMsg( msg );

#if EG_MAC
#define __blockmove( srce, dest, num )  ::BlockMove( (char*) (srce), (char*) (dest), (num) )
#endif


#if EG_WIN
#define __blockmove( srce, dest, num )  ::MoveMemory( (dest), (srce), (num) )
#endif


SoundInput::SoundInput() {
	mParams = 0;
	mIsRecording = false;
	
	DimBufSize( 16 );
}


SoundInput::~SoundInput() {

	StopRecording();
}



void SoundInput::DimBufSize( long inNumSamples ) {
	long i, origSkipRate = ( mParams ) ? mParams -> mNumSampleSkip : 0;
	bool wasRecording = mIsRecording;
	
	// times 2 for GetSpectrum()
	long need = sizeof( SoundInputRec ) + 2 * sizeof( short ) * inNumSamples + 8;
	if ( need > mParamBuf.length() ) {

		StopRecording();

		mParams = (SoundInputRec*) mParamBuf.Dim( need );
		mParams -> mNumSampleSkip = origSkipRate;
	
		for ( i = 0; i < inNumSamples; i++ )
			mParams -> mSamples[ i ] = 0;
	}
			
	if ( mParams -> mNumSamples != inNumSamples ) {
		
		// Signal to our interrupt handler that we're using the buffer
		mParams -> mInUse = 1;

		mParams -> mNumSamples	= inNumSamples;
		mParams -> mBufEnd 	= 0;

		
		// Signal we're not using the buffer anymore
		mParams -> mInUse = 0;
	}
	
	if ( wasRecording )
		StartRecording();
}


bool SoundInput::GetSamples( short outSamples[], long inN ) {
	long i, n;
	
	DimBufSize( inN );
	
	// Signal to our interrupt handler that we're using the buffer
	mParams -> mInUse = 1;
	
	// We use a circular buffer, so to get the sequnce in order, paste the last half after the first half
	i = mParams -> mBufEnd;	
	n = mParams -> mNumSamples - i;
	__blockmove( &mParams -> mSamples[ i ], outSamples, sizeof( short ) * n );		
	__blockmove( &mParams -> mSamples[ 0 ], &outSamples[ n ], sizeof( short ) * ( inN - n ) );
	
	// Signal we're not using the buffer anymore
	mParams -> mInUse = 0;
	
	return true;
}



bool SoundInput::GetSamples( float outSamples[], long inN ) {
	long i, n;
	
	DimBufSize( inN );

	// Signal to our interrupt handler that we're using the buffer
	mParams -> mInUse = 1;
	
	// We use a circular buffer, so to get the sequnce in order, paste the last half after the first half
	n = mParams -> mNumSamples - mParams -> mBufEnd;
	for ( i = 0; i < n; i++ )
		outSamples[ i ] = mParams -> mSamples[ i + mParams -> mBufEnd ];
	for ( i = n; i < inN; i++ )
		outSamples[ i ] = mParams -> mSamples[ i - n ];
		
	// Signal we're not using the buffer anymore
	mParams -> mInUse = 0;

	return true;
}



bool SoundInput::GetSpectrum( long inBins, float inBinRange, float outFT[] ) {
	long i;
	
	DimBufSize( 1024 );
	
	// Signal to our interrupt handler that we're using the buffer
	mParams -> mInUse = 1;
		
	// We use a circular buffer, so to get the sequnce in order, paste the last half after the first half
	i = mParams -> mBufEnd;	
	__blockmove( mParams -> mSamples, &mParams -> mSamples[ mParams -> mNumSamples ], sizeof( short ) * i );
	
	// Turn a sequence of samples into a freq spectrum
	mAnalyzer.Transform( &mParams -> mSamples[ i ], mParams -> mNumSamples, inBins, inBinRange, outFT );
	
	// Signal we're not using the buffer anymore
	mParams -> mInUse = 0;
		
	return true;
}




static OSErr	SetupDevice (long inputDevice,
							short *numChannels,
							short *sampleSize,
							Fixed *sampleRate,
							OSType *compression,
							long *devBuffer) {

	OSErr				err;
	Fixed				gain = 0x00008000;
	short				on = 1;

	#if EG_MAC
	err = SPBSetDeviceInfo (inputDevice, siSampleRate, (Ptr) sampleRate);
	if (err != noErr)
		__err( "Couldn't set sample rate" );

	err = SPBSetDeviceInfo (inputDevice, siSampleSize, (Ptr) sampleSize);
	if (err != noErr)
		__err( "Couldn't set sample size");

	err = SPBSetDeviceInfo (inputDevice, siTwosComplementOnOff, (Ptr) &on);
	if (err != noErr)
		__err( "Couldn't set twos complement");

	err = SPBSetDeviceInfo (inputDevice, siNumberChannels, (Ptr) numChannels);
	if (err != noErr)
		__err( "Couldn't set number of channels");

	err = SPBSetDeviceInfo (inputDevice, siCompressionType, (Ptr) compression);
	if (err != noErr)
		__err( "Couldn't set compression type");

	//turn on continuous recording to "warm up" the input device
	err = SPBSetDeviceInfo (inputDevice, siContinuous, (Ptr) &on);
	if (err != noErr)
		__err( "Couldn't turn on continuous recording");

	//turn on Automatic Gain Control
	err = SPBSetDeviceInfo (inputDevice, siAGCOnOff, (Ptr) &on);
	if (err != noErr) {
		//If AGC isn't available, just turn it all the way down to avoid over driving
		err = SPBSetDeviceInfo (inputDevice, siInputGain, (Ptr) &gain);
		if (err != noErr)
			__err( "Couldn't get siInputGain");
	}

	//check to see what we really got
	err = SPBGetDeviceInfo (inputDevice, siSampleRate, (Ptr) sampleRate);
	if (err != noErr)
		__err( "Couldn't get sample rate");

	err = SPBGetDeviceInfo (inputDevice, siSampleSize, (Ptr) sampleSize);
	if (err != noErr)
		__err( "Couldn't get sample size");

	err = SPBGetDeviceInfo (inputDevice, siNumberChannels, (Ptr) numChannels);
	if (err != noErr)
		__err( "Couldn't get number of channels");

	err = SPBGetDeviceInfo (inputDevice, siDeviceBufferInfo, (Ptr) devBuffer);
	if (err != noErr)
		__err( "Couldn't get number of channels");

	err = SPBGetDeviceInfo (inputDevice, siCompressionType, (Ptr) compression);
	if (err != noErr)
		__err( "Couldn't get compression type");
	#endif
	
	return err;
}



OSErr SoundInput::StartRecording () {
	OSErr				err;

	if ( ! mIsRecording ) {

		#if EG_WIN
		WAVEFORMATEX waveFmt;
		waveFmt.wFormatTag = 1;
		waveFmt.nChannels = 1;
		waveFmt.nSamplesPerSec = 11025;
		waveFmt.wBitsPerSample = 16;
		waveFmt.nAvgBytesPerSec = waveFmt.nSamplesPerSec * waveFmt.wBitsPerSample;
		waveFmt.nBlockAlign = waveFmt.nChannels * waveFmt.wBitsPerSample / 8;
		waveFmt.cbSize = 0;


		if ( waveInOpen( mParams -> recordRec, 0, &waveFmt, 0, 0, 0) == MMSSYSERR_NOERROR ) {
		
		}

LPHWAVEIN lphWaveIn;    /* variable to receive a handle to the input device */
UINT IDDevice;      /* identifier of the device */
LPWAVEFORMAT lpwf;  /* address of structure with device format  */
DWORD dwCallback;       /* address of callback or window handle */
DWORD dwCallbackInstance;   /* instance data    */
DWORD fdwOpen;      /* open option  */

use the following values:

'hAud' or another handle offset,
0 - num devs, (use 0 for default input.  could be MMX device!)
WAVEFORMAT offset,
0, (no callback)
0, (no instance)
0, (no flags)


		#endif
		
		#if EG_MAC
		err = SPBOpenDevice (nil, siWritePermission, &mParams->soundRefNum);
		if (err != noErr) {
			__err( "Couldn't open the device");
		}

		mParams->numChannels	= 1;
		mParams->sampleSize	= 16;
		mParams->sampleRate	= rate44khz;
		mParams->compression	= 'NONE';
		err = SetupDevice (mParams->soundRefNum, &mParams->numChannels,
												&mParams->sampleSize,
												&mParams->sampleRate,
												&mParams->compression,
												&mParams->devBuffer);



		if (err == noErr) {
		/*	IOCompletionUPP 	MyPB0WriteCompUPP,
								MyPB1WriteCompUPP;*/
			//SICompletionUPP 	MyRecCompUPP;
			SIInterruptUPP		MyIntCompUPP;

			//mParams->sanitycheck = 'SANE';
			//myVars->whichBuffer = 0;
			mParams->myA5 = SetCurrentA5 ();

			//set up the record parameters
			//MyRecCompUPP = NewSICompletionProc (MyRecComp);
			MyIntCompUPP = NewSIInterruptProc (MyRecInterrupt);
			mParams->recordRec.inRefNum = mParams->soundRefNum;
			mParams->recordRec.count = 0;
			mParams->recordRec.milliseconds = 0;
			mParams->recordRec.bufferLength = 0 ;//buffSize;
			mParams->recordRec.bufferPtr = 0; //myVars->recBuffer;
			mParams->recordRec.completionRoutine = 0; // MyRecCompUPP;
			mParams->recordRec.interruptRoutine = MyIntCompUPP;
			mParams->recordRec.userLong = (long) mParams;
			mParams->recordRec.error = 0;
			mParams->recordRec.unused1 = 0;
			mParams->mInUse = false;

		}


		//mParams->totalBytes = 0;
		err = SPBRecord (&mParams->recordRec, true);
		if (err != noErr) {
			__err( "SPBRecord failed");
		}
		
		if ( err == noErr )
			mIsRecording = true;

		#endif
	}
	
	return err;
}




void SoundInput::StopRecording () {
	OSErr err;
	
	if ( mIsRecording ) {
	
		#if EG_MAC
		err = SPBStopRecording (mParams -> soundRefNum);
		if (err != noErr)
			__err( "SPBStopRecording failed");

		err = SPBCloseDevice (mParams -> soundRefNum);
		if (err != noErr)
			__err( "SPBCloseDevice failed");
		#endif
			
		mIsRecording = false;
	}
	
}



pascal void		MyRecInterrupt( SPB* inPB, short* buf, long peak, long size ) {

	//SPBPtr inPB:__a0;
	#pragma unused( peak )
	
	SoundInputRec*			recPtr;
	int i;
	long oldA5, offset, skip;

	#if EG_MAC
	recPtr = (SoundInputRec*)inPB->userLong;

	oldA5 = SetA5 (recPtr->myA5);
	
	// We can only write to the buf if its not in use
	if ( ! recPtr -> mInUse ) {
	
		offset = recPtr -> mBufEnd;
		skip = 1 + recPtr -> mNumSampleSkip;
		
		for ( i = 0; i < size; i += skip ) {
			recPtr -> mSamples[ offset ] = buf[ i ];
			offset = ( offset + 1 ) % recPtr -> mNumSamples;
		}
		
		recPtr -> mBufEnd = offset;
	}

	oldA5 = SetA5 (oldA5);
	#endif
}

