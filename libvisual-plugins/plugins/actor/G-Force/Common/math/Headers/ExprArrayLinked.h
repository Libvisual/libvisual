#ifndef _ExprArrayLinked_H
#define _ExprArrayLinked_H


#include "ExprArray.h"
#include "XPtrMatrix.h"


Warning: this code is untested


/* An ExprArray that has the ability to analyze itself, and group elements according to the
highest stength token an element may contain.  

Example usage:

ExprArrayLinked mTempExprs;
mTempExprs.Compile( <blah> );

// Group the temp vars by strength, where the vars they use have a given strength
Hashtable strenthTable( true );
strenthTable.Put( new UtilStr( "S" ), (void*) 2 );
strenthTable.Put( new UtilStr( "T" ), (void*) 1 );
mTempExprs.SetupStrengthGroups( strenthTable );

// We can evaluate these now, since they don't depend on S or T
mTempExprs.EvaluateStrength( 0 );
*/

class ExprArrayLinked : public ExprArray {

	public:

	//						ExprArrayLinked();
	//	virtual				~ExprArrayLinked();

		// Pre:  ioVarList is an owning string-key hashtable, mapping to longs that represent a 'strength'.  
		// Priority n corresponds to ioVarPriority[n], an number array.  An example works best:
		// Input: ioVarList:  ( ("t",1), ("dt",2), ("s",3) ), this: A0="99+s+t", A1="dt*7", A2="A0+dt", A3="t^3", A4="3.4+5"
		// Ouput: ioVarList:  ( ("t",1), ("dt",2), ("s",3), ("A0",3), ("A1",2), ("A2",3), ("A3", 1), ("A4",0) ),
		// mStrengthGroups[]: [0]=4, [1]=3, [2]=1, [3]=0,2  
		// This fcn should be called after Compile() and before EvaluateStrength()
		// Note: strength 0 denotes all elements whose identifiers weren't found in ioVarList
		void				SetupStrengthGroups( Hashtable& ioVarList );
		

		// Evalutes all array elments of the given strength.  
		void				EvaluateStrength( long inStength );


	protected:
		XPtrMatrix			mStrengthGroups;	

};

#endif