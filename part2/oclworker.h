// Source code for Test task
// Licensed after GNU GPL v3

#ifndef OCLWORKER_H
#define OCLWORKER_H

#include <iostream>

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

class OCLWorker
{
	cl::Platform platform;
	cl::Context context;
	cl::CommandQueue command_queue;

	static cl::Platform SelectPlatform();
	static cl::Context GetGpuContext(cl_platform_id);

public:
	OCLWorker();

	std::pair<cl::vector<cl_int>, cl::vector<cl_int>>
	SearchUnique(cl_int* data, cl_int data_size, cl_int hist_size);

};

#endif // OCLWORKER_H
