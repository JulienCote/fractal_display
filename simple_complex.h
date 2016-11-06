#pragma once

#include <string>
#include <cmath>
#include <boost/multiprecision/cpp_dec_float.hpp>

//typedef boost::multiprecision::cpp_dec_float_50 mp_number;
typedef long double mp_number;

struct SimpleComplex {
	mp_number real;
	mp_number imag;
	inline SimpleComplex(mp_number r, mp_number i) : real(r), imag(i) {};

	inline mp_number magnitude_squared() {
		return mp_number(real*real + imag*imag);
	}

	SimpleComplex operator+(const SimpleComplex& num) const { return SimpleComplex(real + num.real, imag + num.imag); }
	SimpleComplex operator*(const SimpleComplex& num) const { return SimpleComplex((real*num.real - imag*num.imag), (real*num.imag + imag*num.real)); } //(ac - bd) + (ad + bc)

	void operator+=(const SimpleComplex& num) { real += num.real; imag += num.imag; }
	void operator*=(const SimpleComplex& num) {
		mp_number new_real = real*num.real - imag*num.imag;
		mp_number new_imag = real*num.imag + imag*num.real;
		imag = new_imag;
		real = new_real;
	} //(ac - bd) + (ad + bc)
};