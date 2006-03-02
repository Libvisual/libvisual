#ifndef _AVS_PARSER_PRIVATE_H
#define _AVS_PARSER_PRIVATE_H 1


/* Parser state initializers */
#undef INIT_STATE
#undef E
#undef R
#undef S
#undef A
#undef D

#define INIT_STATE(command, index, token, length) { (command), (index), (token), (length) }

#define E(index,token,length) INIT_STATE(AvsParserError,index,token,length)
#define R(index,token,length) INIT_STATE(AvsParserReduce,index,token,length)
#define S(index,token,length) INIT_STATE(AvsParserShift,index,token,length)
#define A(index,token,length) INIT_STATE(AvsParserAccept,index,token,length)
#define D(index,token,length) INIT_STATE(AvsParserDefault,index,token,length)

#define AVS_PARSER_STATE(ptr) (((ptr) - (parserstate[0])) / AVS_PARSER_TOKENS)
#define AVS_PARSER_TOKEN(ptr) (((ptr) - (parserstate[0])) % AVS_PARSER_TOKENS)

#define AVS_PARSER_EMPTY (-2)
#define AVS_PARSER_EOF   (0)

#endif /* !_AVS_PARSER_PRIVATE_H */
