/* *WARNING* *FIXME* -or- see some SIGFPEs
 * This file will execute math operations specified by the user in external
 * configuration files.  It does almost no error checking.  It seems that
 * the FPU in the Pentium (and hopefully all other x86 chips) will happily
 * do things like x/0, tan(pi/2.0), sqrt(-1) and so on.  The result will
 * be garbage but the program won't die and no SIGFPE will happen.  But
 * I'm not sure if this is the case on all platforms.
 *
 * Note that invalid integer operations like x/0 or x%0 will cause a SIGFPE,
 * and since I don't know how to handle that, the one integer operation below
 * (mod) does a check for mod 0 and returns 0.
 */

#include "ExprVirtualMachine.h"

#include <math.h>
#ifdef UNIX_X
#include "trunc.h"
#endif

#include <libvisual/libvisual.h>
#include <limits>
#include <stdlib.h>


#define __addInst( opcode, data16 )		long op = (opcode) | (data16);	\
										mProgram.Append( &op, sizeof(long) );


#define REG_IN_USE	0x1
#define REG_USED	0x2

ExprUserFcn ExprVirtualMachine::sZeroFcn = { 0, { 0 } };

#define _fetch( r, val )	switch ( (r) ) {					\
								case 0:		val = FR0;	break;	\
								case 1:		val = FR1;	break;	\
								case 2:		val = FR2;	break;	\
								case 3:		val = FR3;	break;	\
								case 4:		val = FR4;	break;	\
								case 5:		val = FR5;	break;	\
								case 6:		val = FR6;	break;	\
								case 7:		val = FR7;	break;	\
								default:	val = mVirtFR[ r - NUM_PHYS_REGS ];	\
							}
								/*
								case 8:		val = FR8;	break;	\
								case 9:		val = FR9;	break;	\
								case 10:	val = FR10;	break;	\
								case 11:	val = FR11;	break;	\
								case 12:	val = FR12;	break;	\
								case 13:	val = FR13;	break;	\
								case 14:	val = FR14;	break;	\
								case 15:	val = FR15;	break;	\
								default:	val = 0;	break;	\
							}*/

#define _store( r, val )	switch ( (r) ) {					\
								case 0:		FR0 = val;	break;	\
								case 1:		FR1 = val;	break;	\
								case 2:		FR2 = val;	break;	\
								case 3:		FR3 = val;	break;	\
								case 4:		FR4 = val;	break;	\
								case 5:		FR5 = val;	break;	\
								case 6:		FR6 = val;	break;	\
								case 7:		FR7 = val;	break;	\
								default:	mVirtFR[ r - NUM_PHYS_REGS ] = val; \
							}
							/*
								case 8:		FR8 = val;	break;	\
								case 9:		FR9 = val;	break;	\
								case 10:	FR10 = val;	break;	\
								case 11:	FR11 = val;	break;	\
								case 12:	FR12 = val;	break;	\
								case 13:	FR13 = val;	break;	\
								case 14:	FR14 = val;	break;	\
								case 15:	FR15 = val;	break;	\
							}*/




#define _exeFn( r )		switch ( subop ) {							\
							case cSQRT:	r = sqrt( r );	break;		\
							case cABS:	r = fabs( r );	break;		\
							case cSIN:	r = sin( r );	break;		\
							case cCOS:	r = cos( r );	break;		\
							case cSEED: i = *((long*) &r);						\
										size = i % 31;							\
										srand( ( i << size ) | ( i >> ( 32 - size ) )  ); 	break;				\
							case cTAN:	r = tan( r );	break;		\
							case cSGN:	r = ( r >= 0 ) ? 1 : -1;	break;		\
							case cLOG:	r = log( r );	break;		\
							case cEXP:	r = exp( r );	break;		\
							case cSQR:	r = r * r;		break;		\
							case cATAN:	r = atan( r );	break;		\
							case cTRNC:	r = trunc( r );	break;		\
							case cWRAP:	r = r -	 floor( r );  break;	\
							case cRND:	r = r * ( (float) LV::rand() ) / ( (float) std::numeric_limits<uint32_t>::max() ); break; \
							case cSQWV:	r = ( r >= -1 && r <= 1 ) ? 1 : 0;	break;	\
							case cTRWV: r = fabs( .5 * r ); r = 2 * ( r - floor( r ) );  if ( r > 1 ) r = 2 - r;	break;	\
							case cPOS:	if ( r < 0 ) r = 0;			break;		\
							case cCLIP:	if ( r < 0 ) r = 0; else if ( r > 1 ) r = 1; break;	\
							case cFLOR: r = floor( r );	break;		\
						}


#define _exeOp( r1, r2 ) 	switch ( subop ) {						\
								case '+':	r1 += r2;		break;	\
								case '-':	r1 -= r2;		break;	\
								case '/':	r1 /= r2;		break;	\
								case '*':	r1 *= r2;		break;	\
								case '^':	r1 = pow( r1, r2 );					break;	\
								case '%':	{ long tt = r2; r1 = (tt != 0) ? (( (long) r1 ) % tt) : 0.0; 	break; }	\
							}

ExprVirtualMachine::ExprVirtualMachine() {
	mPCStart	= 0;
	mPCEnd		= 0;

	sZeroFcn.mNumFcnBins = 1;
	sZeroFcn.mFcn[ 0 ] = 0;
}


int ExprVirtualMachine::FindGlobalFreeReg() {
	int reg = 1;

	// Look for a global free register
	while ( ( mRegColor[ reg ] & REG_USED ) && reg < NUM_REGS )
		reg++;


	return reg;
}


int ExprVirtualMachine::AllocReg() {
	int reg = 0;

	// Look for a free register (ie, find one not in use right now)...
	while ( ( mRegColor[ reg ] & REG_IN_USE ) && reg < NUM_REGS )
		reg++;

	// Color it
	if ( reg < NUM_REGS )
		mRegColor[ reg ] = REG_IN_USE | REG_USED;

	return reg;
}



void ExprVirtualMachine::DeallocReg( int inReg ) {

	// Clear the bit that says the reg is allocated
	mRegColor[ inReg ] &= ~REG_IN_USE;
}


void ExprVirtualMachine::DoOp( int inReg, int inReg2, char inOpCode ) {

	__addInst( OP_OPER, ( inOpCode << 16 ) | ( inReg2 << 8 ) | inReg )
}





void ExprVirtualMachine::Move( int inReg, int inDestReg ) {

	if ( inDestReg != inReg ) {
		__addInst( OP_MOVE, ( inDestReg << 8 ) | inReg )
	}
}




void ExprVirtualMachine::Loadi( float inVal, int inReg ) {

	__addInst( OP_LOADIMMED, inReg )
	mProgram.Append( &inVal, sizeof( float ) );
}


void ExprVirtualMachine::Loadi( float* inVal, int inReg ) {

	__addInst( OP_LOAD, inReg )
	mProgram.Append( &inVal, sizeof(float*) );
}



void ExprVirtualMachine::UserFcnOp( int inReg, ExprUserFcn** inFcn ) {

	if ( inFcn ) {
		__addInst( OP_USER_FCN, inReg )
		mProgram.Append( &inFcn, sizeof(void*) );  }
	else
		Loadi( 0.0, inReg );
}



void ExprVirtualMachine::MathOp( int inReg, char inFcnCode ) {

	if ( inFcnCode ) {
		__addInst( OP_MATHOP, ( inFcnCode << 16 ) | inReg )
	}
}







void ExprVirtualMachine::Clear() {

	// Init register coloring
	for ( int i = 0; i < NUM_REGS; i++ )
		mRegColor[ i ] = 0;

	mProgram.Wipe();
}



void ExprVirtualMachine::PrepForExecution() {
	mPCStart	= mProgram.getCStr();
	mPCEnd		= mPCStart + mProgram.length();
}


// An inst looks like:
// 0-7: 	Inst opcode
// 8-15:	Sub opcode
// 16-23:	Source Reg
// 24-31:	Dest Register number

float ExprVirtualMachine::Execute/*_Inline*/() {
	float	FR0, FR1, FR2, FR3, FR4, FR5, FR6, FR7; // FR8, FR9, FR10, FR11, FR12, FR13, FR14, FR15;
	float	v1, v2;
	const char*	PC	= mPCStart;
	const char*	end	= mPCEnd;
	unsigned long	inst, opcode, subop, size, i, r2, r1;
	float			mVirtFR[ NUM_REGS - NUM_PHYS_REGS ];

	while ( PC < end ) {
		inst = *((long*) PC);
		PC += sizeof(long);

		opcode = inst & 0xFF000000;
		r1 = inst & 0xFF;
		r2 = ( inst >> 8 ) & 0xFF;

		if ( opcode == OP_LOADIMMED ) {
			v1 = *((float*) PC);
			PC += sizeof(float); }
		else if ( opcode == OP_LOAD ) {
			v1 = **((float**) PC);
			PC += sizeof(float*); }
		else {

			_fetch( r1, v1 )

			switch ( opcode ) {

				case OP_OPER:
					subop = ( inst >> 16 ) & 0xFF;
					_fetch( r2, v2 )
					_exeOp( v1, v2 )
					break;

				case OP_MATHOP:
					subop = ( inst >> 16 ) & 0xFF;
					_exeFn( v1 )
					break;

				case OP_MOVE:
					r1 = r2;
					break;

				case OP_USER_FCN:
				  {
					ExprUserFcn* fcn = **((ExprUserFcn***) PC);
					size = fcn -> mNumFcnBins;
					i = v1 * size;
					if ( i >= 0 && i < size )
						v1 = fcn -> mFcn[ i ];
					else if ( i < 0 )
						v1 = fcn -> mFcn[ 0 ];
					else
						v1 = fcn -> mFcn[ size - 1 ];
					PC += sizeof(void*);
					break;
				  }
				case OP_WLINEAR:
				case OP_WEIGHT:
					_fetch( r2, v2 )
					float temp = **((float**) PC);
					if ( opcode == OP_WEIGHT ) {
						v1 = temp * v2 + ( 1.0 - temp ) * v1;
						PC += sizeof(float*); }
					else {
						v1 = **((float**) PC) * v1 + **((float**) PC+4) * v2;
						PC += sizeof(float*) * 2;
					}
					break;

			}
		}
		_store( r1, v1 )
	}

	return FR0;
}





void ExprVirtualMachine::Chain( ExprVirtualMachine& inVM, float* inC1, float* inC2 ) {
	int tempReg = inVM.FindGlobalFreeReg();

	// Move the output of this VM to a reg that won't get overwritten by inVM
	Move( 0, tempReg );

	// Now execute inVM (we know it won't touch tempReg)
	mProgram.Append( inVM.mProgram );

	// Use the special weight op that combines the two outputs via an embedded addr to a 0 to 1 value
	// Note that the output is moved to register 0
	if ( inC2 ) {
		__addInst( OP_WLINEAR, ( tempReg << 8 ) | 0 )
		mProgram.Append( &inC1, sizeof(float*) );
		mProgram.Append( &inC2, sizeof(float*) ); }
	else {
		__addInst( OP_WEIGHT, ( tempReg << 8 ) | 0 )
		mProgram.Append( &inC1, sizeof(float*) );
	}

	// The reg coloring for this VM is the OR of the two's coloring
	for ( int i = 0; i < NUM_REGS; i++ )
		mRegColor[ i ] |= inVM.mRegColor[ i ];

	PrepForExecution();
}



void ExprVirtualMachine::Assign( ExprVirtualMachine& inExpr ) {

	mProgram.Assign( inExpr.mProgram );

	for ( int i = 0; i < NUM_REGS; i++ )
		mRegColor[ i ] = inExpr.mRegColor[ i ];

	PrepForExecution();
}



/*
void ExprVirtualMachine::Neg() {

	__addInst( OP_NEG, 0 )
}
*/

/*
void ExprVirtualMachine::Optimize() {
	char*		base = mProgram.getCStr();
	long*		PC	= (long*) mProgram.getCStr();
	long*		end	= (long*) (((char*) PC) + mProgram.length());
	long		reg, opcode;
	long*		start;

	while ( PC < end ) {
		opcode = (*PC) & 0xFF000000;
		start = PC;
		PC++;

		// Maintain the PC
		if ( opcode == OP_LOADIMMED )
			PC += 2;
		else if (  opcode == OP_LOAD )
			PC++;

		// Look for a 'Load into r0, <Math op>, Move from r0' sequence
		if ( opcode == OP_LOADIMMED || opcode == OP_LOAD ) {
			opcode = (*PC) & 0xFF000000;
			if ( opcode == OP_MOVE_FR0 ) {
				reg = *PC & 0xFF;										// Extract the final dest register
				*start = (*start) | reg;								// Change the load so it loads right into the reg it needs to
				mProgram.Remove( 1 + ( ((char*) PC) - base ), 4 ); }	// Delete the move from fr0 inst
				// ??
			else if ( opcode == OP_MATHOP ) {
				opcode = (*(PC + 1)) & 0xFF000000;
				if ( opcode == OP_MOVE_FR0 ) {
					reg = *(PC + 1) & 0xFF;								// Extract the final dest register
					*start = (*start) | reg;							// Change the load so it loads right into the reg it needs to
					*PC = (*PC) | reg;									// Change the math op so it operates on the proper reg (see above)
					mProgram.Remove( 5 + ( ((char*) PC) - base ), 4 );	// Delete the move from fr0 inst
				}
			}  // ??
		}
	}

	// Minimzing pushes/pops via stack analysis
	StackReduction( 0, mProgram.length() );
}




long ExprVirtualMachine::StackReduction( long inStartPC, long inEndPC ) {
	long regsInUse = 0, opcode, fcnDepth = 0, pushLoc, reg, regsToPush;
	long PC = inStartPC, progLen, subRegs, *inst;
	char* base = mProgram.getCStr();

	while ( PC < inEndPC ) {
		reg = *((long*) (PC + base));
		opcode = reg & 0xFF000000;		// Extract the opcode
		reg &= 0xFF;					// Extract the dest reg

		// We're only interested in root level pop/pushes (ie, when fcnDepth == 0)
		switch ( opcode ) {

			case OP_MASSPUSH:
				if ( fcnDepth == 0 ) {
					pushLoc = PC;
					regsInUse = reg;	// We know what's in use by what the compiler wanted us to push
				}
				fcnDepth++;
				break;

			case OP_MASSPOP:
				fcnDepth--;
				break;

			case OP_LOADIMMED:
				PC += 4;
			case OP_LOAD:
				PC += 4;
		}

		// see what regs are in use--skip over insts not at the root level
		if ( fcnDepth == 0 ) {
			switch ( opcode ) {

				case OP_OPER:
					regsInUse |= ( 2 << reg );
				case OP_LOADIMMED:
				case OP_LOAD:
				case OP_MATHOP:
				case OP_MOVEUP:
				case OP_MOVE_FR0:
					regsInUse |= ( 1 << reg );
				break;

				// Catch the leaving a fcn at the root level
				case OP_MASSPOP:

					// Get the regs that get sub-used (ie, used between pushLoc and PC)
					progLen = mProgram.length();
					subRegs = StackReduction( pushLoc + 4, PC );

					// StackReduction() may have elminated instructions, so adjust our PC
					PC		-= progLen - mProgram.length();
					inEndPC -= progLen - mProgram.length();

					// Reassign what regs get pushed then popped.  We must push the regs that get used in the sub fcn and we use here
					regsToPush = subRegs & regsInUse;
					if ( regsToPush ) {
						inst = (long*) (base + pushLoc);
						*inst = OP_MASSPUSH | regsToPush;		// Reassign the push
						inst = (long*) (base + PC);
						*inst = OP_MASSPOP | regsToPush; }		// Reassign the pop

					// If no regs need to get pushed, delete the pop and push insts
					else {
						mProgram.Remove( PC + 1, 4 );
						mProgram.Remove( pushLoc + 1, 4 );
						PC		-= 8;
						inEndPC	-= 8;
					}
					break;
			}
		}

		// Move the PC along, for we just looked at an instruction
		PC += 4;
	}

	return regsInUse;
}

*/



