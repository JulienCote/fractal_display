#pragma once

#include <cuda.h>
#include <cuda_runtime_api.h>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>

struct SimpleComplexGPU {
	double real;
	double imag;

    __device__
	inline SimpleComplexGPU(double r, double i) : real(r), imag(i) {};

    __device__
	inline double magnitude_squared() {
		return (double)real*real + imag*imag;
	}

    __device__
	SimpleComplexGPU operator+(const SimpleComplexGPU& num) const { return SimpleComplexGPU(real + num.real, imag + num.imag); }
	__device__
    SimpleComplexGPU operator*(const SimpleComplexGPU& num) const { return SimpleComplexGPU((real*num.real - imag*num.imag), (real*num.imag + imag*num.real)); } //(ac - bd) + (ad + bc)

    __device__
	void operator+=(const SimpleComplexGPU& num) { real += num.real; imag += num.imag; }
	__device__
    void operator*=(const SimpleComplexGPU& num) {
		 double new_real = real*num.real - imag*num.imag;
		 double new_imag = real*num.imag + imag*num.real;
		imag = new_imag;
		real = new_real;
	} //(ac - bd) + (ad + bc)
};

__device__
unsigned int mandelbrot_gpu_dev(SimpleComplexGPU value, int kDepth);
__global__
void mandelbrot_gpu(const  double xmin, const  double xmax, const  double ymin, const  double ymax, int size_x, int size_y, unsigned int* image, int depth);
void render_gpu(double k_xmin,  double k_xmax,  double k_ymin,  double k_ymax, int size_x, int size_y, unsigned int* pixels, int depth);