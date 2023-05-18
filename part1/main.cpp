// Source code for Test task
// Licensed after GNU GPL v3

#include <iostream>
#include <cassert>
#include <thread>
#include <random>
#include <fstream>
#include "compressor.h"

namespace
{
std::vector<int> Generate(int min, int max, size_t size, int seed = 0)
{
	std::vector<int> result(size);
	std::mt19937 gen(seed);
	std::uniform_int_distribution<> distrib(min, max);
	std::transform(result.cbegin(), result.cend(), result.begin(), [&](auto i)
	{
		return distrib(gen);
	});
	return result;
}
}

void Test1(int min, int max, size_t size)
{
	auto sequence = Generate(min, max, size);
	CompressedData compressed;
	compressed.CompressParallel(sequence.cbegin(), sequence.cend());

	auto original_size = sequence.size() * sizeof(int);
	auto compressed_size = compressed.SizeOfData();

	std::cout << "Original size:   " << original_size << " bytes" << std::endl;
	std::cout << "Compressed size: " << compressed_size << " bytes" << std::endl;
	std::cout << "Compress ratio: " << (double)original_size / compressed_size << std::endl;

	auto decompressedData = compressed.Decompress();
	if (sequence == decompressedData)
		std::cout << "Decompressed sequense is ok" << std::endl;
	else
		std::cout << "Decompressed sequense is wrong" << std::endl;
}

void TestTime(int min, int max, size_t size)
{
	auto sequence = Generate(min, max, size);

	auto start1 = std::chrono::high_resolution_clock::now();
	CompressedData compressed;
	compressed.Compress(sequence.cbegin(), sequence.cend());
	auto end1 = std::chrono::high_resolution_clock::now();
	auto ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
	std::cout << "Compress          takes " << ms1 << " ms" << std::endl;

	auto start2 = std::chrono::high_resolution_clock::now();
	CompressedData compressed_parallel;
	compressed_parallel.CompressParallel(sequence.cbegin(), sequence.cend());
	auto end2 = std::chrono::high_resolution_clock::now();
	auto ms2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();
	std::cout << "Compress parallel takes " << ms2 << " ms" << std::endl;
	std::cout << "Time ratio: " << (double)ms1 / ms2 << std::endl;

	auto decompressed = compressed.Decompress();
	auto decomperssed_paralles = compressed_parallel.Decompress();

	if(sequence == decompressed)
		std::cout << "Decompressed sequense is ok" << std::endl;
	else
		std::cout << "Decompressed sequense is wrong" << std::endl;

	if(sequence == decomperssed_paralles)
		std::cout << "Decompressed paralleles sequense is ok" << std::endl;
	else
		std::cout << "Decompressed paralleles sequense is wrong" << std::endl;

}

int main()
{
	constexpr size_t size = 1'000'000;
	constexpr int min = 1;
	constexpr int max = 100;

	Test1(min, max, size);
	TestTime(min, max, size);

	return 0;
}
