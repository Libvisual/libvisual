#include "FourierAnalyzer.h"

#include <math.h>


FourierAnalyzer::FourierAnalyzer() {
	mTrigLookup = 0;
	mSinFTSize = 0;
	mSinFT = 0;
	mBinRange = 0;
	mN = 0;
	mNumBins = 0;
	
}


FourierAnalyzer::~FourierAnalyzer() {

	if ( mSinFT )
		delete []mSinFT;

	if ( mTrigLookup )
		delete []mTrigLookup;

}

void FourierAnalyzer::Transform( short inF[], long inN, long inNumBins, float inBinRange, float outFT[] ) {
	float* trig, samp, t;
	int i, j;

	// Maintain our imaginary component freq spectrum
	if ( mSinFTSize < inNumBins ) {
		if ( mSinFT )
			delete []mSinFT;
		mSinFT = new float[ inNumBins ];
		mSinFTSize = inNumBins;
	}
	
	// Regenerate trig lookup tables if client asks for different specs
	if ( inBinRange != mBinRange || mN != inN || inNumBins != mNumBins ) {
		if ( mTrigLookup )
			delete []mTrigLookup;
		mTrigLookup = new float[ 2 * inN * inNumBins ];
			
		mN 			= inN;
		mNumBins	= inNumBins;
		mBinRange	= inBinRange;
		
		// Compute all the sin and cos values and store them in the order they'll be needed
		trig = mTrigLookup; 
		for ( i = 0; i < inN; i++ ) {
			for ( j = 1; j <= inNumBins; j++ ) {
				t = ( (float) (j * i) ) * inBinRange * 6.2831853071 / ( (float) inN );
				*trig = cos( t ) / ( (float) inN );		trig++;
				*trig = sin( t ) / ( (float) inN );		trig++;
			}
		}
	}
		
	// Init the output freq spectrums
	for ( i = 0; i < inNumBins; i++ )
		mSinFT[ i ] = 0;
	for ( i = 0; i < inNumBins; i++ )
		outFT [ i ] = 0;
	
	// Finally, we can do the fourier transform...
	trig = mTrigLookup;
	for ( i = 0; i < inN; i++ ) {
		samp =inF[ i ];
		for ( j = 0; j < inNumBins; j++ ) {
			outFT [ j ] += samp * (*trig); trig++;	// *trig here is cos( j * 2pi * (i/inN) )
			mSinFT[ j ] += samp * (*trig); trig++;	// *trig here is sin( j * 2pi * (i/inN) )
		}
	}

	// The actual freq component is the magnitude of the sin and cos freq component vector
	for ( j = 0; j < inNumBins; j++ )
		outFT [ j ] = sqrt( outFT [ j ] * outFT [ j ] + mSinFT[ j ] * mSinFT[ j ] );
}
