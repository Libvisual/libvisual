#include "G-Force_Proj.h"

#include "ParticleGroup.h"

#include "ArgList.h"
#include "EgOSUtils.h"

#include <math.h>
#ifdef UNIX_X
#include "trunc.h"
#endif

ParticleGroup::ParticleGroup( float* inTPtr, ExprUserFcn** inMagFcn ) :
	WaveShape( *inTPtr ) {

	SetMagFcn( inMagFcn );

	mDict.AddVar( "ID", &mID );
	mDict.AddVar( "NUM", &mNumInstances );
	mDict.AddVar( "END_TIME", &mEndTime );

	mTPtr = inTPtr;
}





void ParticleGroup::Load( ArgList& inArgs ) {
	UtilStr str;
	Expression expr;

	// User may access the DT var
	mStartTime = *mTPtr;
	mFadeTime = ( (float) EgOSUtils::Rnd( 200, 350 ) ) / 100.0;

	// Calculate how many interations/instances of this particle
#define VAL(a, b, c) ((a << 16) + (b << 8) + c)
	inArgs.GetArg( VAL('N','U','M'), str );
#undef VAL
	expr.Compile( str, mDict );
	mNumInstances = trunc( expr.Evaluate() );
	if ( mNumInstances < 1 )
		mNumInstances = 1;

	// A vars shouldn't be accessing the ID global var, but zero it anyway
	mID = 0;

	// Load everything else in the wave
	WaveShape::Load( inArgs, 32 );
}



void ParticleGroup::DrawGroup( PixPort& inDest ) {
	float fader;

	// User may access the DURATION var
	if ( *mTPtr - mStartTime < mFadeTime ) {
		fader = ( *mTPtr - mStartTime ) / mFadeTime;
		fader = .1 + .9 * sin( 3.14159 * fader / 2 ); }
	else if ( mEndTime - *mTPtr < mFadeTime ) {
		fader = ( mEndTime - *mTPtr ) / mFadeTime;
		fader = 1 - .9 * sin( 3.14159 * ( .5 + fader / 2 ) ); }
	else
		fader = 1;

	// Remember mID can be accessed be accesed byt he particle
	for ( mID = 0; mID < mNumInstances; mID += 1 ) {
		Draw( 32, inDest, fader, 0, 0 );
	}
}
