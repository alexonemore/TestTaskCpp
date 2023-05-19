// Source code for Test task
// Licensed after GNU GPL v3

#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <cassert>
#include <climits>
#include "oclworker.h"


template<typename It>
void GenerateRandom(It first, It last, int min, int max, int seed = 0)
{
	std::mt19937 gen(seed);
	std::uniform_int_distribution<> distrib(min, max);
	std::transform(first, last, first, [&](auto i)
	{
		return distrib(gen);
	});
}

template<typename Vector_int>
Vector_int GenerateWithUnique(size_t size, int unique_count, int max_value, int seed = 0)
{
	assert(size > unique_count);
	assert((size - unique_count) % 2 == 0);
	assert(max_value > unique_count);
	assert(size - unique_count >= 2);
	Vector_int result(size);
	auto first = result.begin();
	auto last_unique = std::next(first, unique_count);
	std::iota(first, last_unique, 0);
	auto bsize = (size - unique_count) / 2;
	auto middle_non_unique = std::next(last_unique, bsize);
	GenerateRandom(last_unique, middle_non_unique, unique_count, max_value, seed);
	std::copy(last_unique, middle_non_unique, middle_non_unique);
	std::mt19937 gen(seed);
	std::shuffle(first, result.end(), gen);
	return result;
}

template<typename Vector_int>
Vector_int FindUniqueOnCPU(const Vector_int& source)
{
	std::unordered_map<int, int> frequency;
	for (auto i : source)
		frequency[i]++;
	Vector_int unique;
	for (auto [value, count] : frequency)
		if (count == 1)
			unique.push_back(value);
	std::sort(unique.begin(), unique.end());
	return unique;
}

template<typename It>
cl::vector<cl_int> MakeHistOnCPU(It first, It last, size_t hist_size)
{
	cl::vector<cl_int> hist(hist_size, 0);
	for (It it = first; it != last; ++it)
		hist[*it]++;
	return hist;
}

void TestGenerate(size_t size, int unique_count)
{
	auto vector_with_unique = GenerateWithUnique<cl::vector<cl_int>>(size, unique_count, INT_MAX);
	std::vector<int> expected(unique_count);
	std::iota(expected.begin(), expected.end(), 0);
	auto finded_unique = FindUniqueOnCPU(vector_with_unique);
	if (finded_unique == expected)
		std::cout << "TestGenerate " << size << " " << unique_count << ": OK" << std::endl;
	else
		std::cout << "TestGenerate " << size << " " << unique_count << ": WRONG" << std::endl;
}

void TestGPUSearchUnique1()
{
	cl::vector<cl_int> data{ 2, 3, 2, 4, 4, 5, 6, 7, 8, 5 };
	cl::vector<cl_int> expected{ 3, 6, 7, 8 };
	cl_int hist_size = 9;
	auto cpu_hist = MakeHistOnCPU(data.cbegin(), data.cend(), hist_size);

	OCLWorker gpu_worker;
	auto [gpu_result, gpu_hist] = gpu_worker.SearchUnique(data.data(), data.size(), hist_size);

	if (cpu_hist == gpu_hist)
		std::cout << "TestGPUSearchUnique2: hist OK" << std::endl;
	else
		std::cout << "TestGPUSearchUnique2: hist WRONG" << std::endl;

	if (expected == gpu_result)
		std::cout << "TestGPUSearchUnique1: result OK" << std::endl;
	else
		std::cout << "TestGPUSearchUnique1: result WRONG" << std::endl;
}


void TestGPUSearchUnique2(size_t size, int unique_count)
{
	auto data = GenerateWithUnique<cl::vector<cl_int>>(size, unique_count, unique_count + 10);
	auto cpu_result = FindUniqueOnCPU(data);
	cl_int hist_size = 1 + *std::max_element(data.cbegin(), data.cend());
	auto cpu_hist = MakeHistOnCPU(data.cbegin(), data.cend(), hist_size);

	OCLWorker gpu_worker;
	auto [gpu_result, gpu_hist] = gpu_worker.SearchUnique(data.data(), data.size(), hist_size);

//	std::copy(gpu_result.cbegin(), gpu_result.cend(), std::ostream_iterator<int>(std::cout, " "));
//	std::cout << std::endl;

	if (cpu_hist == gpu_hist)
		std::cout << "TestGPUSearchUnique2 " << size << " " << unique_count << ": hist OK" << std::endl;
	else
		std::cout << "TestGPUSearchUnique2 " << size << " " << unique_count << ": hist WRONG" << std::endl;

	if (cpu_result == gpu_result)
		std::cout << "TestGPUSearchUnique2 " << size << " " << unique_count << ": result OK" << std::endl;
	else
		std::cout << "TestGPUSearchUnique2 " << size << " " << unique_count << ": result WRONG" << std::endl;
}

int main() try
{
	TestGenerate(100, 10);
	TestGenerate(100, 10);
	TestGenerate(200, 10);
	TestGenerate(1'000, 10);
	TestGenerate(10'000'000, 1'000);
	TestGPUSearchUnique1();
	TestGPUSearchUnique2(100, 10);
	TestGPUSearchUnique2(200, 10);
	TestGPUSearchUnique2(1'000, 10);
	TestGPUSearchUnique2(10'000, 500);
	TestGPUSearchUnique2(20'000, 1'000);
	TestGPUSearchUnique2(100'000, 1'000);
	TestGPUSearchUnique2(1'000'000, 1'000);
	TestGPUSearchUnique2(10'000'000, 1'000);
	return 0;
}
catch (cl::Error &err)
{
	std::cerr << "OCL ERROR " << err.err() << ":" << err.what() << std::endl;
	return -1;
}
catch (std::runtime_error &err)
{
	std::cerr << "RUNTIME ERROR " << err.what() << std::endl;
	return -1;
}
catch (...)
{
	std::cerr << "UNKNOWN ERROR\n";
	return -1;
}
