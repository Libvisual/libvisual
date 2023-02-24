#include "GF_Palette.h"

#include "ArgList.h"
#include "EgOSUtils.h"

#include <time.h>
#include <stdlib.h>

GF_Palette::GF_Palette( float& inT, float& inIntensity ) {

	mIntensity = &inIntensity;
	mDict.AddVar( "I", &inIntensity );
	mDict.AddVar( "T", &inT );

	mPI = 3.14159265358979;
	mDict.AddVar( "PI", &mPI );

}


void GF_Palette::Assign( const ArgList& inArgs ) {
	UtilStr str;

	// Mix up the rnd seed
	srand( clock() );

	// Compile and link the temp exprs.  By spec, A-vars are evaluated now
	mAVars.Compile( inArgs, 'A', mDict );
	mAVars.Evaluate();

	inArgs.GetArg( 'H', str );		mH.Compile( str, mDict );
	inArgs.GetArg( 'S', str );		mS.Compile( str, mDict );
	inArgs.GetArg( 'V', str );		mV.Compile( str, mDict );

	mH_I_Dep = mH.IsDependent( "I" );
	mS_I_Dep = mS.IsDependent( "I" );
	mV_I_Dep = mV.IsDependent( "I" );

}


void GF_Palette::Evaluate( PixPalEntry outPalette[ 256 ] ) {
	float H = 0.0;
	float S = 0.0;
	float V = 0.0;
	float inc = 1.0 / 255.0;

	*mIntensity = 0;

	// Prep to save evaluation time for vars indep of i
	if ( ! mH_I_Dep )	H = mH.Evaluate();
	if ( ! mS_I_Dep )	S = mS.Evaluate();
	if ( ! mV_I_Dep )	V = mV.Evaluate();

	for ( int i = 0; i < 256; i++, *mIntensity += inc ) {

		// Don't reevaluate vars that are indep of i
		if ( mH_I_Dep )		H = mH.Evaluate();
		if ( mS_I_Dep )		S = mS.Evaluate();
		if ( mV_I_Dep )		V = mV.Evaluate();


		RGBColor rgb;
		EgOSUtils::HSV2RGB( H, S, V, rgb );
		outPalette[i].red = rgb.red >> 8;
		outPalette[i].green = rgb.green >> 8;
		outPalette[i].blue = rgb.blue >> 8;
		outPalette[i].x = i;
	}
}


void GF_Palette::SetupTransition( GF_Palette& inDest, float& inC ) {

	mH.Weight( inDest.mH, &inC, 0 );
	mS.Weight( inDest.mS, &inC, 0 );
	mV.Weight( inDest.mV, &inC, 0 );

	mH_I_Dep = mH_I_Dep || inDest.mH_I_Dep;
	mS_I_Dep = mS_I_Dep || inDest.mS_I_Dep;
	mV_I_Dep = mV_I_Dep || inDest.mV_I_Dep;
}
