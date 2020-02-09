//
//  catenary.hpp
//
//  Copyright (c) 2017 ushiostarfish
//  zlib License
//  https://github.com/Ushio/Catenaly
//  ----
//  Note: (by @shspage)
//  The following code (inside the include guard) is a part of above repository, extracted as is.
//  ----

#ifndef __catenary_hpp__
#define __catenary_hpp__

namespace catenary {
	template <int n>
	static double Pow(double v) {
		static_assert(n > 0, "Power can’t be negative");
		double n2 = Pow<n / 2>(v);
		return n2 * n2 * Pow<n & 1>(v);
	}
	template <> double Pow<1>(double v) { return v; }
	template <> double Pow<0>(double v) { return 1.0; }

	// important
	// h: holizontal distance
	// v: vertical distance

	// T = - Sqrt(s^2 - v^2)
	// f(a) = 2*a*sinh(h/(a2)) + T
	// for finding a, f(a) == 0
	inline double f(double a, double h, double sqrt_s2_minus_v2) {
		double T = -sqrt_s2_minus_v2;
		return 2.0 * a * std::sinh(h / (a * 2.0)) + T;
	}
	// f'(a)
	// D[2*a*sinh(h/(2a)) + T, a]
	inline double dfdx(double a, double h) {
		return 2.0 * std::sinh(h / (2.0 * a)) - (h * std::cosh(h / (2.0 * a)) / a);
	}

	// taylor series 2a*sinh(h/(2a)) + T
	inline double f_taylor(double a, double h, double sqrt_s2_minus_v2) {
		double T = -sqrt_s2_minus_v2;
		// T + h + h^3 / (24 a^2) + h^5 / (1920 a^4)
		// T + h * { 1.0 + h^2 / (24 a^2) + h^4 / (1920 a^4) }
		// T + h * { 1.0 + (h^2 / (a^2)) { (1/24 + h^2/(1920 a^2) } }
		// T + h * { 1.0 + (h^2 / (a^2)) { (1/24 + (h^2 / (a^2)) / 1920) } }
		double a2 = a * a;
		double a4 = a2 * a2;
		double h2 = h * h;
		double h2_over_a2 = h2 / a2;
		return std::fma(
			h,
			std::fma(
				h2_over_a2,
				std::fma(h2_over_a2, 1.0 / 1920.0, 1.0 / 24.0),
				1.0),
			T);
	}

	// f(a) = y
	// f^(-1)(y) = a
	inline double f_inverse_taylor(double y, double h, double sqrt_s2_minus_v2) {
		double T = -sqrt_s2_minus_v2;

		// constant
		static const double k_one_over_4_sqrt_15 = 1.0 / (4.0 * std::sqrt(15.0));
		static const double k_sqrt_5 = std::sqrt(5.0);

		// Solve[T+h + h^3/(24 a^2) + h^5/(1920 a^4)==x, a]
		// CForm(sqrt(-(sqrt(5) sqrt(-h^5 (h + 6 T - 6 x)) + 5 h^3)/(h + T - x))/(4 sqrt(15)))
		return std::sqrt(-(k_sqrt_5 * std::sqrt(-(Pow<5>(h) * (h + 6.0 * T - 6.0 * y))) + 5 * Pow<3>(h)) / (h + T - y)) * k_one_over_4_sqrt_15;
	}

	// find a, f(a) = y
	// f^(-1)(y) = a
	inline double f_inverse_newton(double y, double h, double sqrt_s2_minus_v2, int *iteratedCount = nullptr) {
		double T = -sqrt_s2_minus_v2;
		double eps = 1.0e-9;
		double x_cur = f_inverse_taylor(y, h, sqrt_s2_minus_v2);
		double f_value = f(x_cur, h, sqrt_s2_minus_v2) - y;

		if (iteratedCount) {
			*iteratedCount = 1;
		}

		for (int i = 0; i < 100; ++i) {
			if (std::isfinite(x_cur) == false) {
				break;
			}
			if (std::fabs(f_value) < eps) {
				break;
			}
			double move = -f_value / dfdx(x_cur, h);
			x_cur = x_cur + move;
			f_value = f(x_cur, h, sqrt_s2_minus_v2) - y;

			if (iteratedCount) {
				(*iteratedCount)++;
			}
		}
		return x_cur;
	}
	inline double solve_a(double h, double v, double s) {
		return f_inverse_newton(0.0, h, sqrt(s * s - v * v));
	}

	struct Curve {
		double a = 1.0;
		double S = 0.0;
		double T = 0.0;
		double evaluate(double x) const {
			return a * std::cosh((x + S) / a) + T;
		}
		double evaluate_dfdx(double x) const {
			return std::sinh((x + S) / a);
		}
		double x_for_s(double s) const {
			return a * std::asinh(s / a + std::sinh(S / a)) - S;
		}
	};

	/*
	(x1, x2) = (0, 0)
	(x2, y2) = (X, Y)
	length: s Catenary curve
	*/
	inline Curve curve(double X, double Y, double s) {
		Curve c;
		c.a = solve_a(X, std::fabs(Y), s);
		c.S = c.a * std::asinh(Y / (c.a * 2.0 * std::sinh(X / (2.0 * c.a)))) - X * 0.5;
		c.T = -c.a * std::cosh(c.S / c.a);
		return c;
	}

	// double L_actual = c.a * std::sinh((X + c.S) / c.a) - c.a * std::sinh(c.S / c.a);
	// printf("%.3f\n", L_actual);
}

#endif /* Catenary_hpp */
