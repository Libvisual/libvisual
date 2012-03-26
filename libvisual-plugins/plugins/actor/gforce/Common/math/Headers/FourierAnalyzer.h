#ifndef FourierAnalyzer_H
#define FourierAnalyzer_H





class FourierAnalyzer {


	public:
								FourierAnalyzer();
		virtual					~FourierAnalyzer();

		/* Performs a fourier transform of inF[]:
		Pre: 	The function to be transformed is inF[] and is inN elements
		Pre:	outFT[] has at least inBins elements
		Post:	outFT[ x ] is the magnitude of the frequency component of frequency (inBinRange * ( x / inBins )) */
		void 					Transform( short inF[], long inN, long inBins, float inBinRange, float outFT[] );


	protected:

		float*					mTrigLookup;
		float*					mSinFT;
		long					mSinFTSize;

		float					mBinRange;
		long					mN, mNumBins;
};



#endif

