#ifndef _PARTICLES_HPP_
#define _PARTICLES_HPP_

#include <cuda.h>

//#define DEBUG
#ifdef DEBUG
	#include <iostream>
	#define WRAP_CUDA(foo) std::cerr << #foo ": " << cudaGetErrorString(foo) << std::endl;
#else
	#define WRAP_CUDA(foo) (foo)
#endif

#define PX 256
#define PY 8
#define PZ 256

void particles_init(size_t particle_count);
void particles_step(size_t particle_count);

#endif//_PARTICLES_HPP_
