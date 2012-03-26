#include <config.h>

#include <stdio.h>
#include <gettext.h>
#include <stdlib.h>

#include <stdint.h>

#include "Expression.h"

#include "ExpressionDict.h"
#include "ExprVirtualMachine.h"


#if EG_WIN || defined(UNIX_X)
#include "EgOSUtils.h"
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



#define __collapseTwo											\
	highestPriority = 0;										\
	for ( i = 0; i < numExps-1; i++ ) {								\
		priority = oper[ i ] & 0x0F00;								\
		if ( priority > highestPriority ) {							\
			highestPriority	= priority;							\
			collapse		= i;							\
		}											\
	}												\
	i = oper[ 0 ] & 0x0F00;										\
	if ( numExps > 2 && i == 0x100 && ( (oper[ 1 ] & 0x0F00) == 0x100 ) )		\
		collapse = 0; /* Catch case:  a + b + c * d ^  */					\
	i = collapse;											\
	inVM.DoOp( expr[ i ], expr[ i + 1 ], oper[ i ] & 0xFF );					\
	inVM.DeallocReg( expr[ i + 1 ] );								\
	numExps--;											\
	oper[ i ] = oper[ i+1 ];									\
	for ( i++; i < numExps; i++ ) {									\
		oper[ i ] = oper[ i+1 ];								\
		expr[ i ] = expr[ i+1 ];								\
	}



//	Pre:	All whitespace and illegal chars are removed
//	Pre:	Parens balence & all letters are capitalized
//	Note:	I normally don't write such huge, non-compartmentalized code, but speed is important
//	Post:	Allocates a register for the return value and returns that register
int Expression::Compile( const char* inStr, long inLen, ExpressionDict& inDict, ExprVirtualMachine& inVM ) {
	bool			hasLetters = false;
	float*			floatPtr, val;
	long			highestPriority, priority, i, numExps = 0, startPos = 0;
	long			fcnCall, collapse, c, pos, parens = 0, firstParen = -1;
	short			oper[ 4 ];
	short			expr[ 4 ];
	ExprUserFcn**	fcnPtr;

	// Catch a negated expression.  Make it the same as 0-...
	if ( inLen > 0 && *inStr == '-' ) {
		expr[ 0 ] = inVM.AllocReg();
		oper[ 0 ] = 0x100 | '-';
		inVM.Loadi( 0.0, expr[ 0 ] );
		numExps = 1;
		inStr++;
		inLen--;
	}

	// Make a pass thru of the string, finding the starts and ends of all root exprs
	for ( pos = 0; pos < inLen; pos++ ) {
		c = inStr[ pos ];

		if ( c == '(' ) {
			if ( firstParen < 0 )
				firstParen = pos;
			parens++;  }
		else if ( c == ')' )
			parens--;

		// Don't consider any chars if we're not at the root level
		if ( parens == 0 ) {

			if ( c >= 'A' && c <= 'Z' )
				hasLetters = true;

			// Operators are what separate exprs
			switch ( c ) {
				case '-':
				case '+':	priority = 0x0100;	goto doOp;
				case '/':
				case '%':
				case '*':	priority = 0x0200;	goto doOp;
				case '^':	priority = 0x0300;

					// Close cur expr
doOp:				if ( pos > startPos && pos + 1 < inLen ) {

						// Recurse
						expr[ numExps ] = Compile( inStr + startPos, pos - startPos, inDict, inVM );
						oper[ numExps ] = c | priority;
						numExps++;
						startPos = pos + 1;

						// 4 exprs, given three priority operators, 4 exprs guaruntees we can collapse two into one expr
						if ( numExps == 4 ) {
							__collapseTwo
						}
					}
			}
		}
	}


	// Detect base case (ie, whole string is an expr)
	if ( numExps == 0 ) {

		// See if we found a fcn call or an unneeded paren pair: (...)
		if ( firstParen >= 0 && firstParen <= 4 ) {

			// Eval what's inside the parens
			expr[ 0 ] = Compile( inStr + firstParen + 1, inLen - firstParen - 2, inDict, inVM );

			// if we have a fcn (as opposed to just a paren pair)
			if ( firstParen > 0 ) {

				// Translate the string of the fcn to a (one byte) sub op code number
				fcnCall = *((int32_t*) inStr);
				#if EG_WIN || (defined(UNIX_X) && !defined(WORDS_BIGENDIAN))
				fcnCall = EgOSUtils::RevBytes( fcnCall );
				#endif

#define VAL(a,b,c,d) (((a)<<24)+((b)<<16)+((c)<<8)+(d))
				switch ( fcnCall ) {
					case VAL('S','Q','R','T'):	fcnCall = cSQRT;	break;
					case VAL('A','T','A','N'):	fcnCall = cATAN;	break;
					case VAL('A','B','S','('):	fcnCall = cABS;		break;
					case VAL('S','I','N','('):	fcnCall = cSIN;		break;
					case VAL('C','O','S','('):	fcnCall = cCOS;		break;
					case VAL('T','A','N','('):	fcnCall = cTAN;		break;
					case VAL('L','O','G','('):	fcnCall = cLOG;		break;
					case VAL('E','X','P','('):	fcnCall = cEXP;		break;
					case VAL('S','Q','R','('):	fcnCall = cSQR;		break;
					case VAL('S','Q','W','V'):	fcnCall = cSQWV;	break;
					case VAL('F','L','O','R'):	fcnCall = cFLOR;	break;
					case VAL('P','O','S','('):	fcnCall = cPOS;		break;
					case VAL('R','A','N','D'):
					case VAL('R','N','D','('):	fcnCall = cRND;		break;
					case VAL('S','G','N','('):	fcnCall = cSGN;		break;
					case VAL('T','R','W','V'):	fcnCall = cTRWV;	break;
					case VAL('C','L','I','P'):	fcnCall = cCLIP;	break;
					case VAL('S','E','E','D'):	fcnCall = cSEED;	break;
					case VAL('W','R','A','P'):	fcnCall = cWRAP;	break;
					case VAL('T','R','N','C'):	fcnCall = cTRNC;	break;
					default: {
						fcnCall = 0;
						UtilStr temp( inStr, firstParen );
						fcnPtr = inDict.LookupFunc( temp );
						inVM.UserFcnOp( expr[ 0 ], fcnPtr );
					}
				}
#undef VAL
				if ( fcnCall )
					inVM.MathOp( expr[ 0 ], fcnCall );
			} }

		// Catch the case where we have a number/immediate
		else if ( ! hasLetters ) {
			val = UtilStr::GetFloatVal( inStr, inLen );
			expr[ 0 ] = inVM.AllocReg();
			inVM.Loadi( val, expr[ 0 ] ); }

		// At this point we assume it's an identifier, so we'll look up its value
		else {
			UtilStr temp;
			temp.Assign( inStr, inLen );
			expr[ 0 ] = inVM.AllocReg();
			floatPtr = inDict.LookupVar( temp );
			if ( floatPtr )
				inVM.Loadi( floatPtr, expr[ 0 ] );
		} }

	// If there more than one expression
	else {

		// Finish the current expr
		if ( startPos < inLen ) {
			expr[ numExps ] = Compile( inStr + startPos, inLen - startPos, inDict, inVM );
			numExps++;
		}

		// When there'll be no more exprs, we're free to collapse all the exprs into one
		while ( numExps > 1 ) {
			__collapseTwo
		}
	}

	return expr[ 0 ];
}





/*







#define __collapseTwo															\
	highestPriority = 0;														\
	for ( i = 0; i < numExps-1; i++ ) {											\
		priority = oper[ i ] & 0x0F00;											\
		if ( priority > highestPriority ) {										\
			highestPriority	= priority;											\
			collapse		= i;												\
		}																		\
	}																			\
	i = collapse;																\
	inVM.DoOp( i, oper[ i ] & 0xFF );											\
	numExps--;																	\
	oper[ i ] = oper[ i+1 ];													\
	for ( i++; i < numExps; i++ ) {												\
		inVM.MoveUp( i );														\
		oper[ i ] = oper[ i+1 ];												\
	}



//	Pre:	All whitespace and illegal chars are removed
//	Pre:	Parens balence & all letters are capitalized
//	Note:	I normally don't write such huge, non-compartmentalized code, but speed is important
void Expression::Compile( char* inStr, long inLen, const Hashtable& inDict, ExprVirtualMachine& inVM ) {
	bool			hasLetters = false;
	double*			dblPtr, val;
	long			highestPriority, priority, i, numExps = 0, startPos = 0;
	long			fcnCall, collapse, c, pos, parens = 0, firstParen = -1;
	short			oper[ 4 ];
	short			reg[ 4 ];

	// Catch a negated expression
	if ( inLen > 0 && *inStr == '-' ) {
		inVM.Loadi( 0.0 );
		oper[ 0 ] = 0x100 | '-';
		numExps = 1;
		inStr++;
		inLen--;
	}

	// Make a pass thru of the string, finding the starts and ends of all root exprs
	for ( pos = 0; pos < inLen; pos++ ) {
		c = inStr[ pos ];

		if ( c == '(' ) {
			if ( firstParen < 0 )
				firstParen = pos;
			parens++;  }
		else if ( c == ')' )
			parens--;

		// Don't consider any chars if we're not at the root level
		if ( parens == 0 ) {

			if ( c >= 'A' && c <= 'Z' )
				hasLetters = true;

			// Operators are what separate exprs
			switch ( c ) {
				case '-':
				case '+':	priority = 0x0100;	goto doOp;
				case '/':
				case '%':
				case '*':	priority = 0x0200;	goto doOp;
				case '^':	priority = 0x0300;

					// Close cur expr
doOp:				if ( pos > startPos && pos + 1 < inLen ) {

						// Push registers 0 thru (numExps-1), the registers in use
						inVM.MassPush( numExps - 1 );

						// Recurse
						Compile( inStr + startPos, pos - startPos, inDict, inVM );
						oper[ numExps ] = c | priority;
						numExps++;
						startPos = pos + 1;

						// The return value is in reg 0. Also pop/restore the FP registers in use
						inVM.Move( numExps - 1 );
						inVM.MassPop( numExps - 2 );

						// 4 exprs, given three priority operators, 4 exprs guaruntees we can collapse two into one expr
						if ( numExps == 4 ) {
							__collapseTwo
						}
					}
			}
		}
	}


	// Detect base case (ie, whole string is an expr)
	if ( numExps == 0 ) {

		// See if we found a fcn call or an unneeded paren pair: (...)
		if ( firstParen >= 0 && firstParen <= 4 ) {

			// Eval what's inside the parens
			Compile( inStr + firstParen + 1, inLen - firstParen - 2, inDict, inVM );

			// Translate the string of the fcn to a (one byte) code number
			fcnCall = *((long*) inStr);
			switch ( fcnCall ) {
				#if EG_MAC || (defined(UNIX_X) && defined(WORDS_BIGENDIAN))
				case 'SQRT':	fcnCall = cSQRT;		break;
				case 'ATAN':	fcnCall = cATAN;		break;
				case 'ABS(':	fcnCall = cABS;			break;
				case 'SIN(':	fcnCall = cSIN;			break;
				case 'COS(':	fcnCall = cCOS;			break;
				case 'TAN(':	fcnCall = cTAN;			break;
				case 'LOG(':	fcnCall = cLOG;			break;
				case 'EXP(':	fcnCall = cEXP;			break;
				case 'SQR(':	fcnCall = cSQR;			break;
				case 'SQWV':	fcnCall = cSQWV;		break;
				case 'POS(':	fcnCall = cPOS;			break;
				case 'RND(':	fcnCall = cRND;			break;
				#elif EG_WIN || (defined(UNIX_X) && !defined(WORDS_BIGENDIAN))
				case 'TRQS':	fcnCall = cSQRT;		break;
				case 'NATA':	fcnCall = cATAN;		break;
				case '(SBA':	fcnCall = cABS;			break;
				case '(NIS':	fcnCall = cSIN;			break;
				case '(SOC':	fcnCall = cCOS;			break;
				case '(NAT':	fcnCall = cTAN;			break;
				case '(GOL':	fcnCall = cLOG;			break;
				case '(PXE':	fcnCall = cEXP;			break;
				case '(RQS':	fcnCall = cSQR;			break;
				case 'VWQS':	fcnCall = cSQWV;		break;
				case '(SOP':	fcnCall = cPOS;			break;
				case '(DNR':	fcnCall = cRND;			break;
				#endif
				default:		fcnCall	= 0;			break;
			}
			inVM.MathOp( fcnCall );  }

		// Catch the case where we have a number/immediate
		else if ( ! hasLetters ) {
			val = UtilStr::GetFloatVal( inStr, inLen );
			inVM.Loadi( val ); }

		// At this point we assume it's an identifier, so we'll look up its value
		else {
			UtilStr temp;
			temp.Assign( inStr, inLen );
			if ( inDict.Get( &temp, &dblPtr ) )
				inVM.Loadi( dblPtr );
		} }

	// If there more than one expression
	else {

		// Push registers 0 thru (numExps-1), the registers in use
		inVM.MassPush( numExps - 1 );

		// Finish the current expr
		if ( startPos < inLen ) {
			Compile( inStr + startPos, inLen - startPos, inDict, inVM );
			numExps++;
		}

		// The return value is in reg 0. Also pop/restore the FP registers in use
		inVM.Move( numExps - 1 );
		inVM.MassPop( numExps - 2 );

		// When there'll be no more exprs, we're free to collapse all the exprs into one
		while ( numExps > 1 ) {
			__collapseTwo
		}

	}

	// Register 0 is the return value by convention
}


*/




bool Expression::GetNextToken( UtilStr& outStr, long& ioPos ) {
	char* str = mEquation.getCStr();
	long pos = ioPos, len = mEquation.length();
	char c;

	if ( ioPos < 0 )
		ioPos = 0;

	c = str[ pos ];
	while ( ( c < 'A' || c > 'Z' )  && pos < len ) {
		pos++;
		c = str[ pos ];
	}

	outStr.Wipe();
	while ( ( ( c >= 'A' && c <= 'Z' ) || ( c == '_' ) || ( c >= '0' && c <= '9' ) )  && pos < len ) {
		outStr.Append( c );
		pos++;
		c = str[ pos ];
	}

	ioPos = pos;

	return outStr.length() > 0;
}



bool Expression::IsDependent( const char* inStr ) {
 	long pos, len = 0, c;

 	while ( inStr[ len ] )
 		len++;

 	pos = mEquation.contains( inStr, len, 0, false );
 	while ( pos > 0 ) {

 		// This expr is depndent on inStr when we find a substring match that *isn't* a substring of another identifier
 		c = mEquation.getChar( pos - 1 );
 		if ( c < 'A' || c > 'Z' ) {
 			c = mEquation.getChar( pos + len );
 			if ( c < 'A' || c > 'Z' )
 				return true;
 		}
 		pos = mEquation.contains( inStr, len, pos, false );
 	}

 	return false;
}




bool Expression::Weight( Expression& inExpr, float* inC1, float* inC2 ) {

	if ( mIsCompiled && inExpr.mIsCompiled ) {
		Chain( inExpr, inC1, inC2 );
		return true; }
	else
		return false;
}




void Expression::Assign( Expression& inExpr ) {

	mEquation.Assign( inExpr.mEquation );
	mIsCompiled = inExpr.mIsCompiled;

	if ( inExpr.mIsCompiled )
		ExprVirtualMachine::Assign( inExpr );
}




bool Expression::Compile( const UtilStr& inStr, ExpressionDict& inDict ) {
	int i, c, parens, len;

	mEquation.Assign( inStr );

	// Case insensitive, remove all spaces
	mEquation.Capitalize();
	mEquation.Remove( " " );
	mEquation.Remove( "\t" );

	// Check for balenced parens
	parens = 0;
	len = mEquation.length();
	for ( i = 1; i <= len && parens >= 0; i++ ) {
		c = mEquation.getChar( i );

		switch ( c ) {
			case '(':	parens++;	break;
			case ')':	parens--;	break;
		}
	}

	if ( parens != 0 ) {
		mEquation.Wipe();
		mIsCompiled = false; }
	else {

		// Wipe whatever program was in the VM before
		Clear();

		if ( len > 0 ) {

			// Generate insts for the VM that evaluate mEquation
			int retRegNum = Compile( mEquation.getCStr(), mEquation.length(), inDict, *this );
			Move( retRegNum, 0 );  }
		else {

			// Color register 0 as used
			AllocReg();

			// If the expression is blank, make it return 0
			Loadi( 0.0, 0 );
		}


		PrepForExecution();
		mIsCompiled = true;
	}

	return mIsCompiled;
}

