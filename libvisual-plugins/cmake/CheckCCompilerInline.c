#ifndef __cplusplus
// Check wether compiler accepts inlining for both functions and static
// functions and for which keyword among inline, __inline__, __inline
typedef int foo_t;
static POSSIBLE_INLINE_KEYWORD foo_t static_foo () { return 0; }
       POSSIBLE_INLINE_KEYWORD foo_t        foo () { return 0; }
int main()
{
   (void)static_foo();
   (void)foo();
}
#endif

