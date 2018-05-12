#include <cuda.h>
#include <cuda_runtime_api.h>

#include <iostream>
#include <string>

#include "mandelbrot_gpu.cuh"

__device__
unsigned int mandelbrot_gpu_dev(SimpleComplexGPU value, int kDepth) {
    SimpleComplexGPU base = value;
    for (int i = 0; i < kDepth; ++i) {
        if (4 < value.magnitude_squared()) {
            return i;
        }
        value *= value;
        value += base;
    }
    return 0;
}

__global__
void mandelbrot_gpu(const double xmin, const  double xmax, const  double ymin, const  double ymax, int size_x, int size_y, unsigned int* image, int depth) {
    int id = threadIdx.x + blockIdx.x * blockDim.x;
    int nb_pixel = size_x * size_y;

    if (id >= nb_pixel) return;

    int proper_x = id % size_x;
    int proper_y = id / size_x;

    double delta_x = ((proper_x * 1.0) / size_x) * (xmax - xmin);
    double delta_y = ((proper_y * 1.0) / size_y) * (ymax - ymin);

    SimpleComplexGPU coordinate(xmin + delta_x, ymin + delta_y);
    unsigned int deviation = mandelbrot_gpu_dev(coordinate, depth);

    if (deviation > 0) {
        unsigned int r = ((deviation + 7) % 255) << 16;
        unsigned int g = ((deviation + 41) % 255) << 8;
        unsigned int b = (deviation + 127) % 255;
        image[id] = r | g | b;
    }
}

__host__
void render_gpu(double k_xmin,  double k_xmax,  double k_ymin,  double k_ymax, int size_x, int size_y, unsigned int* pixels, int depth) {
    unsigned int* d_image = 0;

    const int nb_pixel = size_x * size_y;

    cudaMalloc((void**)&d_image, nb_pixel * sizeof(unsigned int));

    mandelbrot_gpu <<<(int)(nb_pixel/1024)+1, 1024>>>(k_xmin, k_xmax, k_ymin, k_ymax, size_x, size_y, d_image, depth);
    cudaDeviceSynchronize();

    cudaMemcpy(pixels, d_image, nb_pixel * sizeof(unsigned int), cudaMemcpyKind::cudaMemcpyDeviceToHost);
    cudaFree(d_image);
}