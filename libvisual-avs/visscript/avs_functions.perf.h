/* C code produced by gperf version 3.0.3 */
/* Command-line: gperf ./avs_functions.perf  */
/* Computed positions: -k'1-2,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


#define TOTAL_KEYWORDS 37
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 10
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 68
/* maximum key range = 66, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static unsigned char asso_values[] =
    {
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 35, 69,
      30, 25, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69,  0, 15, 15,
      25,  0,  5,  0, 69,  5, 69, 69, 10, 50,
       0,  5, 35, 25, 10,  5,  0, 69, 69,  5,
      15, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69
    };
  return len + asso_values[(unsigned char)str[1]] + asso_values[(unsigned char)str[0]] + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static AvsBuiltinFunctionToken *
in_word_set (str, len)
     register const char *str;
     register unsigned int len;
{
  static AvsBuiltinFunctionToken wordlist[] =
    {
      {(char*)0,-1}, {(char*)0,-1}, {(char*)0,-1},
#line 10 "./avs_functions.perf"
      {"tan", AVS_BUILTIN_FUNCTION_TAN},
#line 13 "./avs_functions.perf"
      {"atan", AVS_BUILTIN_FUNCTION_ATAN},
      {(char*)0,-1}, {(char*)0,-1},
#line 42 "./avs_functions.perf"
      {"gettime", AVS_BUILTIN_FUNCTION_GETTIME},
      {(char*)0,-1},
#line 11 "./avs_functions.perf"
      {"asin", AVS_BUILTIN_FUNCTION_ASIN},
#line 43 "./avs_functions.perf"
      {"getkbmouse", AVS_BUILTIN_FUNCTION_GETKBMOUSE},
#line 36 "./avs_functions.perf"
      {"assign", AVS_BUILTIN_FUNCTION_ASSIGN},
#line 17 "./avs_functions.perf"
      {"invsqrt", AVS_BUILTIN_FUNCTION_INVSQRT},
#line 8 "./avs_functions.perf"
      {"sin", AVS_BUILTIN_FUNCTION_SIN},
#line 24 "./avs_functions.perf"
      {"sign", AVS_BUILTIN_FUNCTION_SIGN},
      {(char*)0,-1}, {(char*)0,-1},
#line 35 "./avs_functions.perf"
      {"if", AVS_BUILTIN_FUNCTION_IF},
#line 20 "./avs_functions.perf"
      {"log", AVS_BUILTIN_FUNCTION_LOG},
#line 31 "./avs_functions.perf"
      {"bnot", AVS_BUILTIN_FUNCTION_BNOT},
#line 33 "./avs_functions.perf"
      {"above", AVS_BUILTIN_FUNCTION_ABOVE},
#line 40 "./avs_functions.perf"
      {"getosc", AVS_BUILTIN_FUNCTION_GETOSC},
#line 41 "./avs_functions.perf"
      {"getspec", AVS_BUILTIN_FUNCTION_GETSPEC},
#line 7 "./avs_functions.perf"
      {"abs", AVS_BUILTIN_FUNCTION_ABS},
#line 12 "./avs_functions.perf"
      {"acos", AVS_BUILTIN_FUNCTION_ACOS},
#line 34 "./avs_functions.perf"
      {"below", AVS_BUILTIN_FUNCTION_BELOW},
      {(char*)0,-1}, {(char*)0,-1},
#line 9 "./avs_functions.perf"
      {"cos", AVS_BUILTIN_FUNCTION_COS},
#line 23 "./avs_functions.perf"
      {"ceil", AVS_BUILTIN_FUNCTION_CEIL},
#line 22 "./avs_functions.perf"
      {"floor", AVS_BUILTIN_FUNCTION_FLOOR},
      {(char*)0,-1}, {(char*)0,-1},
#line 30 "./avs_functions.perf"
      {"bor", AVS_BUILTIN_FUNCTION_BOR},
#line 16 "./avs_functions.perf"
      {"sqrt", AVS_BUILTIN_FUNCTION_SQRT},
#line 14 "./avs_functions.perf"
      {"atan2", AVS_BUILTIN_FUNCTION_ATAN2},
      {(char*)0,-1}, {(char*)0,-1}, {(char*)0,-1},
#line 28 "./avs_functions.perf"
      {"rand", AVS_BUILTIN_FUNCTION_RAND},
#line 32 "./avs_functions.perf"
      {"equal", AVS_BUILTIN_FUNCTION_EQUAL},
      {(char*)0,-1},
#line 27 "./avs_functions.perf"
      {"sigmoid", AVS_BUILTIN_FUNCTION_SIGMOID},
#line 15 "./avs_functions.perf"
      {"sqr", AVS_BUILTIN_FUNCTION_SQR},
#line 29 "./avs_functions.perf"
      {"band", AVS_BUILTIN_FUNCTION_BAND},
#line 38 "./avs_functions.perf"
      {"exec3", AVS_BUILTIN_FUNCTION_EXEC3},
      {(char*)0,-1}, {(char*)0,-1},
#line 18 "./avs_functions.perf"
      {"pow", AVS_BUILTIN_FUNCTION_POW},
      {(char*)0,-1},
#line 37 "./avs_functions.perf"
      {"exec2", AVS_BUILTIN_FUNCTION_EXEC2},
      {(char*)0,-1}, {(char*)0,-1},
#line 19 "./avs_functions.perf"
      {"exp", AVS_BUILTIN_FUNCTION_EXP},
#line 39 "./avs_functions.perf"
      {"loop", AVS_BUILTIN_FUNCTION_LOOP},
#line 21 "./avs_functions.perf"
      {"log10", AVS_BUILTIN_FUNCTION_LOG10},
      {(char*)0,-1}, {(char*)0,-1},
#line 25 "./avs_functions.perf"
      {"min", AVS_BUILTIN_FUNCTION_MIN},
      {(char*)0,-1}, {(char*)0,-1}, {(char*)0,-1},
      {(char*)0,-1}, {(char*)0,-1}, {(char*)0,-1},
      {(char*)0,-1}, {(char*)0,-1}, {(char*)0,-1},
#line 26 "./avs_functions.perf"
      {"max", AVS_BUILTIN_FUNCTION_MAX}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (s && *str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
#line 44 "./avs_functions.perf"

