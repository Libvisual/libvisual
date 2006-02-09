#define YYEMPTY		(-2)
#define YYEOF		0
   enum {
     IDENTIFIER = 258,
     CONSTANT = 259,
     FUNCTION = 260,
     UNARY = 261
   };

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   76

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  19
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  7
/* YYNRULES -- Number of rules. */
#define YYNRULES  25
/* YYNRULES -- Number of states. */
#define YYNSTATES  41

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   261
/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    13,     8,     2,
      16,    17,    11,     9,    18,    10,     2,    12,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    15,
       2,     6,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     7,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,    14
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     6,     8,    11,    13,    16,    18,
      21,    24,    28,    32,    36,    40,    44,    48,    52,    56,
      60,    61,    63,    67,    69,    71
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const signed char yyrhs[] =
{
      20,     0,    -1,    21,    -1,    -1,    22,    -1,    21,    22,
      -1,    15,    -1,    23,    15,    -1,    25,    -1,    10,    23,
      -1,     9,    23,    -1,    23,     9,    23,    -1,    23,    10,
      23,    -1,    23,    11,    23,    -1,    23,    12,    23,    -1,
      23,    13,    23,    -1,    23,     8,    23,    -1,    23,     7,
      23,    -1,    23,     6,    23,    -1,    16,    23,    17,    -1,
      -1,    23,    -1,    24,    18,    23,    -1,     4,    -1,     3,
      -1,     3,    16,    24,    17,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    27,    27,    28,    32,    33,    37,    38,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      55,    57,    58,    62,    63,    64
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "CONSTANT", "FUNCTION",
  "'='", "'|'", "'&'", "'+'", "'-'", "'*'", "'/'", "'%'", "UNARY", "';'",
  "'('", "')'", "','", "$accept", "translation_unit", "statement_list",
  "statement", "expr", "fn_expr", "primary", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,    61,   124,    38,    43,
      45,    42,    47,    37,   261,    59,    40,    41,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    19,    20,    20,    21,    21,    22,    22,    23,    23,
      23,    23,    23,    23,    23,    23,    23,    23,    23,    23,
      24,    24,    24,    25,    25,    25
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     1,     2,     1,     2,     1,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       0,     1,     3,     1,     1,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,    24,    23,     0,     0,     6,     0,     0,     2,     4,
       0,     8,    20,    10,     9,     0,     1,     5,     0,     0,
       0,     0,     0,     0,     0,     0,     7,    21,     0,    19,
      18,    17,    16,    11,    12,    13,    14,    15,    25,     0,
      22
};

/* YYDEFGOTO[NTERM-NUM]. */
static const signed char yydefgoto[] =
{
      -1,     7,     8,     9,    10,    28,    11
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -15
static const signed char yypact[] =
{
      22,   -14,   -15,    24,    24,   -15,    24,    23,    22,   -15,
      47,   -15,    24,   -15,   -15,    35,   -15,   -15,    24,    24,
      24,    24,    24,    24,    24,    24,   -15,    57,    -4,   -15,
      57,    63,    -5,    -1,    -1,   -15,   -15,   -15,   -15,    24,
      57
};

/* YYPGOTO[NTERM-NUM].  */
static const signed char yypgoto[] =
{
     -15,   -15,   -15,    16,    -3,   -15,   -15
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      13,    14,    12,    15,    21,    22,    23,    24,    25,    27,
      23,    24,    25,    38,    39,    30,    31,    32,    33,    34,
      35,    36,    37,    16,    17,     1,     2,     1,     2,     0,
       0,     3,     4,     3,     4,     0,    40,     5,     6,     0,
       6,    18,    19,    20,    21,    22,    23,    24,    25,     0,
       0,     0,    29,    18,    19,    20,    21,    22,    23,    24,
      25,     0,    26,    18,    19,    20,    21,    22,    23,    24,
      25,    20,    21,    22,    23,    24,    25
};

static const signed char yycheck[] =
{
       3,     4,    16,     6,     9,    10,    11,    12,    13,    12,
      11,    12,    13,    17,    18,    18,    19,    20,    21,    22,
      23,    24,    25,     0,     8,     3,     4,     3,     4,    -1,
      -1,     9,    10,     9,    10,    -1,    39,    15,    16,    -1,
      16,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    17,     6,     7,     8,     9,    10,    11,    12,
      13,    -1,    15,     6,     7,     8,     9,    10,    11,    12,
      13,     8,     9,    10,    11,    12,    13
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     9,    10,    15,    16,    20,    21,    22,
      23,    25,    16,    23,    23,    23,     0,    22,     6,     7,
       8,     9,    10,    11,    12,    13,    15,    23,    24,    17,
      23,    23,    23,    23,    23,    23,    23,    23,    17,    18,
      23
};
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "CONSTANT", "FUNCTION",
  "'='", "'|'", "'&'", "'+'", "'-'", "'*'", "'/'", "'%'", "UNARY", "';'",
  "'('", "')'", "','", "$accept", "translation_unit", "statement_list",
  "statement", "expr", "fn_expr", "primary", 0
};

