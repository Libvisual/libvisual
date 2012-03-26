#ifndef SoundInput_H
#define SoundInput_H

#if EG_MAC
#include <Sound.h>
#endif

#include "FourierAnalyzer.h"
#include "UtilStr.h"
#include "CEgErr.h"

#if EG_WIN
typedef long Fixed;
typedef long OSType;
typedef long OSErr;
typedef LPWAVEIN SPB;

#endif
/*
enum InputSampleRate {
	
	SI_44kHz,
	SI_22kHz,
	SI_11kHz,
};*/


//Keep track of the info needed to record
typedef struct {
		SPB				recordRec;
		Fixed			sampleRate;
		OSType			compression;
		//unsigned long	totalBytes;
		long			mNumSampleSkip;
		long			myA5,
						devBuffer,
						soundRefNum;
		short			/*whichBuffer,*/
						numChannels,
						sampleSize;
		long			mBufEnd;
		char			mInUse;
		long			mNumSamples;
		short			mSamples[ 1 ];
} SoundInputRec;



pascal void		MyRecInterrupt(SPB* inPB, short* buf, long peak, long size );



class SoundInput {


	public:
								SoundInput();
		virtual					~SoundInput();

		// How we turn the port on and off.  Call StartRecording() before you make calls to GetSamples() or GetSpectrum() 
		OSErr					StartRecording();
		void					StopRecording();
		
		// Retrieves a sound clip, filling inF[]
		// Returns false if the data was not returned
		bool 					GetSamples( short outSamples[], long inN );
		bool					GetSamples( float outSamples[], long inN );
		
		// Tells the sound input how many samples are thrown away per sample taken.  The default value is 0.
		void					SetSampleSkip( long inNumSamples )						{ mParams -> mNumSampleSkip = inNumSamples;		}

		// Retrieves a sound clip from the sound in and performs a fourier transform to get a freq spectrum.
		// See FourierAnalyzer.h for a description of the params.
		// Returns false if the data was not returned
		bool 					GetSpectrum( long inBins, float inBinRange, float outFT[] );


		
	protected:
		FourierAnalyzer			mAnalyzer;
		UtilStr					mParamBuf;
		SoundInputRec*			mParams;
		bool					mIsRecording;
		
		void					DimBufSize( long inNumSamples );
		
};


#endif


