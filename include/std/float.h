#ifndef __FLOAT_STD_H_INCLUDE__

#define HUGE_VAL (__builtin_huge_val())

/* Radix of exponent representation */
#undef FLT_RADIX
#define FLT_RADIX 2
/* Number of base-FLT_RADIX digits in the significand of a float */
#undef FLT_MANT_DIG
#define FLT_MANT_DIG 24
/* Number of decimal digits of precision in a float */
#undef FLT_DIG
#define FLT_DIG 6
/* Addition rounds to 0: zero, 1: nearest, 2: +inf, 3: -inf, -1: unknown */
#undef FLT_ROUNDS
#define FLT_ROUNDS 1
/* Difference between 1.0 and the minimum float greater than 1.0 */
#undef FLT_EPSILON
#define FLT_EPSILON 1.19209290e-07F
/* Minimum int x such that FLT_RADIX**(x-1) is a normalised float */
#undef FLT_MIN_EXP
#define FLT_MIN_EXP (-125)
/* Minimum normalised float */
#undef FLT_MIN
#define FLT_MIN 1.17549435e-38F
/* Minimum int x such that 10**x is a normalised float */
#undef FLT_MIN_10_EXP
#define FLT_MIN_10_EXP (-37)
/* Maximum int x such that FLT_RADIX**(x-1) is a representable float */
#undef FLT_MAX_EXP
#define FLT_MAX_EXP 128
/* Maximum float */
#undef FLT_MAX
#define FLT_MAX 3.40282347e+38F
/* Maximum int x such that 10**x is a representable float */
#undef FLT_MAX_10_EXP
#define FLT_MAX_10_EXP 38

/* Number of base-FLT_RADIX digits in the significand of a double */
#undef DBL_MANT_DIG
#define DBL_MANT_DIG 53
/* Number of decimal digits of precision in a double */
#undef DBL_DIG
#define DBL_DIG 15
/* Difference between 1.0 and the minimum double greater than 1.0 */
#undef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131e-16
/* Minimum int x such that FLT_RADIX**(x-1) is a normalised double */
#undef DBL_MIN_EXP
#define DBL_MIN_EXP (-1021)
/* Minimum normalised double */
#undef DBL_MIN
#define DBL_MIN 2.2250738585072014e-308
/* Minimum int x such that 10**x is a normalised double */
#undef DBL_MIN_10_EXP
#define DBL_MIN_10_EXP (-307)
/* Maximum int x such that FLT_RADIX**(x-1) is a representable double */
#undef DBL_MAX_EXP
#define DBL_MAX_EXP 1024
/* Maximum double */
#undef DBL_MAX
#define DBL_MAX 1.7976931348623157e+308
/* Maximum int x such that 10**x is a representable double */
#undef DBL_MAX_10_EXP
#define DBL_MAX_10_EXP 308

#ifdef CONFIG_SOFT_FLOAT
/* Without soft-float, without long double */

/* Number of base-FLT_RADIX digits in the significand of a long double */
#undef LDBL_MANT_DIG
#define LDBL_MANT_DIG 53
/* Number of decimal digits of precision in a long double */
#undef LDBL_DIG
#define LDBL_DIG 15
/* Difference between 1.0 and the minimum long double greater than 1.0 */
#undef LDBL_EPSILON
#define LDBL_EPSILON 2.2204460492503131e-16L
/* Minimum int x such that FLT_RADIX**(x-1) is a normalised long double */
#undef LDBL_MIN_EXP
#define LDBL_MIN_EXP (-1021)
/* Minimum normalised long double */
#undef LDBL_MIN
#define LDBL_MIN 2.2250738585072014e-308L
/* Minimum int x such that 10**x is a normalised long double */
#undef LDBL_MIN_10_EXP
#define LDBL_MIN_10_EXP (-307)
/* Maximum int x such that FLT_RADIX**(x-1) is a representable long double */
#undef LDBL_MAX_EXP
#define LDBL_MAX_EXP 1024
/* Maximum long double */
#undef LDBL_MAX
#define LDBL_MAX 1.7976931348623157e+308L
/* Maximum int x such that 10**x is a representable long double */
#undef LDBL_MAX_10_EXP
#define LDBL_MAX_10_EXP 308
#endif

#if defined(LDBL_MANT_DIG)

typedef long double float_max_t;

#define FPMAX_TYPE           3

#define FPMAX_MANT_DIG       LDBL_MANT_DIG
#define FPMAX_DIG            LDBL_DIG
#define FPMAX_EPSILON        LDBL_EPSILON
#define FPMAX_MIN_EXP        LDBL_MIN_EXP
#define FPMAX_MIN            LDBL_MIN
#define FPMAX_MIN_10_EXP     LDBL_MIN_10_EXP
#define FPMAX_MAX_EXP        LDBL_MAX_EXP
#define FPMAX_MAX            LDBL_MAX
#define FPMAX_MAX_10_EXP     LDBL_MAX_10_EXP

#elif defined(DBL_MANT_DIG)

typedef double float_max_t;
#define FPMAX_TYPE           2

#define FPMAX_MANT_DIG       DBL_MANT_DIG
#define FPMAX_DIG            DBL_DIG
#define FPMAX_EPSILON        DBL_EPSILON
#define FPMAX_MIN_EXP        DBL_MIN_EXP
#define FPMAX_MIN            DBL_MIN
#define FPMAX_MIN_10_EXP     DBL_MIN_10_EXP
#define FPMAX_MAX_EXP        DBL_MAX_EXP
#define FPMAX_MAX            DBL_MAX
#define FPMAX_MAX_10_EXP     DBL_MAX_10_EXP

#elif defined(FLT_MANT_DIG)

typedef float float_max_t;
#define FPMAX_TYPE           1

#define FPMAX_MANT_DIG       FLT_MANT_DIG
#define FPMAX_DIG            FLT_DIG
#define FPMAX_EPSILON        FLT_EPSILON
#define FPMAX_MIN_EXP        FLT_MIN_EXP
#define FPMAX_MIN            FLT_MIN
#define FPMAX_MIN_10_EXP     FLT_MIN_10_EXP
#define FPMAX_MAX_EXP        FLT_MAX_EXP
#define FPMAX_MAX            FLT_MAX
#define FPMAX_MAX_10_EXP     FLT_MAX_10_EXP

#else
#error unable to determine appropriate type for float_max_t!
#endif

#ifndef DECIMAL_DIG

#if !defined(FLT_RADIX) || (FLT_RADIX != 2)
#error unable to compensate for missing DECIMAL_DIG!
#endif

/*  ceil (1 + #mantissa * log10 (FLT_RADIX)) */
#define DECIMAL_DIG   (1 + (((FPMAX_MANT_DIG * 100) + 331) / 332))

#endif /* DECIMAL_DIG */

/* The following checks in an float_max_t is either 0 or +/- infinity.
 *
 * WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!
 *
 * This only works if float_max_t is the actual maximal floating point type
 * used in intermediate calculations.  Otherwise, excess precision in the
 * intermediate values can cause the test to fail.
 *
 * WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!
 */
#define iszoinf(x)		((x) == ((x)/4))

/* Fairly portable nan check.  Bitwise for i386 generated larger code.
 * If you have a better version, comment this out.
 */
#define isnan(x)		((x) != (x))

/* Without seminumerical functions to examine the sign bit, this is
 * about the best we can do to test for '-0'.
 */
#define isnegz(x)		((1./(x)) < 0)

#endif /* __FLOAT_STD_H_INCLUDE__ */
