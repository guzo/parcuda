#include "particles.hpp"

#include <cuda.h>
#include <curand_kernel.h>
#include <cuda_gl_interop.h>

#include <ctime>

#include <iostream>
#include <cmath>

using namespace std;

extern cudaGraphicsResource_t cudavbo;

__device__ __constant__ curandState* curand_states;


__device__ float3 operator+(float3 a, float3 b) {
	return make_float3(a.x+b.x, a.y+b.y, a.z+b.z);
}

__global__ void particles_init_gpu(float3* p, unsigned long long seed) {
	unsigned int x  = blockIdx.x*blockDim.x + threadIdx.x;
	unsigned int y  = blockIdx.y*blockDim.y + threadIdx.y;
	unsigned int z  = blockIdx.z*blockDim.z + threadIdx.z;
	unsigned int i  = x + y*PX*PZ + z*PX;
	curandState* state = curand_states + (x + y + z)*(x%16);

	curand_init(seed, state - curand_states, 0, state);
	p[i*2] = make_float3(
		(float(x)/float(PX) + curand_normal(state)*.05f)*2.f-1.f,
		(float(y)/float(PY) + curand_normal(state)*.4f ),
		(float(z)/float(PZ) + curand_normal(state)*.05f)*2.f-1.f - 2
	);
	p[i*2+1] = make_float3(
		0,
		.002f*sin(float(z)/float(PZ)*3.14),
		.01
	);
}

__global__ void particles_step_gpu(float3* p) {
	unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
	unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;
	unsigned int z = blockIdx.z*blockDim.z + threadIdx.z;
	unsigned int i = x + y*PX*PZ + z*PX;
	curandState* state = curand_states + (x + y + z)*(x%16);

	float3 v = p[i*2+1] + make_float3(0.f,-.002f,0.f);
	float3 s = p[i*2  ] + v;
	if(s.y < -1) {
		v   = make_float3(v.x, -v.y, v.z);
		s.y = -1;
	}
	if(s.z > 1) {
		s.z = -1;
		s.y = .25*curand_normal(state);
		v.y *= 1 + .05f*curand_normal(state);
		if(v.y > .05)
			v.y = .002*curand_normal(state);
	}
	p[i*2]   = s;
	p[i*2+1] = v;
}

void particles_init(size_t particle_count) {
	size_t dummy = particle_count;
	float3* v = 0;

	float* randsd;
	WRAP_CUDA(cudaMalloc(reinterpret_cast<void**>(&randsd), (PX+PY+PZ)*16*sizeof(*curand_states)));
	WRAP_CUDA(cudaMemcpyToSymbol(curand_states, &randsd, sizeof(*curand_states)));

	WRAP_CUDA(cudaGraphicsMapResources(1, &cudavbo));
	WRAP_CUDA(cudaGraphicsResourceGetMappedPointer((void**)&v, &dummy, cudavbo));

	dim3 block(4,2,4);
	dim3 grid(PX/4,PY/2,PZ/4);
	particles_init_gpu<<<grid, block>>>(v, time(0));

	WRAP_CUDA(cudaGraphicsUnmapResources(1, &cudavbo));
}

void particles_step(size_t particle_count) {
	size_t dummy = particle_count;
	float3* v = 0;

	WRAP_CUDA(cudaGraphicsMapResources(1, &cudavbo));
	WRAP_CUDA(cudaGraphicsResourceGetMappedPointer((void**)&v, &dummy, cudavbo));

	dim3 block(4,2,4);
	dim3 grid(PX/4,PY/2,PZ/4);
	particles_step_gpu<<<grid, block>>>(v);

	WRAP_CUDA(cudaGraphicsUnmapResources(1, &cudavbo));
}
