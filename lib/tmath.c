#include <tmath.h>

// A estimation of Pi
const long double PI_LD = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
const double PI_D = (double)PI_LD;
// A estimation of e
const long double E_LD =  2.718281828459045235360287471352662497757247093699959574966967627724076630354;
const double E_D = (double)E_LD;
// Constant for equality comparison for floating point numbers
const long double EQUAL_EPSILON_LD = 1e-7;
const double EQUAL_EPSILON_D = 1e-7;

double tmath_absd(const double x)
{
	if (x < 0) return -x;
	else return x;
}

double tmath_powd(const double x, const long n)
{
	double r = 1;
	if (n < 0) {
		return 1 / tmath_powd(x, -n);
	}

	for (long i = 1; i <= n; i++) {
		r *= x;
	}
	return r;
}

double tmath_facd(const long n)
{
	double r = 1;
	for (long i = 2; i <= n; i++) {
		r *= (double)i;
	}
	return r;
}

double tmath_oddfacd(const long n)
{
	double r = 1;
	for (long i = 3; i <= n; i += 2) {
		r *= (double)i;
	}
	return r;
}

long tmath_floord(const double x)
{
	long truncated = (long)x;
	if (x < 0) {
		if (truncated > x) {
			return truncated - 1;
		} else {
			return truncated;
		}
	}
	else {
		return truncated;
	}
}

long tmath_ceild(const double x)
{
	long truncated = (long)x;
	if (x < 0) {
		return truncated;
	} else {
		if (truncated < x) {
			return truncated + 1;
		} else {
			return truncated;
		}
	}
}

double tmath_modd(const double x, const double y)
{
	return y * ((x / y) - tmath_floord(x / y));
}

double tmath_expd(const double x)
{
	double r = 0;
	for (long n = 0; n <= 15L; n++)
	{
		r += tmath_powd(x, n) / tmath_facd(n);
	}
	return r;
}

double tmath_lnd(const double _x)
{
	const double x = (_x - 1) / (_x + 1);
	double r = 0;
	for (long n = 0; n <= 100L; n++)
	{
		r += 2 * tmath_powd(x, 2 * n + 1) / (2 * n + 1);
	}
	return r;
}

double tmath_lgd(const double x)
{
	return tmath_lnd(x) / tmath_lnd(10);
}

double tmath_lbd(const double x)
{
	return tmath_lnd(x) / tmath_lnd(2);
}

double tmath_logd(const double x, const double n)
{
	return tmath_lnd(x) / tmath_lnd(n);
}

double tmath_sind(const double _x)
{
	const double x = tmath_modd(_x + PI_D, 2 * PI_D) - PI_D;
	double r = 0;
	for (long n = 0; n <= 8L; n++) {
		r += tmath_powd((double)-1, n) * tmath_powd(x, 2 * n + 1) / tmath_facd(2 * n + 1);
	}
	return r;
}

double tmath_asind(const double x)
{
	double r = 0;
	double delta = 1;
	for (long n = 1; delta > 1e-6; n++)
	{
		long odd = 2 * n - 1;
		double oddf = tmath_oddfacd(odd - 2);
		double f = tmath_facd(odd);
		double p = tmath_powd(x, odd);
		double d = p / f * oddf * oddf;
		delta = tmath_absd(d);
		r += p / f * oddf * oddf;
	}
	return r;
}

double tmath_sinhd(const double x)
{
	return 0.5 * (tmath_expd(x) - tmath_expd(-x));
}

double tmath_cosd(const double _x)
{
	const double x = tmath_modd(_x + PI_D, 2 * PI_D) - PI_D;
	double r = 0;
	for (long n = 0; n <= 8L; n++) {
		r += tmath_powd((double)-1.0, n) * tmath_powd(x, 2 * n) / tmath_facd(2 * n);
	}
	return r;
}

double tmath_acosd(const double x)
{
	return PI_D / 2 - tmath_asind(x);
}

double tmath_coshd(const double x)
{
	return 0.5 * (tmath_expd(x) + tmath_expd(-x));
}
