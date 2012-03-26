#include "ExpressionDict.h"

#include "UtilStr.h"


ExpressionDict::ExpressionDict() :
	mVarDict( true ) {

}



void ExpressionDict::AddVar( const char* inKey, float* inPtr ) {

	mVarDict.Put( new UtilStr( inKey ), inPtr );
}


void ExpressionDict::AddFcn( const char* inKey, ExprUserFcn** inFcn ) {


	mVarDict.Put( new UtilStr( inKey ), inFcn );
}


float* ExpressionDict::LookupVar( const UtilStr& inName ) {
	float* addr;

	if ( mVarDict.Get( &inName, (void **)(&addr) ) )
		return addr;
	else
		return 0;
}


ExprUserFcn** ExpressionDict::LookupFunc( const UtilStr& inName ) {

	return (ExprUserFcn**) LookupVar( inName );
}
