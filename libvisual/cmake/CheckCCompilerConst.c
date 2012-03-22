/* Copied from GNU autoconf's c.m4 file (which is GPL) */
#ifndef __cplusplus
int main() {
  /* Ultrix mips cc rejects this.  */
  typedef int charset[2];
  const charset x;
  /* SunOS 4.1.1 cc rejects this.  */
  char const *const *ccp;
  char **p;
  /* NEC SVR4.0.2 mips cc rejects this.  */
  struct point {int x, y;};
  static struct point const zero = {0,0};
  /* AIX XL C 1.02.0.0 rejects this.
     It does not let you subtract one const X* pointer from another in
     an arm of an if-expression whose if-part is not a constant
     expression */
  const char *g = "string";
  ccp = &g + (g ? g-g : 0);
  /* HPUX 7.0 cc rejects these. */
  ++ccp;
  p = (char**) ccp;
  ccp = (char const *const *) p;
  { /* SCO 3.2v4 cc rejects this.  */
    char *t;
    char const *s = 0 ? (char *) 0 : (char const *) 0;

    *t++ = 0;
  }
  { /* Someone thinks the Sun supposedly-ANSI compiler will reject this.  */
    int x[] = {25, 17};
    const int *foo = &x[0];
    ++foo;
  }
  { /* Sun SC1.0 ANSI compiler rejects this -- but not the above. */
    typedef const int *iptr;
    iptr p = 0;
    ++p;
  }
  { /* AIX XL C 1.02.0.0 rejects this saying
       "k.c", line 2.27: 1506-025 (S) Operand must be a modifiable lvalue. */
    struct s { int j; const int *ap[3]; };
    struct s *b; b->j = 5;
  }
  { /* ULTRIX-32 V3.1 (Rev 9) vcc rejects this */
    const int foo = 10;
  }
}
#endif
