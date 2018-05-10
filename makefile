all: fractal_gpu
cpu: fractal_cpu

fractal_cpu:
	g++ src/main.cpp -pthread -O3 -std=c++11 -o fractal_display -lsfml-graphics -lsfml-window -lsfml-system

fractal_gpu: mandelbrot_gpu.o
	g++ -o fractal_display -O3 -std=c++11 src/main.cpp mandelbrot_gpu.o mandelbrot_gpu_link.o -lcudadevrt -lcudart -pthread -lsfml-graphics -lsfml-window -lsfml-system 
	#g++ -o program -L/usr/local/cuda/lib64 -lcuda -lcudart main.cpp  cudacode.o 

mandelbrot_gpu.o:
	nvcc -rdc=true -c -arch=sm_60 src/mandelbrot_gpu.cu -std=c++11
	nvcc -arch=sm_60 -dlink -o mandelbrot_gpu_link.o mandelbrot_gpu.o -lcudadevrt -lcudart

clean:
	rm -rf *o fractal_display

