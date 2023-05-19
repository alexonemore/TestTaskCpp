// Source code for Test task
// Licensed after GNU GPL v3

#include "oclworker.h"
#include <cassert>

#define STRINGIFY(...) #__VA_ARGS__

// ---------------------------------- OpenCL ---------------------------------
const char *findUniqueValues = STRINGIFY(
__kernel void histogram(__global int *data, int num_data,
						__global int *histogram, int num_bins
						//,__global int *result, volatile __global int *count
						)
{
	int i;
//	int lid = get_local_id(0);
	int gid = get_global_id(0);
//	int lsize = get_local_size(0);
	int gsize = get_global_size(0);

	barrier(CLK_GLOBAL_MEM_FENCE);
	for (i = gid; i < num_bins; i += gsize)
		histogram[i] = 0;

	barrier(CLK_GLOBAL_MEM_FENCE);
	for (i = gid; i < num_data; i += gsize)
		atomic_add(&histogram[data[i]], 1);

//	TODO: Data race somewhere, "return" only histogram
//	barrier(CLK_GLOBAL_MEM_FENCE);
//	count[0] = 0;

//	barrier(CLK_GLOBAL_MEM_FENCE);
//	for (i = gid; i < num_bins; i += gsize)
//		result[i] = 0;

//	barrier(CLK_GLOBAL_MEM_FENCE);
//	if(get_global_id(0) == 0)
//	{
//		//printf("get_global_id(0): %i", get_global_id(0));
//		for (i = 0; i < num_bins; i += 1)
//		{
//			if (histogram[i] == 1)
//			{
//				atomic_xchg(&result[*count], i);
//				atomic_inc(count);
//				//printf("Counter: %i", *count);
//			}
//		}
//	}
}
);
// ---------------------------------- OpenCL ---------------------------------


OCLWorker::OCLWorker()
	: platform(SelectPlatform())
	, context(GetGpuContext(platform()))
	, command_queue(context)
{
	cl::string name = platform.getInfo<CL_PLATFORM_NAME>();
	cl::string profile = platform.getInfo<CL_PLATFORM_PROFILE>();
	std::cout << "Selected: " << name << ": " << profile << std::endl;
}

cl::Platform OCLWorker::SelectPlatform()
{
	cl::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	for (auto p : platforms)
	{
		cl_uint numdevices = 0;
		::clGetDeviceIDs(p(), CL_DEVICE_TYPE_GPU, 0, NULL, &numdevices);
		if (numdevices > 0)
			return cl::Platform(p);
	}
	throw std::runtime_error("No platform selected");
}

cl::Context OCLWorker::GetGpuContext(cl_platform_id PId)
{
	cl_context_properties properties[] = {
		CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(PId),
		0 // signals end of property list
	};
	return cl::Context(CL_DEVICE_TYPE_GPU, properties);
}

namespace
{
template<typename It>
cl::vector<cl_int> ConvertHistToResult(It first, It last)
{
	cl::vector<cl_int> result;
	cl_int i = 0;
	for (It it = first; it != last; ++it, ++i)
	{
		if (*it == 1)
			result.push_back(i);
	}
	return result;
}
}

constexpr size_t local_size = 256;

std::pair<cl::vector<cl_int>, cl::vector<cl_int>>
OCLWorker::SearchUnique(cl_int* data, cl_int data_size, cl_int hist_size)
{
	size_t buffer_size = data_size * sizeof(cl_int);
	size_t buffer_result_size = hist_size * sizeof(cl_int);

	cl::Buffer Array(context, CL_MEM_READ_ONLY, buffer_size);
	cl::Buffer Hist(context, CL_MEM_WRITE_ONLY, buffer_result_size);
	//cl::Buffer Res(context, CL_MEM_WRITE_ONLY, buffer_result_size);
	//cl::Buffer Count(context, CL_MEM_WRITE_ONLY, sizeof(cl_int));

	cl::copy(command_queue, data, data + data_size, Array);

	cl::Program program(context, findUniqueValues, true);

	cl::KernelFunctor<cl::Buffer, cl_int, cl::Buffer, cl_int/*, cl::Buffer, cl::Buffer*/>
	func(program, "histogram");

	cl::NDRange GlobalRange(data_size);
	cl::NDRange LocalRange(local_size);
	cl::EnqueueArgs Args(command_queue, GlobalRange, LocalRange);

	cl::Event evt = func(Args, Array, data_size, Hist, hist_size/*, Res, Count*/);
	evt.wait();

//	cl_int count[1]{0};
//	cl::copy(command_queue, Count, count, count + 1);
//	std::cout << "COUNT: " << count[0] << std::endl;

//	cl::vector<cl_int> result(count[0]);
//	cl::copy(command_queue, Res, result.data(), result.data() + result.size());

	cl::vector<cl_int> hist(hist_size);
	cl::copy(command_queue, Hist, hist.data(), hist.data() + hist.size());

	auto result = ConvertHistToResult(hist.cbegin(), hist.cend());
	return std::make_pair(result, hist);
}



