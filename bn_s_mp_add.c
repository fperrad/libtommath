/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is library that provides for multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library is designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@iahu.ca, http://math.libtomcrypt.org
 */
#include <tommath.h>

/* low level addition, based on HAC pp.594, Algorithm 14.7 */
int
s_mp_add (mp_int * a, mp_int * b, mp_int * c)
{
  mp_int *x;
  int     olduse, res, min, max;

  /* find sizes, we let |a| <= |b| which means we have to sort
   * them.  "x" will point to the input with the most digits
   */
  if (a->used > b->used) {
    min = b->used;
    max = a->used;
    x = a;
  } else if (a->used < b->used) {
    min = a->used;
    max = b->used;
    x = b;
  } else {
    min = max = a->used;
    x = NULL;
  }

  /* init result */
  if (c->alloc < max + 1) {
    if ((res = mp_grow (c, max + 1)) != MP_OKAY) {
      return res;
    }
  }

  olduse = c->used;
  c->used = max + 1;

  /* add digits from lower part */

  /* set the carry to zero */
  {
    register mp_digit u, *tmpa, *tmpb, *tmpc;
    register int i;

    /* alias for digit pointers */
    
    /* first input */
    tmpa = a->dp;
    
    /* second input */
    tmpb = b->dp;
    
    /* destination */
    tmpc = c->dp;

    u = 0;
    for (i = 0; i < min; i++) {
      /* Compute the sum at one digit, T[i] = A[i] + B[i] + U */
      *tmpc = *tmpa++ + *tmpb++ + u;

      /* U = carry bit of T[i] */
      u = *tmpc >> DIGIT_BIT;

      /* take away carry bit from T[i] */
      *tmpc++ &= MP_MASK;
    }

    /* now copy higher words if any, that is in A+B if A or B has more digits add those in */
    if (min != max) {
      for (; i < max; i++) {
	/* T[i] = X[i] + U */
	*tmpc = x->dp[i] + u;

	/* U = carry bit of T[i] */
	u = *tmpc >> DIGIT_BIT;

	/* take away carry bit from T[i] */
	*tmpc++ &= MP_MASK;
      }
    }

    /* add carry */
    *tmpc++ = u;

    /* clear digits above used (since we may not have grown result above) */
    for (i = c->used; i < olduse; i++) {
      *tmpc++ = 0;
    }
  }

  mp_clamp (c);
  return MP_OKAY;
}